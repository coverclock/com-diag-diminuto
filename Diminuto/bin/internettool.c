/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Tests internet connectivity.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * internettool is used to test internet connectivity using application
 * protocols like Transmission Control Protocol or TCP (-t), or User Datagram
 * Protocol or UDP (-u), although it can also use Internet Control Message
 * Protocol or ICMP (-g) like ping(1). internettool can use IPv4 (-4) or IPv6
 * (-6). It can be used in server mode (-p or -e) or client mode (-A, -P, or
 * -E). In server mode, it receives packets from one or more clients, and
 * sends them back to the sender unaltered. In client mode, it reads standard
 * input and sends the data to the server, and receives from the server and
 * writes the data to standard output. It can bind its socket to a specific
 * interface (-i) or address (-a). internettool is not intended to replace
 * socat(1), but provides a way to functionally test the Diminuto IPC feature
 * in a variety of contexts.
 *
 * EXAMPLES
 *
 * internettool -6 -u -p 5555
 * internettool -6 -u -p tumbleweed
 * internettool -6 -u -E :5555
 * internettool -6 -E :tumbleweed
 * internettool -6 -A 2001:470:4b:4e2:8eae:4cff:fef4:40c -u -P 5555
 * internettool -4 -A 192.168.1.237 -u -P 5555
 * internettool -u -E 192.168.1.237:5555
 * internettool -6 -A ::ffff:192.168.1.237 -u -P 5555
 * internettool -u -E [::ffff:192.168.1.237]:5555
 * internettool -A tumbleweed -u -P tumbleweed
 * internettool -E tumbleweed:tumbleweed
 *
 * IPv4, TCP:
 *
 * internettool -4 -t -e :5555 & # Loopback Server
 * timesource | internettool -t -E localhost:5555 | timesink # Client
 *
 * IPv4, UDP:
 *
 * internettool -4 -u -e :5555 & # Loopback Server
 * timesource | internettool -u -E localhost:5555 | timesink # Client
 *
 * IPv6, TCP:
 *
 * internettool -6 -t -e :5555 & # Loopback Server
 * timesource | internettool -t -E ip6-localhost:5555 | timesink # Client
 *
 * IPv6, UDP:
 *
 * internettool -6 -u -e :5555 & # Loopback Server
 * timesource | internettool -u -E ip6-localhost:5555 | timesink # Client
 *
 * IPv4, ICMP:
 *
 * internettool -I # Select Interface
 * sudo internettool -g -e ip6-localhost -i lo # Ping
 *
 * IPv6, ICMP:
 *
 * internettool -I # Select Interface
 * sudo internettool -g -e localhost -i lo # Ping
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_phex.h"
#include "com/diag/diminuto/diminuto_ping4.h"
#include "com/diag/diminuto/diminuto_ping6.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void stamp(FILE *fp)
{
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t now = 0;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    diminuto_ticks_t ticks = 0;
    int rc = -1;
    diminuto_ticks_t microseconds = 0;

    now = diminuto_time_clock();
    diminuto_assert(now != -1);
    rc = diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &ticks);
    diminuto_assert(rc >= 0);
    microseconds = ticks;
    microseconds *= 1000000;
    microseconds /= diminuto_time_frequency();
    ticks = microseconds;
    fprintf(fp, "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6lldZ ", year, month, day, hour, minute, second, (diminuto_lld_t)ticks);
}

static void emit(FILE * fp, const void * buffer, size_t length)
{
    const unsigned char * bb;
    size_t current = 0;
    int end = 0;

    for (bb = (const unsigned char *)buffer; length > 0; --length) {
        diminuto_phex_emit(fp, *(bb++), ~0, 0, !0, 0, &current, &end, 0);
    }
}

