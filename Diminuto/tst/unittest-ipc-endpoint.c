/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Endpoint portion of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Endpoint portion of the IPC feature.
 */

#include <stdlib.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_dump.h"

#define FQDN "diag.com"
#define IPV4 "205.178.189.131"
#define IPV6 "2607:f8b0:400f:805::200e"
#define PORT "8888"
#define TCP "http"
#define UDP "tftp"
#define EITHER "time"
#define LOCAL "unix-domain.sock"

static const char * type2string(diminuto_ipc_type_t type)
{
    static char buffer[sizeof(int) * 3];
    const char * result = "invalid";
    switch (type) {
    case DIMINUTO_IPC_TYPE_UNSPECIFIED: result = "UNSPECIFIED"; break;
    case DIMINUTO_IPC_TYPE_IPV4:        result = "IPV4";        break;
    case DIMINUTO_IPC_TYPE_IPV6:        result = "IPV6";        break;
    case DIMINUTO_IPC_TYPE_LOCAL:       result = "LOCAL";       break;
    default:
        snprintf(buffer, sizeof(buffer), "%d", type);
        buffer[sizeof(buffer) - 1] = '\0';
        result = buffer;
        break;
    }
    return result;
}

#define SETUP \
    char * endpoint = ""; \
    diminuto_ipv4_buffer_t ipv4buffer = { '\0', }; \
    diminuto_ipv6_buffer_t ipv6buffer = { '\0', }; \
    diminuto_ipc_endpoint_t parse = { DIMINUTO_IPC_TYPE_UNSPECIFIED, }; \
    int rc = -1 \

#define DISPLAY \
        COMMENT("endpoint=\"%s\" type=%s ipv4=%s ipv6=%s tcp=%d udp=%d local=\"%s\" rc=%d\n", \
            endpoint, \
            type2string(parse.type), \
            diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), \
            diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), \
            parse.tcp, \
            parse.udp, \
            parse.local, \
            rc)

#define VERIFYINET4(_POINTER_, _IPV4_, _IPV6_, _TCP_, _UDP_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(rc == 0); \
        EXPECT(diminuto_ipc4_compare(&(_pp_->ipv4), &(_IPV4_)) == 0); \
        EXPECT(diminuto_ipc6_compare(&(_pp_->ipv6), &(_IPV6_)) == 0); \
        EXPECT(_pp_->tcp == (_TCP_)); \
        EXPECT(_pp_->udp == (_UDP_)); \
        EXPECT(_pp_->local[0] == '\0'); \
        EXPECT(_pp_->type == AF_INET); \
    } while (0)

#define VERIFYINET6(_POINTER_, _IPV4_, _IPV6_, _TCP_, _UDP_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(rc == 0); \
        EXPECT(diminuto_ipc4_compare(&(_pp_->ipv4), &(_IPV4_)) == 0); \
        EXPECT(diminuto_ipc6_compare(&(_pp_->ipv6), &(_IPV6_)) == 0); \
        EXPECT(_pp_->tcp == (_TCP_)); \
        EXPECT(_pp_->udp == (_UDP_)); \
        EXPECT(_pp_->local[0] == '\0'); \
        EXPECT(_pp_->type == AF_INET6); \
    } while (0)

