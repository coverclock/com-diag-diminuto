/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * EXAMPLES
 *
 * loopback -\?
 *
 * loopback -6 -p 5555                                                              # Service Provider
 * loopback -6 -A 2001:470:4b:4e2:e79:7f1e:21f5:9355 -P 5555 < OLDFILE > NEWFILE    # Service Consumer
 *
 * loopback -4 -p 5555                                                              # Service Provider
 * loopback -4 -A 192.168.2.182 -P 5555 < OLDFILE > NEWFILE                         # Service Consumer
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#undef NDEBUG
#include <assert.h>

int main(int argc, char * argv[])
{
    const char * program = (const char *)0;
    const char * Address = (const char *)0;
    const char * Interface = (const char *)0;
    const char * Port = (const char *)0;
    const char * Server =  (const char *)0;
    const char * Rendezvous = (const char *)0;
    const char * Blocksize = (const char *)0;
    diminuto_ipv4_t address4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t address6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_ipv4_t server4 = DIMINUTO_IPC4_LOOPBACK;
    diminuto_ipv6_t server6 = DIMINUTO_IPC6_LOOPBACK;
    diminuto_ipv4_t datum4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t datum6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_ipv4_t * addresses4 = (diminuto_ipv4_t *)0;
    diminuto_ipv6_t * addresses6 = (diminuto_ipv6_t *)0;
    diminuto_port_t port46 = 0;
    diminuto_port_t rendezvous46 = 0;
    diminuto_port_t datum46 = 0;
    char * interface = (char *)0;
    size_t blocksize = 4096;
    char string[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")] = { '\0' };
    char * buffer = (char *)0;
    ssize_t input = 0;
    ssize_t output = 0;
    ssize_t total = 0;
    int Protocol = 0;
    int sock = -2;
    int fd = -3;
    int rc = 0;
    int fds = 0;
    int eof = 0;
    diminuto_unsigned_t value;
    char ** interfaci;
    char ** interfaces;
    diminuto_mux_t mux;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

/*******************************************************************************
 * INITIALIZATION
 ******************************************************************************/

    diminuto_log_setmask();

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

/*******************************************************************************
 * PARSE
 ******************************************************************************/

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

/*******************************************************************************
 * PARAMETERS
 ******************************************************************************/

    if (Protocol == 4) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Protocol=%d\n", Protocol);
    } else if (Protocol == 6) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Protocol=%d\n", Protocol);
    } else {
        assert(!0);
    }

    if (Address == (const char *)0) {
        /* Do nothing. */
    } else if (Protocol == 4) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Address4=\"%s\"\n", Address);
        address4 = diminuto_ipc4_address(Address);
    } else if (Protocol == 6) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Address6=\"%s\"\n", Address);
        address6 = diminuto_ipc6_address(Address);
    } else {
        /* Do nothing. */
    }

    if (Protocol == 4) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address4=%s\n",  diminuto_ipc4_address2string(address4, string, sizeof(string)));
    } else if (Protocol == 6) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address6=%s\n",  diminuto_ipc6_address2string(address6, string, sizeof(string)));
    } else {
        /* Do nothing. */
    }

    if (Port == (const char *)0) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Port46=\"%s\"\n", Port);
        port46 = diminuto_ipc_port(Port, "tcp");
    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "port46==%d\n", port46);

    if (Server == (const char *)0) {
        /* Do nothing. */
    } else if (Protocol == 4) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Server4=\"%s\"\n", Server);
        server4 = diminuto_ipc4_address(Server);
    } else if (Protocol == 6) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Server6=\"%s\"\n", Server);
        server6 = diminuto_ipc6_address(Server);
    } else {
        /* Do nothing. */
    }

    if (Protocol == 4) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "server4=%s\n", diminuto_ipc4_address2string(server4, string, sizeof(string)));
    } else if (Protocol == 6) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "server6=%s\n", diminuto_ipc6_address2string(server6, string, sizeof(string)));
    } else {
        /* Do nothing. */
    }

    if (Rendezvous == (const char *)0) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Rendezvous46=\"%s\"\n", Rendezvous);
        rendezvous46 = diminuto_ipc_port(Rendezvous, "tcp");
    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "rendezvous46=%d\n", rendezvous46);

    if (Interface == (const char *)0) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Interface=\"%s\"\n", Interface);
        interfaci = interfaces = diminuto_ipc_interfaces();
        assert(interfaces != (char **)0);
        for (; *interfaces != (char *)0; ++interfaces) {
            if (strcmp(Interface, *interfaces) == 0) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface=\"%s\"\n", *interfaces);
                interface = *interfaces;
            }
        }
        assert(interface != (char *)0);
        if (Protocol == 4) {
            addresses4 = diminuto_ipc4_interface(interface);
            assert(addresses4 != (diminuto_ipv4_t *)0);
            for (; *addresses4 != DIMINUTO_IPC4_UNSPECIFIED; ++addresses4) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface4=%s\n", diminuto_ipc4_address2string(*addresses4, string, sizeof(string)));
            }
        } else if (Protocol == 6) {
            addresses6 = diminuto_ipc6_interface(interface);
            assert(addresses6 != (diminuto_ipv6_t *)0);
            for (; memcmp(addresses6, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(*addresses6)) != 0; ++addresses6) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface6=%s\n", diminuto_ipc6_address2string(*addresses6, string, sizeof(string)));
            }
        } else {
            /* Do nothing. */
        }
        free(interfaci);
    }

    if (Blocksize == (const char *)0) {
        /* Do nothing. */
    } else if (*diminuto_number_unsigned(Blocksize, &value) != '\0') {
        assert(!0);
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Blocksize=\"%s\"\n", Blocksize);
        blocksize = value;
    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "blocksize=%zu\n", blocksize);