int main(int argc, char * argv[])
{
    const char * Program = (const char *)0;
    const char * Interface = (const char *)0;
    const char * Nearendpoint = (const char *)0;
    const char * Address = (const char *)0;
    const char * Port = (const char *)0;
    const char * Farendpoint = (const char *)0;
    const char * Server =  (const char *)0;
    const char * Rendezvous = (const char *)0;
    const char * Blocksize = (const char *)0;
    char Layer2 = '4';
    char Layer3 = 't';
    char Debug = '\0';
    char Verbose = '\0';
    char Interfaces = '\0';
    bool Exit = false;
    diminuto_ipv4_t address4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t address6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_ipv4_t server4 = DIMINUTO_IPC4_LOOPBACK;
    diminuto_ipv6_t server6 = DIMINUTO_IPC6_LOOPBACK;
    diminuto_ipv4_t datum4 = DIMINUTO_IPC4_UNSPECIFIED;
    diminuto_ipv6_t datum6 = DIMINUTO_IPC6_UNSPECIFIED;
    diminuto_ipv4_t * addresses4 = (diminuto_ipv4_t *)0;
    diminuto_ipv6_t * addresses6 = (diminuto_ipv6_t *)0;
    diminuto_port_t port46 = DIMINUTO_IPC_EPHEMERAL;
    diminuto_port_t rendezvous46 = DIMINUTO_IPC_EPHEMERAL;
    diminuto_port_t datum46 = DIMINUTO_IPC_EPHEMERAL;
    diminuto_ipc_endpoint_t farendpoint = { 0, };
    diminuto_ipc_endpoint_t nearendpoint = { 0, };
    diminuto_ipv4_buffer_t address4buffer = { '\0', };
    diminuto_ipv6_buffer_t address6buffer = { '\0', };
    diminuto_ipv4_buffer_t server4buffer = { '\0', };
    diminuto_ipv6_buffer_t server6buffer = { '\0', };
    diminuto_ipv4_buffer_t datum4buffer = { '\0', };
    diminuto_ipv6_buffer_t datum6buffer = { '\0', };
    diminuto_ipv4_buffer_t addresses4buffer = { '\0', };
    diminuto_ipv6_buffer_t addresses6buffer = { '\0', };
    diminuto_port_buffer_t port46buffer = { '\0', };
    diminuto_port_buffer_t rendezvous46buffer = { '\0', };
    diminuto_port_buffer_t datum46buffer = { '\0', };
    char * interface = (char *)0;
    size_t blocksize = 512;
    int source = STDIN_FILENO;
    int sink = STDOUT_FILENO;
    FILE * tee = stderr;
    char * buffer = (char *)0;
    ssize_t input = 0;
    ssize_t output = 0;
    ssize_t total = 0;
    int sock = -2;
    int fd = -3;
    int ready = -3;
    int rc = 0;
    int fds = 0;
    int eof = 0;
    uint8_t type = 0;
    uint8_t code = 0;
    uint16_t id = 0;
    uint16_t sn = 0;
    uint8_t ttl = 0;
    uint16_t ii = 0;
    uint16_t ss = 0;
    diminuto_unsigned_t value = 0;
    char ** interfaci = (char **)0;
    char ** interfaces = (char **)0;
    diminuto_mux_t mux;
    diminuto_ticks_t delay = 0;
    diminuto_ticks_t elapsed = 0;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt = '\0';

/*******************************************************************************
 * PARSE
 ******************************************************************************/

    diminuto_log_setmask();

    Program = ((Program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : Program + 1;

    while ((opt = getopt(argc, argv, "46?A:E:IP:a:b:de:gi:p:tuvx")) >= 0) {
        switch (opt) {
        case '4':
        case '6':
            Layer2 = opt; /* IPv4 or IPv6 */
            break;
        case 'A':
            Server = optarg; /* far end server address */
            break;
        case 'E':
            Farendpoint = optarg; /* far end endpoint string */
            break;
        case 'I':
            Interfaces = opt; /* log interfaces. */
            break;
        case 'P':
            Rendezvous = optarg; /* far end port number */
            break;
        case 'a':
            Address = optarg; /* near end bind address */
            break;
        case 'b':
            Blocksize = optarg; /* input/output blocksize */
            break;
        case 'd':
            Debug = opt; /* dump to stderr */
            break;
        case 'e':
            Nearendpoint = optarg; /* near end endpoint string */
            break;
        case 'i':
            Interface = optarg; /* near end interface */
            break;
        case 'p':
            Port = optarg; /* near end bind port number */
            break;
        case 'g':
        case 't':
        case 'u':
            Layer3 = opt; /* tcp or udp or ping */
            break;
        case 'v':
            Verbose = opt; /* print to stderr */
            break;
        case 'x':
            Exit = true; /* exit after parsing */
            break;
        case '?': 
            fprintf(stderr, "usage: %s [ -? ] [ -I ] [ -4 | -6 ] [ -t | -u | -g ] [ -I INFD ] [ -O OUTFD ] [ -a NEADDR ] [ -p NEPORT ] [ -e NEPOINT ] [ -i NEINTF ] [ -A FEADDR ] [ -P FEPORT ] [ -E FEPOINT ] [ -b BYTES ] [ -x ]\n", Program);
            fprintf(stderr, "       -?          Display this menu.\n");
            fprintf(stderr, "       -4          Use IPv4.\n");
            fprintf(stderr, "       -6          Use IPv6.\n");
            fprintf(stderr, "       -A FEADDR   Connect far end socket to host or address FEADDR.\n");
            fprintf(stderr, "       -E FEPOINT  Connect far end socket to host or endpoint FEPOINT.\n");
            fprintf(stderr, "       -I          Display interfaces and exit.\n");
            fprintf(stderr, "       -P FEPORT   Connect far end socket to service or port FEPORT.\n");
            fprintf(stderr, "       -a NEADDR   Bind near end socket to host or address NEADDR.\n");
            fprintf(stderr, "       -b BYTES    Size input/output buffer to BYTES bytes.\n");
            fprintf(stderr, "       -d          Dump received data to standard error on provider.\n");
            fprintf(stderr, "       -e NEPOINT  Connect near end socket to host or endpoint NEPOINT.\n");
            fprintf(stderr, "       -g          Use ICMP echo request (ping).\n");
            fprintf(stderr, "       -i NEINTF   Bind near end socket to interface NEINTF.\n");
            fprintf(stderr, "       -p NEPORT   Bind near end socket to service or port NEPORT.\n");
            fprintf(stderr, "       -t          Use TCP.\n");
            fprintf(stderr, "       -u          Use UDP.\n");
            fprintf(stderr, "       -v          Print received data to standard error on provider.\n");
            fprintf(stderr, "       -x          Exit after parsing parameters.\n");
            fprintf(stderr, "       -?          Display this menu and exit.\n");
            return 1;
            break;
        default:
            break;
        }
    }

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

    if (Interfaces) {
        char ** ifvp = (char **)0;
        char ** ifp = (char **)0;
        diminuto_ipv4_t * v4vp = (diminuto_ipv4_t *)0;
        diminuto_ipv4_t * v4p = (diminuto_ipv4_t *)0;
        diminuto_ipv6_t * v6vp = (diminuto_ipv6_t *)0;
        diminuto_ipv6_t * v6p = (diminuto_ipv6_t *)0;
        diminuto_ipv4_buffer_t v4pbuffer = { '\0', };
        diminuto_ipv6_buffer_t v6pbuffer = { '\0', };
        const char * type = (const char *)0;

        ifvp = diminuto_ipc_interfaces();
        diminuto_assert(ifvp != (char **)0);

        /*
         * We have a array of zero or more network interfaces, each
         * of which is bound to a array of zero or more IPv4 address
         * and zero or more IPv6 addresses.
         */

        for (ifp = ifvp; *ifp != (char *)0; ++ifp) {

            type = (const char *)0;

            v4vp = diminuto_ipc4_interface(*ifp);
            diminuto_assert(v4vp != (diminuto_ipv4_t *)0);
            for (v4p = v4vp; !diminuto_ipc4_is_unspecified(v4p); ++v4p) {
                type = diminuto_ipc4_address2type(*v4p);
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "interface=%s binding4=%s type4=%s\n", *ifp, diminuto_ipc4_address2string(*v4p, v4pbuffer, sizeof(v4pbuffer)), type);
            }
            free(v4vp);

            v6vp = diminuto_ipc6_interface(*ifp);
            diminuto_assert(v6vp != (diminuto_ipv6_t *)0);
            for (v6p = v6vp; !diminuto_ipc6_is_unspecified(v6p); ++v6p) {
                type = diminuto_ipc6_address2type(*v6p);
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "interface=%s binding6=%s type6=%s\n", *ifp, diminuto_ipc6_address2string(*v6p, v6pbuffer, sizeof(v6pbuffer)), type);
            }
            free(v6vp);

            if (type == (const char *)0) {
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "interface=%s\n", *ifp);
            }
        }

        free(ifvp);

        return 0;
    }

