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
 * internettool -\?                                                                         # Help Menu
 * 
 * internettool -6 -p 5555                                                                  # Service Provider
 * internettool -6 -A 2001:470:4b:4e2:e79:7f1e:21f5:9355 -P 5555 < OLDFILE > NEWFILE        # Service Consumer
 *
 * internettool -4 -p 5555                                                                  # Service Provider
 * internettool -4 -A 192.168.2.182 -P 5555 < OLDFILE > NEWFILE                             # Service Consumer
 *
 * internettool -6 -t -p 5555                                                               # Service Provider
 * datesource | internettool -6 -t -A 2001:470:4b:4e2:8eae:4cff:fef4:40c -P 5555 | datesink # Service Consumer 1
 * datesource | internettool -4 -t -A 192.168.1.237 -P 5555 | datesink                      # Service Consumer 2
 * timesource | internettool -6 -t -A ::ffff:192.168.1.237 | timesink                       # Service Consumer 3
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_ping4.h"
#include "com/diag/diminuto/diminuto_ping6.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#undef NDEBUG
#include <assert.h>

static inline const char * newline(const char * buffer, ssize_t size)
{
    return (((size > 0) && (buffer[size - 1] == '\n')) || ((size > 1) && (buffer[size - 1] == '\0') && (buffer[size - 2] == '\n'))) ? "" : "\n";
}