/*******************************************************************************
 * ENDPOINTS
 ******************************************************************************/

    if (Protocol == 4) {
        if (Rendezvous == (const char *)0) {
            sock = diminuto_ipc4_stream_provider_specific(address4, port46, Interface, -1);
            assert(sock >= 0);
            rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
            assert(rc >= 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
        } else {
            sock = diminuto_ipc4_stream_consumer_specific(server4, rendezvous46, address4, port46, Interface);
            assert(sock >= 0);
            rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
            assert(rc >= 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
            rc = diminuto_ipc4_farend(sock, &datum4, &datum46);
            assert(rc >= 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=far sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
        }
    } else if (Protocol == 6) {
        if (Rendezvous == (const char *)0) {
            sock = diminuto_ipc6_stream_provider_specific(address6, port46, Interface, -1);
            assert(sock >= 0);
            rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
            assert(rc >= 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
        } else {
            sock = diminuto_ipc6_stream_consumer_specific(server6, rendezvous46, address6, port46, Interface);
            assert(sock >= 0);
            rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
            assert(rc >= 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
            rc = diminuto_ipc6_farend(sock, &datum6, &datum46);
            assert(rc >= 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=far sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
        }
    } else {
        /* Do nothing. */
    }

    buffer = (char *)malloc(blocksize);
    assert(buffer != (char *)0);

/*******************************************************************************
 * SETUP
 ******************************************************************************/

    diminuto_mux_init(&mux);

/*******************************************************************************
 * PROVIDER
 ******************************************************************************/

    if (Rendezvous == (const char *)0) {

        rc = diminuto_mux_register_accept(&mux, sock);
        assert(rc >= 0);
        while (!0) {
            fds = diminuto_mux_wait(&mux, -1);
            assert(fds > 0);
            while (!0) {
                fd = diminuto_mux_ready_accept(&mux);
                if (fd < 0) {
                    break;
                }
                if (Protocol == 4) {
                    fd = diminuto_ipc4_stream_accept_generic(fd, &datum4, &datum46);
                    assert(fd >= 0);
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "role=provider end=far fd=%d datum4=%s datum46=%d\n", fd, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
                } else if (Protocol == 6) {
                    fd = diminuto_ipc6_stream_accept_generic(fd, &datum6, &datum46);
                    assert(fd >= 0);
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "role=provider end=far fd=%d datum6=%s datum46=%d\n", fd, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
                } else {
                    /* Do nothing. */
                }
                rc = diminuto_mux_register_read(&mux, fd);
                assert(rc >= 0);
            }
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                input = diminuto_fd_read(fd, buffer, 1, blocksize);
                assert(input >= 0);
                if (input == 0) {
                    rc = diminuto_ipc_close(fd);
                    assert(rc >= 0);
                    rc = diminuto_mux_unregister_read(&mux, fd);
                    assert(rc >= 0);
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "role=provider end=far fd=%d input=%zd\n", fd, input);
                } else {
                    output = diminuto_fd_write(fd, buffer, input, input);
                    assert(output == input);
                }
            }
        }
        /* Can never reach here. */
        assert(fds == 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);

/*******************************************************************************
 * CONSUMER
 ******************************************************************************/

    } else {

        rc = diminuto_mux_register_read(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, STDIN_FILENO);
        assert(rc >= 0);
        while ((!eof) || (total > 0)) {
            fds = diminuto_mux_wait(&mux, -1);
            assert(fds > 0);
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                if (fd == sock) {
                    input = diminuto_fd_read(sock, buffer, 1, blocksize);
                    assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "role=consumer fd=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, sock);
                        assert(rc >= 0);
                    } else {
                        output = diminuto_fd_write(STDOUT_FILENO, buffer, input, input);
                        assert(output == input);
                        total -= output;
                    }
                } else if (fd == STDIN_FILENO) {
                    input = diminuto_fd_read(STDIN_FILENO, buffer, 1, blocksize);
                    assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "role=consumer fd=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, STDIN_FILENO);
                        assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_fd_write(sock, buffer, input, input);
                        assert(output == input);
                        total += output;
                    }
                } else {
                    assert(!0);
                }
            }
        }
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);
    }

/*******************************************************************************
 * EXIT
 ******************************************************************************/

    diminuto_mux_fini(&mux);

    exit(0);
}