/*******************************************************************************
 * PARAMETERS
 ******************************************************************************/

    if (Nearendpoint == (const char *)0) {
        /* Do nothing. */
    } else if (diminuto_ipc_endpoint(Nearendpoint, &nearendpoint) < 0) {
        diminuto_assert(0);
    } else if (diminuto_ipc4_is_unspecified(&nearendpoint.ipv4) && diminuto_ipc6_is_unspecified(&nearendpoint.ipv6)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "NearEndpoint46=\"%s\"\n", Nearendpoint);
        if ((!diminuto_ipc_is_ephemeral(nearendpoint.tcp)) && (!diminuto_ipc_is_ephemeral(nearendpoint.udp))) {
            if (Layer3 == 't') {
                port46 = nearendpoint.tcp;
            } else if (Layer3 == 'u') {
                port46 = nearendpoint.udp;
            } else {
                /* Do nothing. */
            }
        } else if (!diminuto_ipc_is_ephemeral(nearendpoint.tcp)) {
            Layer3 = 't';
            port46 = nearendpoint.tcp;
        } else if (!diminuto_ipc_is_ephemeral(nearendpoint.udp)) {
            Layer3 = 'u';
            port46 = nearendpoint.udp;
        } else {
            /* Do nothing. */
        }
    } else {
        if ((!diminuto_ipc4_is_unspecified(&nearendpoint.ipv4)) && (!diminuto_ipc6_is_unspecified(&nearendpoint.ipv6))) {
            address4 = nearendpoint.ipv4;
            address6 = nearendpoint.ipv6;
            if (Layer2 == '4') {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "NearEndpoint4=\"%s\"\n", Nearendpoint);
            } else if (Layer2 == '6') {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "NearEndpoint6=\"%s\"\n", Nearendpoint);
            } else {
                /* Do nothing. */
            }
        } else if (!diminuto_ipc4_is_unspecified(&nearendpoint.ipv4)) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "NearEndpoint4=\"%s\"\n", Nearendpoint);
            Layer2 = '4';
            address4 = nearendpoint.ipv4;
            address6 = DIMINUTO_IPC6_UNSPECIFIED;
        } else if (!diminuto_ipc6_is_unspecified(&nearendpoint.ipv6)) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "NearEndpoint6=\"%s\"\n", Nearendpoint);
            Layer2 = '6';
            address4 = DIMINUTO_IPC4_UNSPECIFIED;
            address6 = nearendpoint.ipv6;
        } else {
            /* Do nothing. */
        }
        if ((!diminuto_ipc_is_ephemeral(nearendpoint.tcp)) && (!diminuto_ipc_is_ephemeral(nearendpoint.udp))) {
            if (Layer3 == 't') {
                port46 = nearendpoint.tcp;
            } else if (Layer3 == 'u') {
                port46 = nearendpoint.udp;
            } else {
                /* Do nothing. */
            }
        } else if (!diminuto_ipc_is_ephemeral(nearendpoint.tcp)) {
            Layer3 = 't';
            port46 = nearendpoint.tcp;
        } else if (!diminuto_ipc_is_ephemeral(nearendpoint.udp)) {
            Layer3 = 'u';
            port46 = nearendpoint.udp;
        } else {
            /* Do nothing. */
        }
    }

    if (Farendpoint == (const char *)0) {
        /* Do nothing. */
    } else if (diminuto_ipc_endpoint(Farendpoint, &farendpoint) < 0) {
        diminuto_assert(0);
    } else if (diminuto_ipc4_is_unspecified(&farendpoint.ipv4) && diminuto_ipc6_is_unspecified(&farendpoint.ipv6)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "FarEndpoint46=\"%s\"\n", Farendpoint);
        if ((!diminuto_ipc_is_ephemeral(farendpoint.tcp)) && (!diminuto_ipc_is_ephemeral(farendpoint.udp))) {
            if (Layer3 == 't') {
                rendezvous46 = farendpoint.tcp;
            } else if (Layer3 == 'u') {
                rendezvous46 = farendpoint.udp;
            } else {
                /* Do nothing. */
            }
        } else if (!diminuto_ipc_is_ephemeral(farendpoint.tcp)) {
            Layer3 = 't';
            rendezvous46 = farendpoint.tcp;
        } else if (!diminuto_ipc_is_ephemeral(farendpoint.udp)) {
            Layer3 = 'u';
            rendezvous46 = farendpoint.udp;
        } else {
            /* Do nothing. */
        }
    } else {
        if ((!diminuto_ipc4_is_unspecified(&farendpoint.ipv4)) && (!diminuto_ipc6_is_unspecified(&farendpoint.ipv6))) {
            server4 = farendpoint.ipv4;
            server6 = farendpoint.ipv6;
            if (Layer2 == '4') {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "FarEndpoint4=\"%s\"\n", Farendpoint);
            } else if (Layer2 == '6') {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "FarEndpoint6=\"%s\"\n", Farendpoint);
            } else {
                /* Do nothing. */
            }
        } else if (!diminuto_ipc4_is_unspecified(&farendpoint.ipv4)) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "FarEndpoint4=\"%s\"\n", Farendpoint);
            Layer2 = '4';
            server4 = farendpoint.ipv4;
            server6 = DIMINUTO_IPC6_UNSPECIFIED;
        } else if (!diminuto_ipc6_is_unspecified(&farendpoint.ipv6)) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "FarEndpoint6=\"%s\"\n", Farendpoint);
            Layer2 = '6';
            server4 = DIMINUTO_IPC4_UNSPECIFIED;
            server6 = farendpoint.ipv6;
        } else {
            /* Do nothing. */
        }
        if ((!diminuto_ipc_is_ephemeral(farendpoint.tcp)) && (!diminuto_ipc_is_ephemeral(farendpoint.udp))) {
            if (Layer3 == 't') {
                rendezvous46 = farendpoint.tcp;
            } else if (Layer3 == 'u') {
                rendezvous46 = farendpoint.udp;
            } else {
                /* Do nothing. */
            }
        } else if (!diminuto_ipc_is_ephemeral(farendpoint.tcp)) {
            Layer3 = 't';
            rendezvous46 = farendpoint.tcp;
        } else if (!diminuto_ipc_is_ephemeral(farendpoint.udp)) {
            Layer3 = 'u';
            rendezvous46 = farendpoint.udp;
        } else {
            /* Do nothing. */
        }
    }

    if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer2=IPv4\n");
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer2=IPv6\n");
    } else {
        diminuto_assert(0);
    }

    if (Layer3 == 't') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer3=TCP\n");
    } else if (Layer3 == 'u') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer3=UDP\n");
    } else if (Layer3 == 'g') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer3=ICMP Echo Request\n");
    } else {
        diminuto_assert(0);
    }

    if (Address == (const char *)0) {
        /* Do nothing. */
    } else if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Address4=\"%s\"\n", Address);
        address4 = diminuto_ipc4_address(Address);
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Address6=\"%s\"\n", Address);
        address6 = diminuto_ipc6_address(Address);
    } else {
        diminuto_assert(0);
    }

    if (!diminuto_ipc4_is_unspecified(&address4)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address4=%s\n",  diminuto_ipc4_address2string(address4, address4buffer, sizeof(address4buffer)));
    }

    if (!diminuto_ipc6_is_unspecified(&address6)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address6=%s\n",  diminuto_ipc6_address2string(address6, address6buffer, sizeof(address6buffer)));
    }

    if (Port == (const char *)0) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Port46=\"%s\"\n", Port);
        if (Layer3 == 't') {
            port46 = diminuto_ipc_port(Port, "tcp");
        } else if (Layer3 == 'u') {
            port46 = diminuto_ipc_port(Port, "udp");
        } else {
            /* Do nothing. */
        }
    }

    if (!diminuto_ipc_is_ephemeral(port46)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "port46=%s\n", diminuto_ipc_port2string(port46, port46buffer, sizeof(port46buffer)));
    }

    if (Server == (const char *)0) {
        /* Do nothing. */
    } else if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Server4=\"%s\"\n", Server);
        server4 = diminuto_ipc4_address(Server);
        server6 = DIMINUTO_IPC6_UNSPECIFIED;
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Server6=\"%s\"\n", Server);
        server4 = DIMINUTO_IPC4_UNSPECIFIED;
        server6 = diminuto_ipc6_address(Server);
    } else {
        diminuto_assert(0);
    }

    if (!diminuto_ipc4_is_unspecified(&server4)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "server4=%s\n", diminuto_ipc4_address2string(server4, server4buffer, sizeof(server4buffer)));
    }

    if (!diminuto_ipc6_is_unspecified(&server6)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "server6=%s\n", diminuto_ipc6_address2string(server6, server6buffer, sizeof(server6buffer)));
    }

    if (Rendezvous == (const char *)0) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Rendezvous46=\"%s\"\n", Rendezvous);
        rendezvous46 = diminuto_ipc_port(Rendezvous, "tcp");
    }

    if (!diminuto_ipc_is_ephemeral(rendezvous46)) {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "rendezvous46=%s\n", diminuto_ipc_port2string(rendezvous46, rendezvous46buffer, sizeof(rendezvous46buffer)));
    }

    if (Interface == (const char *)0) {
        /* Do nothing. */
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Interface=\"%s\"\n", Interface);
        interfaci = diminuto_ipc_interfaces();
        diminuto_assert(interfaci != (char **)0);
        for (interfaces = interfaci; *interfaces != (char *)0; ++interfaces) {
            if (strcmp(Interface, *interfaces) == 0) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface=\"%s\"\n", *interfaces);
                interface = *interfaces;
            }
        }
        diminuto_assert(interface != (char *)0);
        if (Layer2 == '4') {
            addresses4 = diminuto_ipc4_interface(interface);
            diminuto_assert(addresses4 != (diminuto_ipv4_t *)0);
            for (; !diminuto_ipc4_is_unspecified(addresses4); ++addresses4) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "binding4=%s\n", diminuto_ipc4_address2string(*addresses4, addresses4buffer, sizeof(addresses4buffer)));
            }
        } else if (Layer2 == '6') {
            addresses6 = diminuto_ipc6_interface(interface);
            diminuto_assert(addresses6 != (diminuto_ipv6_t *)0);
            for (; !diminuto_ipc6_is_unspecified(addresses6); ++addresses6) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "binding6=%s\n", diminuto_ipc6_address2string(*addresses6, addresses6buffer, sizeof(addresses6buffer)));
            }
        } else {
            diminuto_assert(0);
        }
        free(interfaci);
    }

    if (Blocksize == (const char *)0) {
        /* Do nothing. */
    } else if (*diminuto_number_unsigned(Blocksize, &value) != '\0') {
        diminuto_assert(0);
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Blocksize=\"%s\"\n", Blocksize);
        blocksize = value;
    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "blocksize=%zu\n", blocksize);

    if (Exit) {
        fflush(stderr);
        printf("%s L2 %c L3 %c FE %s %s %s NE %s %s %s\n",
            Program,
            Layer2,
            Layer3,
            diminuto_ipc4_address2string(server4, server4buffer, sizeof(server4buffer)),
            diminuto_ipc6_address2string(server6, server6buffer, sizeof(server6buffer)),
            diminuto_ipc_port2string(rendezvous46, rendezvous46buffer, sizeof(rendezvous46buffer)),
            diminuto_ipc4_address2string(address4, address4buffer, sizeof(address4buffer)),
            diminuto_ipc6_address2string(address6, address6buffer, sizeof(address6buffer)),
            diminuto_ipc_port2string(port46, port46buffer, sizeof(port46buffer)));
        fflush(stdout);
        exit(0);
    }

    buffer = (char *)malloc(blocksize);
    diminuto_assert(buffer != (char *)0);

