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
#include "com/diag/diminuto/diminuto_mux.h"

int main(int argc, char * argv[])
{
    const char * Address = (const char *)0;
    const char * Interface = (const char *)0;
    const char * Port = (const char *)0;
    const char * Server =  (const char *)0;
    const char * Rendezvous = (const char *)0;
    diminuto_ipv4_t address4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t address6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_ipv4_t server4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t server6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_port_t port46 = 0;
    diminuto_port_t rendezvous46 = 0;
    diminuto_mux_t mux;
    int Protocol = 4;
    int sock = -2;
    int fd = -3;
    int rc = 0;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

    SETLOGMASK();

    while ((opt = getopt(argc, argv, "46A:P:a:i:p:")) >= 0) {
        switch (opt) {
        case 4:
            Protocol = 4; /* IPv4 */
            break;
        case 6:
            Protocol = 6; /* IPv6 */
            break;
        case 'a':
            Address = optarg; /* near end bind address */
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
        default:
            break;
        }
    }

    TEST();

    if (Address == (const char *)0) {
        /* Do nothing. */
    } else if (Protocol == 4) {
            address4 = diminuto_ipc4_address(Address);
    } else if (Protocol == 6) {
            address6 = diminuto_ipc6_address(Address);
    } else {
        /* Do nothing. */
    }

    if (Port == (const char *)0) {
        /* Do nothing. */
    } else {
        port46 = diminuto_ipc_port(Port, "tcp");
    }

    if (Server == (const char *)0) {
        /* Do nothing. */
    } else if (Protocol == 4) {
        server4 = diminuto_ipc4_address(Server);
    } else if (Protocol == 6) {
        server6 = diminuto_ipc6_address(Server);
    } else {
        /* Do nothing. */
    }

    if (Rendezvous == (const char *)0) {
        /* Do nothing. */
    } else {
        rendezvous46 = diminuto_ipc_port(Rendezvous, "tcp");
    }

    TEST();

    if (Protocol == 4) {
        if (Rendezvous == (const char *)0) {
            sock = diminuto_ipc4_stream_provider_specific(address4, port46, Interface, -1);
            ASSERT(sock >= 0);
        } else {
            fd = diminuto_ipc4_stream_consumer_specific(server4, rendezvous46, address4, port46, Interface);
            ASSERT(fd >= 0);
        }
    } else if (Protocol == 6) {
        if (Rendezvous == (const char *)0) {
            sock = diminuto_ipc6_stream_provider_specific(address6, port46, Interface, -1);
            ASSERT(sock >= 0);
        } else {
            fd = diminuto_ipc6_stream_consumer_specific(server6, rendezvous46, address6, port46, Interface);
            ASSERT(fd >= 0);
        }
    } else {
        /* Do nothing. */
    }

    TEST();

    diminuto_mux_init(&mux);

    if (Rendezvous == (const char *)0) {
        rc = diminuto_mux_register_accept(&mux, sock);
        ASSERT(rc >= 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        ASSERT(rc >= 0);
        rc = diminuto_ipc_close(sock);
        ASSERT(rc >= 0);
    } else {
        rc = diminuto_mux_register_read(&mux, fd);
        ASSERT(rc >= 0);
        rc = diminuto_mux_register_write(&mux, fd);
        ASSERT(rc >= 0);
        rc = diminuto_mux_unregister_read(&mux, fd);
        ASSERT(rc >= 0);
        rc = diminuto_mux_unregister_write(&mux, fd);
        ASSERT(rc >= 0);
    }

    diminuto_mux_fini(&mux);

    EXIT();
}
