/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2023 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (mailto:coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * THIS IS A WORK IN PROGRESS.
 * Conestoga is a project to pass packets containing arbitary binary data
 * in a UDP-like manner over an asynchronous serial connection. It will be
 * used in conjunction with the Fothergill project, which uses USB-powered
 * paired LoRa radios which enumerate as serial ports on the USB bus.
 * Conestoga uses the Diminuto Framer feature to do this. Framer implements
 * a packet framing mechanism using byte stuffing in a manner similar to
 * HDLC and PPP.
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

static const size_t MAXDATAGRAM = diminuto_maximumof(uint16_t) - 8;

static const char MASKPATH[] = "/var/run";

typedef enum Role {
    CLIENT  = 'c',
    SERVER  = 's',
} role_t;

int main(int argc, char * argv[])
{
    diminuto_path_t maskfile = { '\0', };
    extern char * optarg;
    int opt = '\0';
    char * endptr = (char *)0;
    const char * program = (const char *)0;
    const char * device = "-"; /* Defaults to stdin and stdout. */
    int baudrate = 57600; /* Defaults to SparkFun serial LoRa radio pair. */
    int stopbits = 1;
    int databits = 8;
    int paritybit = 0; /* None. */
    int modemcontrol = 0;
    int rtscts = 0;
    int xonxoff = 0;
    role_t role = CLIENT;
    diminuto_ipc_preference_t preference = DIMINUTO_IPC_PREFERENCE_NONE;
    size_t bufsize = MAXDATAGRAM;
    diminuto_ticks_t timeout = 1000000000;
    unsigned long milliseconds = 0;
    diminuto_ipc_endpoint_t endpoint = { 0, };
    int rc = -1;
    int file = -1;
    int sock = -1;
    int fds = 0;
    int fd = -1;
    FILE * input = (FILE *)0;
    FILE * output = (FILE *)0;
    diminuto_mux_t multiplexor = { 0 };
    diminuto_ipv4_t service4 = DIMINUTO_IPC4_UNSPECIFIED_INIT;
    diminuto_ipv6_t service6 = DIMINUTO_IPC6_UNSPECIFIED_INIT;
    diminuto_port_t serviceport = 0;
    diminuto_ipv4_t sender4 = DIMINUTO_IPC4_UNSPECIFIED_INIT;
    diminuto_ipv6_t sender6 = DIMINUTO_IPC6_UNSPECIFIED_INIT;
    diminuto_port_t senderport = 0;
    bool error = false;
    bool done = false;
    ssize_t sent = 0;
    ssize_t received = 0;
    diminuto_framer_t framer = DIMINUTO_FRAMER_INIT;
    diminuto_framer_t * ff = (diminuto_framer_t *)0;
    uint8_t * frame = (uint8_t *)0;
    uint8_t * datagram = (uint8_t *)0;
    diminuto_endpoint_buffer_t endpointstring = { '\0', };

    /*
     * SETUP
     */

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    (void)diminuto_core_enable();

    (void)snprintf(maskfile, sizeof(maskfile), "%s/%s-%d.msk", MASKPATH, program, getpid());
    if (diminuto_fs_type(maskfile) == DIMINUTO_FS_TYPE_FILE) {
        (void)diminuto_log_importmask(maskfile);
    }

    (void)diminuto_log_setmask();

    /*
     * PARSING
     */

    while ((opt = getopt(argc, argv, "124678B:D:E:b:cdehmnorst:x?")) >= 0) {

        switch (opt) {

        case '1':
            stopbits = 1;
            break;

        case '2':
            stopbits = 2;
            break;

        case '4':
            preference = DIMINUTO_IPC_PREFERENCE_IPV4;
            break;

        case '6':
            preference = DIMINUTO_IPC_PREFERENCE_IPV6;
            break;

        case '7':
            databits = 7;
            break;

        case '8':
            databits = 8;
            break;

        case 'B':
            baudrate = strtoul(optarg, &endptr, 0);
            if ((baudrate <= 0) || ((endptr != (char *)0) && (*endptr != '\0'))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            }
            break;

        case 'D':
            if ((strcmp(optarg, "-") != 0) && (diminuto_fs_type(optarg) == DIMINUTO_FS_TYPE_NONE)) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            }
            device = optarg;
            break;

        case 'E':
            rc = diminuto_ipc_endpoint_prefer(optarg, &endpoint, preference);
            if (rc < 0) {
                diminuto_perror(optarg);
                error = true;
            } else if ((endpoint.type != DIMINUTO_IPC_TYPE_IPV4) && (endpoint.type != DIMINUTO_IPC_TYPE_IPV6)) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else if ((role == CLIENT) && ((endpoint.udp == 0) || ((endpoint.type == DIMINUTO_IPC_TYPE_IPV4) && diminuto_ipc4_is_unspecified(&endpoint.ipv4)) || ((endpoint.type == DIMINUTO_IPC_TYPE_IPV6) && diminuto_ipc6_is_unspecified(&endpoint.ipv6)))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else if ((role == SERVER) && ((endpoint.udp == 0) || (!diminuto_ipc4_is_unspecified(&endpoint.ipv4)) || (!diminuto_ipc6_is_unspecified(&endpoint.ipv6)))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else {
                /* Do nothing. */
            }
            break;

        case 'b':
            bufsize = strtoul(optarg, &endptr, 0);
            if ((bufsize <= 0) || ((endptr != (char *)0) && (*endptr != '\0'))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else if (bufsize > MAXDATAGRAM) {
                bufsize = MAXDATAGRAM;
            } else {
                /* Do nothing. */
            }
            break;

        case 'c':
            role = CLIENT;
            break;

        case 'd':
            rc = diminuto_daemon(program);
            if (rc < 0) {
                error = true;
            }
            break;

        case 'e':
            paritybit = 2;
            break;

        case 'm':
            modemcontrol = !0;
            break;

        case 'n':
            paritybit = 0;
            break;

        case 'o':
            paritybit = 1;
            break;

        case 'r':
            rtscts = !0;
            break;

        case 's':
            role = SERVER;
            break;

        case 't':
            milliseconds = strtoul(optarg, &endptr, 0);
            if ((milliseconds < 0) || ((endptr != (char *)0) && (*endptr != '\0'))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else {
                timeout = diminuto_frequency_units2ticks(milliseconds, 1000);
            }
            break;

        case 'x':
            xonxoff = !0;
            break;

        case '?':
            fprintf(stderr, "       -?                   prints this help menu and exits.\n");
            fprintf(stderr, "       -1                   sets DEVICE to one stop bits.\n");
            fprintf(stderr, "       -2                   sets DEVICE to two stop bits.\n");
            fprintf(stderr, "       -4                   to prefer IPv4 (must preceed -E).\n");
            fprintf(stderr, "       -6                   to prefer IPv6 (must preceed -E).\n");
            fprintf(stderr, "       -7                   sets DEVICE to seven data bits.\n");
            fprintf(stderr, "       -8                   sets DEVICE to eight data bits.\n");
            fprintf(stderr, "       -B BAUDRATE          sets the DEVICE to BAUDRATE bits per second.\n");
            fprintf(stderr, "       -D DEVICE            is the serial device name.\n");
            fprintf(stderr, "       -E HOST:PORT         sets the far end point for the proxy client.\n");
            fprintf(stderr, "       -E :PORT             sets the near end point for the proxy server.\n");
            fprintf(stderr, "       -b BYTES             sets the buffer sizes to BYTES bytes.\n");
            fprintf(stderr, "       -c                   sets proxy client mode (must preceed -E).\n");
            fprintf(stderr, "       -d                   immediately daemonizes the process.\n");
            fprintf(stderr, "       -e                   sets DEVICE to even parity.\n");
            fprintf(stderr, "       -m                   enables modem control.\n");
            fprintf(stderr, "       -o                   sets the DEVICE to odd parity.\n");
            fprintf(stderr, "       -r                   enables RTS/CTS.\n");
            fprintf(stderr, "       -s                   sets proxy server mode (must preceed -E).\n");
            fprintf(stderr, "       -t MILLISECONDS      sets the multiplexor timeout to MILLISECONDS.\n");
            fprintf(stderr, "       -x                   enables XON/XOFF.\n");
            exit(1);
            break;

        }
 
    }

    if (error) {
        exit(2);
    }

    DIMINUTO_LOG_INFORMATION("%s: role %c\n", program, role);

    /*
     * SIGNAL HANDLERS
     */

    rc = diminuto_hangup_install(!0);
    if (rc < 0) {
        error = true;
    }

    rc = diminuto_terminator_install(!0);
    if (rc < 0) {
        error = true;
    }

    if (error) {
        exit(3);
    }

    /*
     * DEVICE
     */

    DIMINUTO_LOG_INFORMATION("%s: device %s %d%c%d %s %s %s\n", program, device, databits, (paritybit == 0) ? 'n' : ((paritybit % 2) == 0) ? 'e' : 'o', stopbits, modemcontrol ? "modemcontrol" : "local", xonxoff ? "xonxoff" : "", rtscts ? "rtscts" : "");

    if (strcmp(device, "-") == 0) {
        input = stdin;
        output = stdout;
    } else if ((fd = open(device, O_RDWR)) < 0) {
        error = true;
    } else if ((input = output = fdopen(fd, "a+")) == (FILE *)0) {
        error = true;
    } else if (!diminuto_serial_valid(fd)) {
        /* Do nothing. */
    } else if (diminuto_serial_set(fd, baudrate, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts) < 0) {
        error = true;
    } else if (diminuto_serial_raw(fd) < 0) {
        error = true;
    } else {
        /* Do nothing. */
    }

    if (error) {
        diminuto_perror(device);
        exit(4);
    }

    file = fileno(input);

    /*
     * SOCKET
     */

    DIMINUTO_LOG_INFORMATION("%s: endpoint %s\n", program, diminuto_ipc_endpoint2string(&endpoint, &endpointstring, sizeof(endpointstring)));

    if ((endpoint.type != DIMINUTO_IPC_TYPE_IPV4) && (endpoint.type != DIMINUTO_IPC_TYPE_IPV6)) {
        errno = EINVAL;
        diminuto_perror("-E");
        exit(5);
    }

    switch (role) {
    case CLIENT:
        switch (endpoint.type) {
        case DIMINUTO_IPC_TYPE_IPV4:
            sock = diminuto_ipc4_datagram_peer(0);
            service4 = endpoint.ipv4;
            break;
        case DIMINUTO_IPC_TYPE_IPV6:
            sock = diminuto_ipc6_datagram_peer(0);
            service6 = endpoint.ipv6;
            break;
        default:
            diminuto_assert(false);
        }
        serviceport = endpoint.udp;
        break;
    case SERVER:
        switch (endpoint.type) {
        case DIMINUTO_IPC_TYPE_IPV4:
            sock = diminuto_ipc4_datagram_peer(endpoint.udp);
            break;
        case DIMINUTO_IPC_TYPE_IPV6:
            sock = diminuto_ipc6_datagram_peer(endpoint.udp);
            break;
        default:
            diminuto_assert(false);
            break;
        }
        break;
    }

    if (sock < 0) {
        exit(6);
    }

    /*
     * MULTIPLEXER
     */

    (void)diminuto_mux_init(&multiplexor);

    rc = diminuto_mux_register_read(&multiplexor, file);
    if (rc < 0) {
        error = true;
    }

    rc = diminuto_mux_register_read(&multiplexor, sock);
    if (rc < 0) {
        error = true;
    }

    if (error) {
        exit(7);
    }

    /*
     * BUFFERS
     */

    frame = (uint8_t *)malloc(bufsize);
    if (frame == (uint8_t *)0) {
        diminuto_perror("malloc");
        error = true;
    }

    datagram = (uint8_t *)malloc(bufsize);
    if (datagram == (uint8_t *)0) {
        diminuto_perror("malloc");
        error = true;
    }

    if (error) {
        exit(8);
    }

    /*
     * FRAMER
     */

    ff = diminuto_framer_init(&framer, frame, bufsize);
    if (ff == (diminuto_framer_t *)0) {
        error = true;
    }

    if (error) {
        exit(9);
    }

    /*
     * WORK LOOP
     */

    do {

        if (diminuto_hangup_check()) {
            DIMINUTO_LOG_NOTICE("%s: SIGHUP\n", program);
            if (diminuto_fs_type(maskfile) == DIMINUTO_FS_TYPE_FILE) {
                (void)diminuto_log_importmask(maskfile);
            }
        }

        if (diminuto_terminator_check()) {
            DIMINUTO_LOG_NOTICE("%s: SIGTERM\n", program);
            break;
        }

        /*
         * WAITING
         */

        fds = diminuto_mux_wait(&multiplexor, timeout);
        if (fds == 0) {
            diminuto_yield();
            continue;
        } else if (fds > 0) {
            /* Do nothing. */
        } else if (errno == EINTR) {
            diminuto_yield();
            continue;
        } else {
            break;
        }

        while ((fds--) > 0) {

            fd = diminuto_mux_ready_read(&multiplexor);

            if (fd == sock) {

                switch (endpoint.type) {
                case DIMINUTO_IPC_TYPE_IPV4:
                    received = diminuto_ipc4_datagram_receive_generic(sock, datagram, bufsize, &sender4, &senderport, 0);
                    break;
                case DIMINUTO_IPC_TYPE_IPV6:
                    received = diminuto_ipc6_datagram_receive_generic(sock, datagram, bufsize, &sender6, &senderport, 0);
                    break;
                default:
                    diminuto_assert(false);
                    break;
                }
                if (received <= 0) {
                    done = true;
                    break;
                } else {
                    sent = diminuto_framer_writer(output, &framer, datagram, received);
                    if (sent <= 0) {
                        done = true;
                        break;
                    }
                }


            } else if (fd == file) {

                received = diminuto_framer_reader(input, &framer);
                if (received == 0) {
                    /* Do nothing. */
                } else if (received < 0) {
                    done = true;
                    break;
                } else {
                    diminuto_framer_reset(&framer);
                    switch (role) {
                    case CLIENT:
                        switch (endpoint.type) {
                        case DIMINUTO_IPC_TYPE_IPV4:
                            sent = diminuto_ipc4_datagram_send(sock, frame, received, service4, serviceport);
                            break;
                        case DIMINUTO_IPC_TYPE_IPV6:
                            sent = diminuto_ipc6_datagram_send(sock, frame, received, service6, serviceport);
                            break;
                        default:
                            diminuto_assert(false);
                        }
                    case SERVER:
                        switch (endpoint.type) {
                        case DIMINUTO_IPC_TYPE_IPV4:
                            sent = diminuto_ipc4_datagram_send(sock, frame, received, sender4, senderport);
                            break;
                        case DIMINUTO_IPC_TYPE_IPV6:
                            sent = diminuto_ipc6_datagram_send(sock, frame, received, sender6, senderport);
                            break;
                        default:
                            diminuto_assert(false);
                        }
                        break;
                    default:
                        diminuto_assert(false);
                        break;
                    }
                    if (sent <= 0) {
                        done = true;
                        break;
                    }
                }

            } else {

                diminuto_yield();

            }
        }

    } while (!done);

    /*
     * FINALIZATING
     */

    (void)diminuto_mux_unregister_read(&multiplexor, sock);

    (void)diminuto_mux_unregister_read(&multiplexor, file);

    diminuto_mux_fini(&multiplexor);

    (void)diminuto_ipc_close(sock);

    (void)fclose(input);
    if (input != output) {
        (void)fclose(output);
    }

    free(frame);
    free(datagram);

    exit(0);
}