/*******************************************************************************
 * SERVICE PROVIDER - IPv4 - TCP
 ******************************************************************************/

    if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '4') && (Layer3 == 't')) {

         sock = diminuto_ipc4_stream_provider_generic(address4, port46, Interface, -1);
         diminuto_assert(sock >= 0);
         rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
         diminuto_assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near type=stream sock=%d datum4=%s datum46=%s\n", sock, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE PROVIDER - IPv4 - UDP
 ******************************************************************************/

    else if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '4') && (Layer3 == 'u')) {

         sock = diminuto_ipc4_datagram_peer_generic(address4, port46, Interface);
         diminuto_assert(sock >= 0);
         rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
         diminuto_assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near type=datagram sock=%d datum4=%s datum46=%s\n", sock, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE PROVIDER - IPv6 - TCP
 ******************************************************************************/

    else if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '6') && (Layer3 == 't')) {

         sock = diminuto_ipc6_stream_provider_generic(address6, port46, Interface, -1);
         diminuto_assert(sock >= 0);
         rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
         diminuto_assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near sock=%d datum6=%s datum46=%s\n", sock, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE PROVIDER - IPv6 - UDP
 ******************************************************************************/

    else if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '6') && (Layer3 == 'u')) {

         sock = diminuto_ipc6_datagram_peer_generic(address6, port46, Interface);
         diminuto_assert(sock >= 0);
         rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
         diminuto_assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near type=datagram sock=%d datum6=%s datum46=%s\n", sock, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv4 - TCP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && (Layer2 == '4') && (Layer3 == 't')) {

        sock = diminuto_ipc4_stream_consumer_generic(server4, rendezvous46, address4, port46, Interface);
        diminuto_assert(sock >= 0);
        rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=stream sock=%d datum4=%s datum46=%s\n", sock, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));
        rc = diminuto_ipc4_farend(sock, &datum4, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=far type=stream sock=%d datum4=%s datum46=%s\n", sock, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv4 - UDP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && (Layer2 == '4') && (Layer3 == 'u')) {

        sock = diminuto_ipc4_datagram_peer_generic(address4, port46, Interface);
        diminuto_assert(sock >= 0);
        rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=datagram sock=%d datum4=%s datum46=%s\n", sock, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv6 - TCP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && (Layer2 == '6') && (Layer3 == 't')) {

        sock = diminuto_ipc6_stream_consumer_generic(server6, rendezvous46, address6, port46, Interface);
        diminuto_assert(sock >= 0);
        rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=stream sock=%d datum6=%s datum46=%s\n", sock, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));
        rc = diminuto_ipc6_farend(sock, &datum6, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=far type=stream sock=%d datum6=%s datum46=%s\n", sock, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv6 - UDP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && (Layer2 == '6') && (Layer3 == 'u')) {

        sock = diminuto_ipc6_datagram_peer_generic(address6, port46, Interface);
        diminuto_assert(sock >= 0);
        rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=datagram sock=%d datum6=%s datum46=%s\n", sock, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * PING - IPv4 - ICMP Echo Request
 ******************************************************************************/

    else if ((Layer2 == '4') && (Layer3 == 'g')) {

        sock = diminuto_ping4_datagram_peer();
        diminuto_assert(sock >= 0);
        rc = diminuto_ipc4_source(sock, address4, port46);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc_set_interface(sock, interface);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=ping end=near type=datagram sock=%d interface=%s datum4=%s datum46=%s\n", sock, interface, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * PING - IPv6 - ICMP Echo Request
 ******************************************************************************/

    else if ((Layer2 == '6') && (Layer3 == 'g')) {

        sock = diminuto_ping6_datagram_peer();
        diminuto_assert(sock >= 0);
        rc = diminuto_ipc6_source(sock, address6, port46);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc_set_interface(sock, interface);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
        diminuto_assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=ping end=near type=datagram sock=%d interface=%s datum6=%s datum46=%s\n", sock, interface, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));

    }

/*******************************************************************************
 * DONE
 ******************************************************************************/

    else {

        diminuto_assert(0);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv4 - TCP
 ******************************************************************************/

    if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '4') && (Layer3 == 't')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_accept(&mux, sock);
        diminuto_assert(rc >= 0);
        while (!0) {
            fds = diminuto_mux_wait(&mux, -1);
            diminuto_assert(fds > 0);
            while (!0) {
                ready = diminuto_mux_ready_accept(&mux);
                if (ready < 0) {
                    break;
                }
                datum4 = DIMINUTO_IPC4_UNSPECIFIED;
                datum46 = DIMINUTO_IPC_EPHEMERAL;
                fd = diminuto_ipc4_stream_accept_generic(ready, &datum4, &datum46);
                if ((fd < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))) {
                    break;
                }
                diminuto_assert(fd >= 0);
                diminuto_assert(!diminuto_ipc4_is_unspecified(&datum4));
                diminuto_assert(!diminuto_ipc_is_ephemeral(datum46));
                DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream sock=%d datum4=%s datum46=%s\n", fd, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));
                rc = diminuto_mux_register_read(&mux, fd);
                diminuto_assert(rc >= 0);
            }
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                input = diminuto_fd_read(fd, buffer, blocksize);
                diminuto_assert(input >= 0);
                if (input == 0) {
                    rc = diminuto_ipc_close(fd);
                    diminuto_assert(rc >= 0);
                    rc = diminuto_mux_unregister_read(&mux, fd);
                    diminuto_assert(rc >= 0);
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream sock=%d input=%zd\n", fd, input);
                } else {
                    output = diminuto_fd_write(fd, buffer, input);
                    diminuto_assert(output == input);
                    if (Verbose) { stamp(tee); fprintf(tee, "%d \"", fd); emit(tee, buffer, input); fputs("\"\n", tee); }
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                }
            }
        }
        /* Can never reach here but if we did this is what we would do. */
        diminuto_assert(fds == 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv6 - TCP
 ******************************************************************************/

    else if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '6') && (Layer3 == 't')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_accept(&mux, sock);
        diminuto_assert(rc >= 0);
        while (!0) {
            fds = diminuto_mux_wait(&mux, -1);
            diminuto_assert(fds > 0);
            while (!0) {
                ready = diminuto_mux_ready_accept(&mux);
                if (ready < 0) {
                    break;
                }
                datum6 = DIMINUTO_IPC6_UNSPECIFIED;
                datum46 = DIMINUTO_IPC_EPHEMERAL;
                fd = diminuto_ipc6_stream_accept_generic(ready, &datum6, &datum46);
                if ((fd < 0) && ((errno == EWOULDBLOCK) || (errno == EAGAIN))) {
                    break;
                }
                diminuto_assert(fd >= 0);
                diminuto_assert(!diminuto_ipc6_is_unspecified(&datum6));
                diminuto_assert(!diminuto_ipc_is_ephemeral(datum46));
                DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream sock=%d datum6=%s datum46=%s\n", fd, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), diminuto_ipc_port2string(datum46, datum46buffer, sizeof(datum46buffer)));
                rc = diminuto_mux_register_read(&mux, fd);
                diminuto_assert(rc >= 0);
            }
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                input = diminuto_fd_read(fd, buffer, blocksize);
                diminuto_assert(input >= 0);
                if (input == 0) {
                    rc = diminuto_ipc_close(fd);
                    diminuto_assert(rc >= 0);
                    rc = diminuto_mux_unregister_read(&mux, fd);
                    diminuto_assert(rc >= 0);
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream sock=%d input=%zd\n", fd, input);
                } else {
                    output = diminuto_fd_write(fd, buffer, input);
                    diminuto_assert(output == input);
                    if (Verbose) { stamp(tee); fprintf(tee, "%d \"", fd); emit(tee, buffer, input); fputs("\"\n", tee); }
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                }
            }
        }
        /* Can never reach here but if we did this is what we would do. */
        diminuto_assert(fds == 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv4 - UDP
 ******************************************************************************/

    else if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '4') && (Layer3 == 'u')) {

        while (!0) {
            datum4 = DIMINUTO_IPC4_UNSPECIFIED;
            datum46 = DIMINUTO_IPC_EPHEMERAL;
            input = diminuto_ipc4_datagram_receive_generic(sock, buffer, blocksize, &datum4, &datum46, 0);
            diminuto_assert(input > 0);
            diminuto_assert(!diminuto_ipc4_is_unspecified(&datum4));
            diminuto_assert(!diminuto_ipc_is_ephemeral(datum46));
            output = diminuto_ipc4_datagram_send_generic(sock, buffer, input, datum4, datum46, 0);
            diminuto_assert(output == input);
            if (Verbose) { stamp(tee); fprintf(tee, "%s:%d \"", diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), datum46); emit(tee, buffer, input); fputs("\"\n", tee); }
            if (Debug) { diminuto_dump(tee, buffer, input); }
        }
        /* Can never reach here but if we did this is what we would do. */
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv6 - UDP
 ******************************************************************************/

    else if (diminuto_ipc_is_ephemeral(rendezvous46) && (Layer2 == '6') && (Layer3 == 'u')) {

        while (!0) {
            datum6 = DIMINUTO_IPC6_UNSPECIFIED;
            datum46 = DIMINUTO_IPC_EPHEMERAL;
            input = diminuto_ipc6_datagram_receive_generic(sock, buffer, blocksize, &datum6, &datum46, 0);
            diminuto_assert(input > 0);
            diminuto_assert(!diminuto_ipc6_is_unspecified(&datum6));
            diminuto_assert(!diminuto_ipc_is_ephemeral(datum46));
            output = diminuto_ipc6_datagram_send_generic(sock, buffer, input, datum6, datum46, 0);
            diminuto_assert(output == input);
            if (Verbose) { stamp(tee); fprintf(tee, "[%s]:%d \"", diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), datum46); emit(tee, buffer, input); fputs("\"\n", tee); }
            if (Debug) { diminuto_dump(tee, buffer, input); }
        }
        /* Can never reach here but if we did this is what we would do. */
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);

    }