int main(int argc, char * argv[])
{
    const char * Program = (const char *)0;
    const char * Address = (const char *)0;
    const char * Interface = (const char *)0;
    const char * Port = (const char *)0;
    const char * Server =  (const char *)0;
    const char * Rendezvous = (const char *)0;
    const char * Blocksize = (const char *)0;
    char Layer2 = '4';
    char Layer3 = 't';
    char Debug = '\0';
    char Verbose = '\0';
    char Interfaces = '\0';
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
    size_t blocksize = 512;
    int source = STDIN_FILENO;
    int sink = STDOUT_FILENO;
    FILE * tee = stderr;
    char string[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")] = { '\0' };
    char * buffer = (char *)0;
    ssize_t input = 0;
    ssize_t output = 0;
    ssize_t total = 0;
    int sock = -2;
    int fd = -3;
    int rc = 0;
    int fds = 0;
    int eof = 0;
    uint8_t type = 0;
    uint8_t code = 0;
    uint16_t id = 0;
    uint16_t sn = 0;
    uint8_t ttl = 0;
    uint16_t ii;
    uint16_t ss;
    diminuto_unsigned_t value;
    char ** interfaci;
    char ** interfaces;
    diminuto_mux_t mux;
    diminuto_ticks_t delay;
    diminuto_ticks_t elapsed;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

/*******************************************************************************
 * PARSE
 ******************************************************************************/

    diminuto_log_setmask();

    Program = ((Program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : Program + 1;

    while ((opt = getopt(argc, argv, "46?A:IP:a:b:dgi:p:tuv")) >= 0) {
        switch (opt) {
        case '4':
        case '6':
            Layer2 = opt; /* IPv4 or IPv6 */
            break;
        case 'A':
            Server = optarg; /* far end server address */
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
        case '?':
            fprintf(stderr, "usage: %s [ -? ] [ -I ] [ -4 | -6 ] [ -t | -u | -g ] [ -I INFD ] [ -O OUTFD ] [ -a NEADDR ] [ -p NEPORT ] [ -i NEINTF ] [ -A FEADDR ] [ -P FEPORT ] [ -b BYTES ]\n", Program);
            fprintf(stderr, "       -?          Display this menu.\n");
            fprintf(stderr, "       -4          Use IPv4.\n");
            fprintf(stderr, "       -6          Use IPv6.\n");
            fprintf(stderr, "       -A FEADDR   Connect far end socket to host or address FEADDR.\n");
            fprintf(stderr, "       -I          Display interfaces and exit.\n");
            fprintf(stderr, "       -P FEPORT   Connect far end socket to service or port FEPORT.\n");
            fprintf(stderr, "       -a NEADDR   Bind near end socket to host or address NEADDR.\n");
            fprintf(stderr, "       -b BYTES    Size input/output buffer to BYTES bytes.\n");
            fprintf(stderr, "       -d          Dump received data to standard error on provider.\n");
            fprintf(stderr, "       -g          Use ICMP echo request (ping).\n");
            fprintf(stderr, "       -i NEINTF   Bind near end socket to interface NEINTF.\n");
            fprintf(stderr, "       -p NEPORT   Bind near end socket to service or port NEPORT.\n");
            fprintf(stderr, "       -t          Use TCP.\n");
            fprintf(stderr, "       -u          Use UDP.\n");
            fprintf(stderr, "       -v          Print received data to standard error on provider.\n");
            fprintf(stderr, "       -?          Display this menu and exit.\n");
            return 1;
            break;
        default:
            break;
        }
    }

/*******************************************************************************
 * IMMEDIATES
 ******************************************************************************/

    if (Interfaces) {
        char ** ifvp;
        char ** ifp;
        diminuto_ipv4_t * v4vp;
        diminuto_ipv4_t * v4p;
        diminuto_ipv6_t * v6vp;
        diminuto_ipv6_t * v6p;
        const char * type;

        assert((ifvp = diminuto_ipc_interfaces()) != (char **)0);

        for (ifp = ifvp; *ifp != (char *)0; ++ifp) {

            assert((v4vp = diminuto_ipc4_interface(*ifp)) != (diminuto_ipv4_t *)0);
            assert((v6vp = diminuto_ipc6_interface(*ifp)) != (diminuto_ipv6_t *)0);

            if (*v4vp != DIMINUTO_IPC4_UNSPECIFIED) {
                /* Do nothing. */
            } else if (diminuto_ipc6_compare(v6vp, &DIMINUTO_IPC6_UNSPECIFIED) != 0) {
                /* Do nothing. */
            } else {
                DIMINUTO_LOG_NOTICE("%s %s\n", DIMINUTO_LOG_HERE, *ifp);
                continue;
            }

            for (v4p = v4vp; *v4p != DIMINUTO_IPC4_UNSPECIFIED; ++v4p) {
                if (diminuto_ipc4_is_unspecified(v4p)) {
                    type = "unspecified"; /* IMpossible. */
                } else if (diminuto_ipc4_is_limitedbroadcast(v4p)) {
                    type = "limited-broadcast";
                } else if (diminuto_ipc4_is_loopback(v4p)) {
                    type = "loopback";
                } else if (diminuto_ipc4_is_private(v4p)) {
                    type = "private";
                } else if (diminuto_ipc4_is_multicast(v4p)) {
                    type = "multicast";
                } else {
                    type = "other";
                }
                DIMINUTO_LOG_NOTICE("%s %s v4 %s %s\n", DIMINUTO_LOG_HERE, *ifp, diminuto_ipc4_dotnotation(*v4p, string, sizeof(string)), type);
            }

            for (v6p = v6vp; diminuto_ipc6_compare(v6p, &DIMINUTO_IPC6_UNSPECIFIED) != 0; ++v6p) {
                if (diminuto_ipc6_is_unspecified(v6p)) {
                    type = "unspecified"; /* Impossible. */
                } else if (diminuto_ipc6_is_loopback(v6p)) {
                    type = "loopback";
                } else if (diminuto_ipc6_is_unicastglobal(v6p)) {
                    type = "global-unicast";
                } else if (diminuto_ipc6_is_uniquelocal(v6p)) {
                    type = "unique-local";
                } else if (diminuto_ipc6_is_linklocal(v6p)) {
                    type = "link-local";
                } else if (diminuto_ipc6_is_multicast(v6p)) {
                    type = "multicast";
                } else if (diminuto_ipc6_is_nat64wkp(v6p)) {
                    type = "nat64-wkp";
                } else if (diminuto_ipc6_is_isatap(v6p)) {
                    type = "atap";
                } else if (diminuto_ipc6_is_6to4(v6p)) {
                    type = "6to4";
                } else if (diminuto_ipc6_is_loopback4(v6p)) {
                    type = "v4-loopback";
                } else if (diminuto_ipc6_is_v4mapped(v6p)) {
                    type = "v4-mapped";
                } else if (diminuto_ipc6_is_v4compatible(v6p)) {
                    type = "v4-compatible";
                } else {
                    type = "other";
                }
                DIMINUTO_LOG_NOTICE("%s %s v6 %s %s\n", DIMINUTO_LOG_HERE, *ifp, diminuto_ipc6_colonnotation(*v6p, string, sizeof(string)), type);
            }

            free(v4vp);
            free(v6vp);
        }

        free(ifvp);

        return 0;
    }

/*******************************************************************************
 * PARAMETERS
 ******************************************************************************/

    if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer2=IPv4\n");
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer2=IPv6\n");
    } else {
        assert(0);
    }

    if (Layer3 == 't') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer3=TCP\n");
    } else if (Layer3 == 'g') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer3=ICMP Echo Request\n");
    } else if (Layer3 == 'u') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Layer3=UDP\n");
    } else {
        assert(0);
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
        assert(0);
    }

    if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address4=%s\n",  diminuto_ipc4_address2string(address4, string, sizeof(string)));
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address6=%s\n",  diminuto_ipc6_address2string(address6, string, sizeof(string)));
    } else {
        assert(0);
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
    } else if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Server4=\"%s\"\n", Server);
        server4 = diminuto_ipc4_address(Server);
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Server6=\"%s\"\n", Server);
        server6 = diminuto_ipc6_address(Server);
    } else {
        assert(0);
    }

    if (Layer2 == '4') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "server4=%s\n", diminuto_ipc4_address2string(server4, string, sizeof(string)));
    } else if (Layer2 == '6') {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "server6=%s\n", diminuto_ipc6_address2string(server6, string, sizeof(string)));
    } else {
        assert(0);
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
        if (Layer2 == '4') {
            addresses4 = diminuto_ipc4_interface(interface);
            assert(addresses4 != (diminuto_ipv4_t *)0);
            for (; *addresses4 != DIMINUTO_IPC4_UNSPECIFIED; ++addresses4) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface4=%s\n", diminuto_ipc4_address2string(*addresses4, string, sizeof(string)));
            }
        } else if (Layer2 == '6') {
            addresses6 = diminuto_ipc6_interface(interface);
            assert(addresses6 != (diminuto_ipv6_t *)0);
            for (; memcmp(addresses6, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(*addresses6)) != 0; ++addresses6) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface6=%s\n", diminuto_ipc6_address2string(*addresses6, string, sizeof(string)));
            }
        } else {
            assert(0);
        }
        free(interfaci);
    }

    if (Blocksize == (const char *)0) {
        /* Do nothing. */
    } else if (*diminuto_number_unsigned(Blocksize, &value) != '\0') {
        assert(0);
    } else {
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "Blocksize=\"%s\"\n", Blocksize);
        blocksize = value;
    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "blocksize=%zu\n", blocksize);

    buffer = (char *)malloc(blocksize);
    assert(buffer != (char *)0);

