/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_fd.h"

int main(int argc, char * argv[])
{
    const char * program = (const char *)0;
    const char * Address = (const char *)0;
    const char * Interface = (const char *)0;
    const char * Port = (const char *)0;
    const char * Server =  (const char *)0;
    const char * Rendezvous = (const char *)0;
    const char * Blocksize = (const char *)0;
    diminuto_ipv4_t address4 = DIMINUTO_IPC4_LOOPBACK;
    diminuto_ipv6_t address6 = DIMINUTO_IPC6_LOOPBACK;
    diminuto_ipv4_t server4 = DIMINUTO_IPC4_LOOPBACK;
    diminuto_ipv6_t server6 = DIMINUTO_IPC6_LOOPBACK;
    diminuto_ipv4_t datum4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t datum6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_port_t port46 = 0;
    diminuto_port_t rendezvous46 = 0;
    diminuto_port_t datum46 = 0;
    size_t blocksize = 4096;
    char string[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")] = { '\0' };
    char * buffer = (char *)0;
    ssize_t input = 0;
    ssize_t output = 0;
    diminuto_unsigned_t value;
    diminuto_mux_t mux;
    int Protocol = 0;
    int sock = -2;
    int fd = -3;
    int rc = 0;
    int fds = 0;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    SETLOGMASK();

    while ((opt = getopt(argc, argv, "46?A:P:a:b:i:p:")) >= 0) {
        switch (opt) {
        case '4':
            Protocol = 4; /* IPv4 */
            break;
        case '6':
            Protocol = 6; /* IPv6 */
            break;
        case 'a':
            Address = optarg; /* near end bind address */
            break;
        case 'b':
            Blocksize = optarg; /* input/output blocksize */
            break;
        case 'i':
            Interface = optarg; /* near end interface */
            break;
        case 'p':
            Port = optarg; /* near end bind port number */
            break;
        case 'A':
            Server = optarg; /* far end server address */
            break;
        case 'P':
            Rendezvous = optarg; /* far end port number */
            break;
        case '?':
            fprintf(stderr, "usage: %s [ -? ] [ -4 | -6 ] [ -a NEADDR ] [ -p NEPORT ] [ -i NEINTF ] [ -A FEADDR ] [ -P FEPORT ] [ -b BYTES ]\n", program);
            return 1;
            break;
        default:
            break;
        }
    }

    TEST();

    if (Protocol == 4) {
        COMMENT("Protocol=%d\n", Protocol);
    } else if (Protocol == 6) {
        COMMENT("Protocol=%d\n", Protocol);
    } else {
        FATAL("Protocol=%d\n", Protocol);
    }

    if (Address == (const char *)0) {
        /* Do nothing. */
    } else if (Protocol == 4) {
        COMMENT("Address4=\"%s\"\n", Address);
        address4 = diminuto_ipc4_address(Address);
    } else if (Protocol == 6) {
        COMMENT("Address6=\"%s\"\n", Address);
        address6 = diminuto_ipc6_address(Address);
    } else {
        /* Do nothing. */
    }

    if (Protocol == 4) {
        COMMENT("address4=%s\n",  diminuto_ipc4_address2string(address4, string, sizeof(string)));
    } else if (Protocol == 6) {
        COMMENT("address6=%s\n",  diminuto_ipc6_address2string(address6, string, sizeof(string)));
    } else {
        /* Do nothing. */
    }

    if (Port == (const char *)0) {
        /* Do nothing. */
    } else {
        COMMENT("Port46=\"%s\"\n", Port);
        port46 = diminuto_ipc_port(Port, "tcp");
    }

    COMMENT("port46==%d\n", port46);

    if (Server == (const char *)0) {
        /* Do nothing. */
    } else if (Protocol == 4) {
        COMMENT("Server4=\"%s\"\n", Server);
        server4 = diminuto_ipc4_address(Server);
    } else if (Protocol == 6) {
        COMMENT("Server6=\"%s\"\n", Server);
        server6 = diminuto_ipc6_address(Server);
    } else {
        /* Do nothing. */
    }

    if (Protocol == 4) {
        COMMENT("server4==%s\n", diminuto_ipc4_address2string(server4, string, sizeof(string)));
    } else if (Protocol == 6) {
        COMMENT("server6==%s\n", diminuto_ipc6_address2string(server6, string, sizeof(string)));
    } else {
        /* Do nothing. */
    }

    if (Rendezvous == (const char *)0) {
        /* Do nothing. */
    } else {
        COMMENT("Rendezvous46=\"%s\"\n", Rendezvous);
        rendezvous46 = diminuto_ipc_port(Rendezvous, "tcp");
    }

    COMMENT("rendezvous46=%d\n", rendezvous46);

    if (Blocksize == (const char *)0) {
        /* Do nothing. */
    } else if (*diminuto_number_unsigned(Blocksize, &value) != '\0') {
        FATAL("Blocksize=\"%s\"\n", Blocksize);
    } else {
        COMMENT("Blocksize=\"%s\"\n", Blocksize);
        blocksize = value;
    }

    COMMENT("blocksize=%zu\n", blocksize);

    TEST();

    if (Protocol == 4) {
        if (Rendezvous == (const char *)0) {
            sock = diminuto_ipc4_stream_provider_specific(address4, port46, Interface, -1);
            ASSERT(sock >= 0);
            rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
            ASSERT(rc >= 0);
            COMMENT("end=near sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
        } else {
            fd = diminuto_ipc4_stream_consumer_specific(server4, rendezvous46, address4, port46, Interface);
            ASSERT(fd >= 0);
            rc = diminuto_ipc4_nearend(fd, &datum4, &datum46);
            ASSERT(rc >= 0);
            COMMENT("end=near fd=%d datum4=%s datum46=%d\n", fd, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
            rc = diminuto_ipc4_farend(fd, &datum4, &datum46);
            ASSERT(rc >= 0);
            COMMENT("end=far fd=%d datum4=%s datum46=%d\n", fd, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
        }
    } else if (Protocol == 6) {
        if (Rendezvous == (const char *)0) {
            sock = diminuto_ipc6_stream_provider_specific(address6, port46, Interface, -1);
            ASSERT(sock >= 0);
            rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
            ASSERT(rc >= 0);
            COMMENT("end=near sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
        } else {
            fd = diminuto_ipc6_stream_consumer_specific(server6, rendezvous46, address6, port46, Interface);
            ASSERT(fd >= 0);
            rc = diminuto_ipc6_nearend(fd, &datum6, &datum46);
            ASSERT(rc >= 0);
            COMMENT("end=near fd=%d datum6=%s datum46=%d\n", fd, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
            rc = diminuto_ipc6_farend(fd, &datum6, &datum46);
            ASSERT(rc >= 0);
            COMMENT("end=far fd=%d datum6=%s datum46=%d\n", fd, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
        }
    } else {
        /* Do nothing. */
    }

    buffer = (char *)malloc(blocksize);
    ASSERT(buffer != (char *)0);

    TEST();

    diminuto_mux_init(&mux);

    if (Rendezvous == (const char *)0) {
        rc = diminuto_mux_register_accept(&mux, sock);
        ASSERT(rc >= 0);
        while (!0) {
            fds = diminuto_mux_wait(&mux, -1);
            ASSERT(fds > 0);
            while (!0) {
                fd = diminuto_mux_ready_accept(&mux);
                if (fd < 0) {
                    break;
                }
                if (Protocol == 4) {
                    fd = diminuto_ipc4_stream_accept(fd, &datum4, &datum46);
                    ASSERT(fd >= 0);
                    COMMENT("end=far fd=%d datum4=%s datum46=%d\n", fd, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
                } else if (Protocol == 6) {
                    fd = diminuto_ipc6_stream_accept(fd, &datum6, &datum46);
                    ASSERT(fd >= 0);
                    COMMENT("end=far fd=%d datum6=%s datum46=%d\n", fd, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
                } else {
                    /* Do nothing. */
                }
                rc = diminuto_mux_register_read(&mux, fd);
                ASSERT(rc >= 0);
            }
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                input = diminuto_fd_read(fd, buffer, 1, blocksize);
                ASSERT(input >= 0);
                if (input == 0) {
                    rc = diminuto_ipc_close(fd);
                    ASSERT(rc >= 0);
                    rc = diminuto_mux_unregister_read(&mux, fd);
                    ASSERT(rc >= 0);
                    COMMENT("end=far fd=%d input=%zd\n", fd, input);
                } else {
                    output = diminuto_fd_write(fd, buffer, input, input);
                    ASSERT(output == input);
                }
            }
        }
        /* Can never reach here. */
        ASSERT(fds == 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        ASSERT(rc >= 0);
        rc = diminuto_ipc_close(sock);
        ASSERT(rc >= 0);
    } else {
        rc = diminuto_mux_register_read(&mux, fd);
        ASSERT(rc >= 0);
        rc = diminuto_mux_register_write(&mux, fd);
        ASSERT(rc >= 0);
        while (!0) {
        }
        rc = diminuto_mux_unregister_read(&mux, fd);
        ASSERT(rc >= 0);
        rc = diminuto_mux_unregister_write(&mux, fd);
        ASSERT(rc >= 0);
    }

    diminuto_mux_fini(&mux);

    EXIT();
}