/*******************************************************************************
 * CONSUME SERVICE - IPv4 or IPv6 - TCP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && ((Layer2 == '4') || (Layer2 == '6')) && (Layer3 == 't')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_read(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, source);
        diminuto_assert(rc >= 0);
        while ((!eof) || (total > 0)) {
            fds = diminuto_mux_wait(&mux, -1);
            diminuto_assert(fds > 0);
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                if (fd == sock) {
                    input = diminuto_fd_read(sock, buffer, blocksize);
                    diminuto_assert(input > 0);
                    output = diminuto_fd_write(sink, buffer, input);
                    diminuto_assert(output == input);
                    total -= output;
                    if (Verbose) { stamp(tee); fprintf(tee, "%d \"", fd); emit(tee, buffer, input); fputs("\"\n", tee); }
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                } else if (fd == source) {
                    input = diminuto_fd_read(source, buffer, blocksize);
                    diminuto_assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=consumer type=stream sock=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, source);
                        diminuto_assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_fd_write(sock, buffer, input);
                        diminuto_assert(output == input);
                        total += output;
                    }
                } else {
                    diminuto_assert(0);
                }
            }
        }
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);
        diminuto_mux_fini(&mux);
    }

/*******************************************************************************
 * CONSUME SERVICE - IPv4 - UDP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && (Layer2 == '4') && (Layer3 == 'u')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_read(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, source);
        diminuto_assert(rc >= 0);
        while ((!eof) || (total > 0)) {
            fds = diminuto_mux_wait(&mux, -1);
            diminuto_assert(fds > 0);
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                if (fd == sock) {
                    datum4 = DIMINUTO_IPC4_UNSPECIFIED;
                    datum46 = DIMINUTO_IPC_EPHEMERAL;
                    input = diminuto_ipc4_datagram_receive_generic(sock, buffer, blocksize, &datum4, &datum46, 0);
                    diminuto_assert(input > 0);
#if 0
                    diminuto_assert(diminuto_ipc4_compare(&datum4, &server4) == 0);
#endif
                    diminuto_assert(datum46 == rendezvous46);
                    output = diminuto_fd_write(sink, buffer, input);
                    diminuto_assert(output == input);
                    total -= output;
                    if (Verbose) { stamp(tee); fprintf(tee, "%s:%d \"", diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)), datum46); emit(tee, buffer, input); fputs("\"\n", tee); }
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                } else if (fd == source) {
                    input = diminuto_fd_read(source, buffer, blocksize);
                    diminuto_assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=consumer type=datagram sock=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, source);
                        diminuto_assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_ipc4_datagram_send_generic(sock, buffer, input, server4, rendezvous46, 0);
                        diminuto_assert(output == input);
                        total += output;
                    }
                } else {
                    diminuto_assert(0);
                }
            }
        }
        rc = diminuto_mux_unregister_read(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * CONSUME SERVICE - IPv6 - UDP
 ******************************************************************************/

    else if ((!diminuto_ipc_is_ephemeral(rendezvous46)) && (Layer2 == '6') && (Layer3 == 'u')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_read(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, source);
        diminuto_assert(rc >= 0);
        while ((!eof) || (total > 0)) {
            fds = diminuto_mux_wait(&mux, -1);
            diminuto_assert(fds > 0);
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                if (fd == sock) {
                    datum6 = DIMINUTO_IPC6_UNSPECIFIED;
                    datum46 = DIMINUTO_IPC_EPHEMERAL;
                    input = diminuto_ipc6_datagram_receive_generic(sock, buffer, blocksize, &datum6, &datum46, 0);
                    diminuto_assert(input > 0);
#if 0
                    diminuto_assert(diminuto_ipc6_compare(&datum6, &server6) == 0);
#endif
                    diminuto_assert(datum46 == rendezvous46);
                    output = diminuto_fd_write(sink, buffer, input);
                    diminuto_assert(output == input);
                    total -= output;
                    if (Verbose) { stamp(tee); fprintf(tee, "[%s]:%d \"", diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)), datum46); emit(tee, buffer, input); fputs("\"\n", tee); }
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                } else if (fd == source) {
                    input = diminuto_fd_read(source, buffer, blocksize);
                    diminuto_assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=consumer type=datagram sock=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, source);
                        diminuto_assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_ipc6_datagram_send_generic(sock, buffer, input, server6, rendezvous46, 0);
                        diminuto_assert(output == input);
                        total += output;
                    }
                } else {
                    diminuto_assert(0);
                }
            }
        }
        rc = diminuto_mux_unregister_read(&mux, sock);
        diminuto_assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        diminuto_assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * PING - IPv4 - ICMP Echo Request
 ******************************************************************************/

    else if ((Layer2 == '4') && (Layer3 == 'g')) {

        delay = diminuto_frequency();
        srandom(diminuto_time_clock());
        ii = random();
        ss = 0;
        while (!0) {
            output = diminuto_ping4_datagram_send(sock, server4, ii, ss);
            diminuto_assert(output > 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=request id=0x%4.4x sn=%u to=%s\n", ii, ss, diminuto_ipc4_address2string(server4, server4buffer, sizeof(server4buffer)));
            do {
                input = diminuto_ping4_datagram_receive(sock, &datum4, &type, &code, &id, &sn, &ttl, &elapsed);
                diminuto_assert(input >= 0);
            } while (input == 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=reply id=0x%4.4x sn=%u ttl=%u elapsed=%lfs from=%s\n", id, sn, ttl, (double)elapsed / delay, diminuto_ipc4_address2string(datum4, datum4buffer, sizeof(datum4buffer)));
#if 0
            diminuto_assert(diminuto_ipc4_compare(&datum4, &server4) == 0);
#endif
            diminuto_assert(id == ii);
            diminuto_assert(sn == ss);
            ss += 1;
            diminuto_delay(delay, 0);
        }

    }

/*******************************************************************************
 * PING - IPv6 - ICMP Echo Request
 ******************************************************************************/

    else if ((Layer2 == '6') && (Layer3 == 'g')) {

        delay = diminuto_frequency();
        srandom(diminuto_time_clock());
        ii = random();
        ss = 0;
        while (!0) {
            output = diminuto_ping6_datagram_send(sock, server6, ii, ss);
            diminuto_assert(output > 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=request id=0x%4.4x sn=%u to=%s\n", ii, ss, diminuto_ipc6_address2string(server6, server6buffer, sizeof(server6buffer)));
            do {
                input = diminuto_ping6_datagram_receive(sock, &datum6, &type, &code, &id, &sn, &elapsed);
                diminuto_assert(input >= 0);
            } while (input == 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=reply id=0x%4.4x sn=%u elapsed=%lfs from=%s\n", id, sn, (double)elapsed / delay, diminuto_ipc6_address2string(datum6, datum6buffer, sizeof(datum6buffer)));
#if 0
            diminuto_assert(diminuto_ipc6_compare(&datum6, &server6) == 0);
#endif
            diminuto_assert(id == ii);
            diminuto_assert(sn == ss);
            ss += 1;
            diminuto_delay(delay, 0);
        }

    }

/*******************************************************************************
 * DONE
 ******************************************************************************/

    else {

        diminuto_assert(0);

    }

    exit(0);
}