#define VERIFYUNIX(_POINTER_, _PATH_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(rc == 0); \
        EXPECT(diminuto_ipc4_compare(&(_pp_->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) == 0); \
        EXPECT(diminuto_ipc6_compare(&(_pp_->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) == 0); \
        EXPECT(_pp_->tcp == 0); \
        EXPECT(_pp_->udp == 0); \
        ADVISE(strcmp(_pp_->local, (_PATH_)) == 0); \
        EXPECT(_pp_->type == AF_UNIX); \
    } while (0)

int main(int argc, char * argv[])
{
    const char fqdn[] = FQDN;
    diminuto_ipv4_t unspecified4;
    diminuto_ipv6_t unspecified6;
    diminuto_ipv4_t localhost4;
    diminuto_ipv6_t localhost6;
    diminuto_ipv4_t fqdn4;
    diminuto_ipv6_t fqdn6;
    diminuto_ipv4_t address4;
    diminuto_ipv6_t address46;
    diminuto_ipv6_t address6;
    diminuto_port_t ephemeral;
    diminuto_port_t porttcp;
    diminuto_port_t portudp;
    diminuto_port_t tcptcp;
    diminuto_port_t tcpudp;
    diminuto_port_t udptcp;
    diminuto_port_t udpudp;
    diminuto_port_t eithertcp;
    diminuto_port_t eitherudp;

    SETLOGMASK();

    if ((argc > 1) && (strcmp(argv[1], "-d") == 0)) {
        extern int diminuto_ipc_endpoint_debug(int);
        (void)diminuto_ipc_endpoint_debug(!0);
    }

    COMMENT("FQDN=\"%s\"\n", FQDN);
    COMMENT("IPV4=\"%s\"\n", IPV4);
    COMMENT("IPV6=\"%s\"\n", IPV6);
    COMMENT("PORT=\"%s\"\n", PORT);
    COMMENT("TCP=\"%s\"\n", TCP);
    COMMENT("UDP=\"%s\"\n", UDP);
    COMMENT("EITHER=\"%s\"\n", EITHER);
    COMMENT("LOCAL=\"%s\"\n", LOCAL);

    unspecified4 = DIMINUTO_IPC4_UNSPECIFIED;
    unspecified6 = DIMINUTO_IPC6_UNSPECIFIED;
    localhost4 = diminuto_ipc4_address("localhost");
    localhost6 = diminuto_ipc6_address("localhost");
    fqdn4 = diminuto_ipc4_address(fqdn);
    fqdn6 = diminuto_ipc6_address(fqdn);
    address4 = diminuto_ipc4_address(IPV4);
    address46 = diminuto_ipc6_address("::ffff:" IPV4);
    address6 = diminuto_ipc6_address(IPV6);
    ephemeral = 0;
    porttcp = atoi(PORT);
    portudp = atoi(PORT);
    tcptcp = diminuto_ipc_port(TCP, "tcp");
    tcpudp = diminuto_ipc_port(TCP, "udp");
    udptcp = diminuto_ipc_port(UDP, "tcp");
    udpudp = diminuto_ipc_port(UDP, "udp");
    eithertcp = diminuto_ipc_port(EITHER, "tcp");
    eitherudp = diminuto_ipc_port(EITHER, "udp");

    {
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        COMMENT("unspecified4=%s\n", diminuto_ipc4_address2string(unspecified4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("unspecified6=%s\n", diminuto_ipc6_address2string(unspecified6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("localhost4=%s\n", diminuto_ipc4_address2string(localhost4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("localhost6=%s\n", diminuto_ipc6_address2string(localhost6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("fqdn4=%s\n", diminuto_ipc4_address2string(fqdn4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("fqdn6=%s\n", diminuto_ipc6_address2string(fqdn6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("address4=%s\n", diminuto_ipc4_address2string(address4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("address46=%s\n", diminuto_ipc6_address2string(address46, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("address6=%s\n", diminuto_ipc6_address2string(address6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("ephemeral=%d\n", ephemeral);
        COMMENT("port tcp=%d\n", porttcp);
        COMMENT("port udp=%d\n", portudp);
        COMMENT("http tcp=%d\n", tcptcp);
        COMMENT("http udp=%d\n", tcpudp);
        COMMENT("tftp tcp=%d\n", udptcp);
        COMMENT("tftp udp=%d\n", udpudp);
        COMMENT("time tcp=%d\n", eithertcp);
        COMMENT("time udp=%d\n", eitherudp);
    }

    /*
     * Below are the sunny day scenarios I expect to succeed. To test
     * arbitary endpoint strings, see the endpoint utility.
     */

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = PORT, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" PORT, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = TCP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" TCP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = UDP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" UDP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = EITHER, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" EITHER, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" PORT, &parse);
        DISPLAY;
        VERIFYINET6(&parse, localhost4, localhost6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" TCP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, localhost4, localhost6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" UDP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, localhost4, localhost6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" EITHER, &parse);
        DISPLAY;
        VERIFYINET6(&parse, localhost4, localhost6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN, &parse);
        DISPLAY;
        VERIFYINET6(&parse, fqdn4, fqdn6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":0", &parse);
        DISPLAY;
        VERIFYINET6(&parse, fqdn4, fqdn6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":" PORT, &parse);
        VERIFYINET6(&parse, fqdn4, fqdn6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":" TCP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, fqdn4, fqdn6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":" UDP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, fqdn4, fqdn6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":" EITHER, &parse);
        DISPLAY;
        VERIFYINET6(&parse, fqdn4, fqdn6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0", &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:0", &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" PORT, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" TCP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" UDP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" EITHER, &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4, &parse);
        DISPLAY;
        VERIFYINET4(&parse, address4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":0", &parse);
        DISPLAY;
        VERIFYINET4(&parse, address4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" PORT, &parse);
        DISPLAY;
        VERIFYINET4(&parse, address4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" TCP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, address4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" UDP, &parse);
        DISPLAY;
        VERIFYINET4(&parse, address4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" EITHER, &parse);
        DISPLAY;
        VERIFYINET4(&parse, address4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]", &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:0", &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" PORT, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" TCP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" UDP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" EITHER, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]", &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address46, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:0", &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address46, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" PORT, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address46, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" TCP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address46, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" UDP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address46, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" EITHER, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address46, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]", &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:0", &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" PORT, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address6, porttcp, portudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" TCP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address6, tcptcp, tcpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" UDP, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address6, udptcp, udpudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" EITHER, &parse);
        DISPLAY;
        VERIFYINET6(&parse, unspecified4, address6, eithertcp, eitherudp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0", &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":0", &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "", &parse);
        DISPLAY;
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/" LOCAL, &parse);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "./" LOCAL, &parse);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "../" LOCAL, &parse);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/run/" LOCAL, &parse);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/var/tmp/" LOCAL, &parse);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = LOCAL, &parse);
        DISPLAY;
        EXPECT(rc < 0);
        EXPECT(parse.type == DIMINUTO_IPC_TYPE_UNSPECIFIED);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/I-hope-this-directory-does-not-exist/" LOCAL, &parse);
        DISPLAY;
        EXPECT(rc < 0);
        EXPECT(parse.type == DIMINUTO_IPC_TYPE_UNSPECIFIED);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "I-hope-this-domain-name-is-unresolvable.com", &parse);
        DISPLAY;
        EXPECT(rc < 0);
        EXPECT(parse.type == DIMINUTO_IPC_TYPE_UNSPECIFIED);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "(I-hope-this-does-not-look-like-anything-valid)", &parse);
        DISPLAY;
        EXPECT(rc < 0);
        EXPECT(parse.type == DIMINUTO_IPC_TYPE_UNSPECIFIED);
        STATUS();
    }

    EXIT();
}