/*******************************************************************************
 * SERVICE PROVIDER - IPv4 - TCP
 ******************************************************************************/

    if ((Rendezvous == (const char *)0) && (Layer2 == '4') && (Layer3 == 't')) {

         sock = diminuto_ipc4_stream_provider_generic(address4, port46, Interface, -1);
         assert(sock >= 0);
         rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
         assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near type=stream sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE PROVIDER - IPv4 - UDP
 ******************************************************************************/

    else if ((Rendezvous == (const char *)0) && (Layer2 == '4') && (Layer3 == 'u')) {

         sock = diminuto_ipc4_datagram_peer_generic(address4, port46, Interface);
         assert(sock >= 0);
         rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
         assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near type=datagram sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE PROVIDER - IPv6 - TCP
 ******************************************************************************/

    else if ((Rendezvous == (const char *)0) && (Layer2 == '6') && (Layer3 == 't')) {

         sock = diminuto_ipc6_stream_provider_generic(address6, port46, Interface, -1);
         assert(sock >= 0);
         rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
         assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE PROVIDER - IPv6 - UDP
 ******************************************************************************/

    else if ((Rendezvous == (const char *)0) && (Layer2 == '6') && (Layer3 == 'u')) {

         sock = diminuto_ipc6_datagram_peer_generic(address6, port46, Interface);
         assert(sock >= 0);
         rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
         assert(rc >= 0);
         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=provider end=near type=datagram sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv4 - TCP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && (Layer2 == '4') && (Layer3 == 't')) {

        sock = diminuto_ipc4_stream_consumer_generic(server4, rendezvous46, address4, port46, Interface);
        assert(sock >= 0);
        rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=stream sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
        rc = diminuto_ipc4_farend(sock, &datum4, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=far type=stream sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv4 - UDP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && (Layer2 == '4') && (Layer3 == 'u')) {

        sock = diminuto_ipc4_datagram_peer_generic(address4, port46, Interface);
        assert(sock >= 0);
        rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=datagram sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv6 - TCP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && (Layer2 == '6') && (Layer3 == 't')) {

        sock = diminuto_ipc6_stream_consumer_generic(server6, rendezvous46, address6, port46, Interface);
        assert(sock >= 0);
        rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=stream sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
        rc = diminuto_ipc6_farend(sock, &datum6, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=far type=stream sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * SERVICE CONSUMER - IPv6 - UDP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && (Layer2 == '6') && (Layer3 == 'u')) {

        sock = diminuto_ipc6_datagram_peer_generic(address6, port46, Interface);
        assert(sock >= 0);
        rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=consumer end=near type=datagram sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * PING - IPv4 - ICMP Echo Request
 ******************************************************************************/

    else if ((Layer2 == '4') && (Layer3 == 'g')) {

        sock = diminuto_ping4_datagram_peer();
        assert(sock >= 0);
        rc = diminuto_ipc4_source(sock, address4, port46);
        assert(rc >= 0);
        rc = diminuto_ipc_set_interface(sock, interface);
        assert(rc >= 0);
        rc = diminuto_ipc4_nearend(sock, &datum4, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=ping end=near type=datagram sock=%d datum4=%s datum46=%d\n", sock, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * PING - IPv6 - ICMP Echo Request
 ******************************************************************************/

    else if ((Layer2 == '6') && (Layer3 == 'g')) {

        sock = diminuto_ping6_datagram_peer();
        assert(sock >= 0);
        rc = diminuto_ipc6_source(sock, address6, port46);
        assert(rc >= 0);
        rc = diminuto_ipc_set_interface(sock, interface);
        assert(rc >= 0);
        rc = diminuto_ipc6_nearend(sock, &datum6, &datum46);
        assert(rc >= 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "role=ping end=near type=datagram sock=%d datum6=%s datum46=%d\n", sock, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);

    }

/*******************************************************************************
 * DONE
 ******************************************************************************/

    else {

        assert(0);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv4 - TCP
 ******************************************************************************/

    if ((Rendezvous == (const char *)0) && (Layer2 == '4') && (Layer3 == 't')) {

        diminuto_mux_init(&mux);
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
                datum4 = DIMINUTO_IPC4_UNSPECIFIED;
                datum46 = 0;
                fd = diminuto_ipc4_stream_accept_generic(fd, &datum4, &datum46);
                assert(fd >= 0);
                assert(datum4 != DIMINUTO_IPC4_UNSPECIFIED);
                assert(datum46 != 0);
                DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream fd=%d datum4=%s datum46=%d\n", fd, diminuto_ipc4_address2string(datum4, string, sizeof(string)), datum46);
                rc = diminuto_mux_register_read(&mux, fd);
                assert(rc >= 0);
            }
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                input = diminuto_fd_read(fd, buffer, blocksize);
                assert(input >= 0);
                if (input == 0) {
                    rc = diminuto_ipc_close(fd);
                    assert(rc >= 0);
                    rc = diminuto_mux_unregister_read(&mux, fd);
                    assert(rc >= 0);
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream fd=%d input=%zd\n", fd, input);
                } else {
                    output = diminuto_fd_write(fd, buffer, input);
                    assert(output == input);
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                    if (Verbose) { fprintf(tee, "%.*s%s", (int)input, buffer, newline(buffer, input)); }
                }
            }
        }
        /* Can never reach here but if we did this is what we would do. */
        assert(fds == 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv6 - TCP
 ******************************************************************************/

    else if ((Rendezvous == (const char *)0) && (Layer2 == '6') && (Layer3 == 't')) {

        diminuto_mux_init(&mux);
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
                datum6 = DIMINUTO_IPC6_UNSPECIFIED;
                datum46 = 0;
                fd = diminuto_ipc6_stream_accept_generic(fd, &datum6, &datum46);
                assert(fd >= 0);
                assert(memcmp(&datum6, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(datum6)) != 0);
                assert(datum46 != 0);
                DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream fd=%d datum6=%s datum46=%d\n", fd, diminuto_ipc6_address2string(datum6, string, sizeof(string)), datum46);
                rc = diminuto_mux_register_read(&mux, fd);
                assert(rc >= 0);
            }
            while (!0) {
                fd = diminuto_mux_ready_read(&mux);
                if (fd < 0) {
                    break;
                }
                input = diminuto_fd_read(fd, buffer, blocksize);
                assert(input >= 0);
                if (input == 0) {
                    rc = diminuto_ipc_close(fd);
                    assert(rc >= 0);
                    rc = diminuto_mux_unregister_read(&mux, fd);
                    assert(rc >= 0);
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=provider end=far type=stream fd=%d input=%zd\n", fd, input);
                } else {
                    output = diminuto_fd_write(fd, buffer, input);
                    assert(output == input);
                    if (Debug) { diminuto_dump(tee, buffer, input); }
                    if (Verbose) { fprintf(tee, "%.*s%s", (int)input, buffer, newline(buffer, input)); }
                }
            }
        }
        /* Can never reach here but if we did this is what we would do. */
        assert(fds == 0);
        rc = diminuto_mux_unregister_accept(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv4 - UCP
 ******************************************************************************/

    else if ((Rendezvous == (const char *)0) && (Layer2 == '4') && (Layer3 == 'u')) {

        while (!0) {
            datum4 = DIMINUTO_IPC4_UNSPECIFIED;
            datum46 = 0;
            input = diminuto_ipc4_datagram_receive_generic(sock, buffer, blocksize, &datum4, &datum46, 0);
            assert(input > 0);
            assert(datum4 != DIMINUTO_IPC4_UNSPECIFIED);
            assert(datum46 != 0);
            output = diminuto_ipc4_datagram_send_generic(sock, buffer, input, datum4, datum46, 0);
            assert(output == input);
            if (Debug) { diminuto_dump(tee, buffer, input); }
            if (Verbose) { fprintf(tee, "%.*s%s", (int)input, buffer, newline(buffer, input)); }
        }
        /* Can never reach here but if we did this is what we would do. */
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);

    }

/*******************************************************************************
 * PROVIDE SERVICE - IPv6 - UCP
 ******************************************************************************/

    else if ((Rendezvous == (const char *)0) && (Layer2 == '6') && (Layer3 == 'u')) {

        while (!0) {
            datum6 = DIMINUTO_IPC6_UNSPECIFIED;
            datum46 = 0;
            input = diminuto_ipc6_datagram_receive_generic(sock, buffer, blocksize, &datum6, &datum46, 0);
            assert(input > 0);
            assert(memcmp(&datum6, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(datum6)) != 0);
            assert(datum46 != 0);
            output = diminuto_ipc6_datagram_send_generic(sock, buffer, input, datum6, datum46, 0);
            assert(output == input);
            if (Debug) { diminuto_dump(tee, buffer, input); }
            if (Verbose) { fprintf(tee, "%.*s%s", (int)input, buffer, newline(buffer, input)); }
        }
        /* Can never reach here but if we did this is what we would do. */
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);

    }

/*******************************************************************************
 * CONSUME SERVICE - IPv4 or IPv6 - TCP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && ((Layer2 == '4') || (Layer2 == '6')) && (Layer3 == 't')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_read(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, source);
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
                    input = diminuto_fd_read(sock, buffer, blocksize);
                    assert(input > 0);
                    output = diminuto_fd_write(sink, buffer, input);
                    assert(output == input);
                    total -= output;
                } else if (fd == source) {
                    input = diminuto_fd_read(source, buffer, blocksize);
                    assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=consumer type=stream fd=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, source);
                        assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_fd_write(sock, buffer, input);
                        assert(output == input);
                        total += output;
                    }
                } else {
                    assert(0);
                }
            }
        }
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);
        diminuto_mux_fini(&mux);
    }

/*******************************************************************************
 * CONSUME SERVICE - IPv4 - UDP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && (Layer2 == '4') && (Layer3 == 'u')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_read(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, source);
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
                    datum4 = DIMINUTO_IPC4_UNSPECIFIED;
                    datum46 = 0;
                    input = diminuto_ipc4_datagram_receive_generic(sock, buffer, blocksize, &datum4, &datum46, 0);
                    assert(input > 0);
#if 0
                    assert(datum4 == server4);
#endif
                    assert(datum46 == rendezvous46);
                    output = diminuto_fd_write(sink, buffer, input);
                    assert(output == input);
                    total -= output;
                } else if (fd == source) {
                    input = diminuto_fd_read(source, buffer, blocksize);
                    assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=consumer type=datagram fd=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, source);
                        assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_ipc4_datagram_send_generic(sock, buffer, input, server4, rendezvous46, 0);
                        assert(output == input);
                        total += output;
                    }
                } else {
                    assert(0);
                }
            }
        }
        rc = diminuto_mux_unregister_read(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);
        diminuto_mux_fini(&mux);

    }

/*******************************************************************************
 * CONSUME SERVICE - IPv6 - UDP
 ******************************************************************************/

    else if ((Rendezvous != (const char *)0) && (Layer2 == '6') && (Layer3 == 'u')) {

        diminuto_mux_init(&mux);
        rc = diminuto_mux_register_read(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, source);
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
                    datum6 = DIMINUTO_IPC6_UNSPECIFIED;
                    datum46 = 0;
                    input = diminuto_ipc6_datagram_receive_generic(sock, buffer, blocksize, &datum6, &datum46, 0);
                    assert(input > 0);
#if 0
                    assert(memcmp(&datum6, &server6, sizeof(datum6)) == 0);
#endif
                    assert(datum46 == rendezvous46);
                    output = diminuto_fd_write(sink, buffer, input);
                    assert(output == input);
                    total -= output;
                } else if (fd == source) {
                    input = diminuto_fd_read(source, buffer, blocksize);
                    assert(input >= 0);
                    if (input == 0) {
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=consumer type=datagram fd=%d input=%zd\n", fd, input);
                        rc = diminuto_mux_unregister_read(&mux, source);
                        assert(rc >= 0);
                        eof = !0;
                    } else {
                        output = diminuto_ipc6_datagram_send_generic(sock, buffer, input, server6, rendezvous46, 0);
                        assert(output == input);
                        total += output;
                    }
                } else {
                    assert(0);
                }
            }
        }
        rc = diminuto_mux_unregister_read(&mux, sock);
        assert(rc >= 0);
        rc = diminuto_ipc_close(sock);
        assert(rc >= 0);
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
            assert(output > 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=request id=0x%4.4x sn=%u to=%s\n", ii, ss, diminuto_ipc4_address2string(server4, string, sizeof(string)));
            do {
                input = diminuto_ping4_datagram_receive(sock, &datum4, &type, &code, &id, &sn, &ttl, &elapsed);
                assert(input >= 0);
            } while (input == 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=reply id=0x%4.4x sn=%u ttl=%u elapsed=%lfs from=%s\n", id, sn, ttl, (double)elapsed / delay, diminuto_ipc4_address2string(datum4, string, sizeof(string)));
#if 0
            assert(datum4 == server4);
#endif
            assert(id == ii);
            assert(sn == ss);
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
            assert(output > 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=request id=0x%4.4x sn=%u to=%s\n", ii, ss, diminuto_ipc6_address2string(server6, string, sizeof(string)));
            do {
                input = diminuto_ping6_datagram_receive(sock, &datum6, &type, &code, &id, &sn, &elapsed);
                assert(input >= 0);
            } while (input == 0);
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "role=ping action=reply id=0x%4.4x sn=%u elapsed=%lfs from=%s\n", id, sn, (double)elapsed / delay, diminuto_ipc6_address2string(datum6, string, sizeof(string)));
#if 0
            assert(memcmp(&datum6, &server6, sizeof(datum6)) == 0);
#endif
            assert(id == ii);
            assert(sn == ss);
            ss += 1;
            diminuto_delay(delay, 0);
        }

    }

/*******************************************************************************
 * DONE
 ******************************************************************************/

    else {

        assert(0);

    }

    exit(0);
}
