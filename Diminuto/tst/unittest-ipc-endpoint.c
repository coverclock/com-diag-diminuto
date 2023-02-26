/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Endpoint portion of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Endpoint portion of the IPC feature.
 *
 * It's really hard to test against production domain names like
 * "google.com": their name servers may return different IPv4 or IPv6
 * addresses for each query, balancing the load across multiple
 * physical servers. Even the address "prairiethorn.org" may change
 * if the underlying server changes or reboots at the wrong time.
 *
 * More useful debugging output can be had by enabling more logging:
 *
 * COM_DIAG_DIMINUTO_LOG_MASK=0xff unittest-ipc-endpoint
 *
 * N.B. I have had this unit test fail because I had misconfigured the
 * dnsresolver on the test system to search "diag.com", and my web server
 * define to respond go "http.diag.com" in addition to "www.diag.com".
 * This caused diminuto_ipc_endpoint() to resolve "http" (without the colon)
 * to the IP address of my web server instead of the HTTP port number (80).
 * It is true that the use of a service name without the leading colon
 * can be ambiguous, but this is intentional on my part in the sense that
 * that case the feature is "working as designed". When I tried to fix the
 * configuration of the resolver on the test system using "resolvectl" and
 * "systemctl", I discovered a bug in "resolvectl" in that the command
 * 
 *      resolvectl domain eth0 ""
 * 
 * did not work as documented on the man page: it did nothing, instead of
 * removing "diag.com" as a search domain. Furthermore, using the command
 *
 *      resolvectl domain eth0 "invalid"
 *
 * appeared to override the old configuration (at the expense of using a
 * non-working search domain), but this configuration change did not persist
 * across reboots. I finally (I think) fixed this by removing the soft link
 * to /etc/resolv.conf and hand coding /etc/resolv.conf, removing the line
 * "search diag.com"; this is not the preferred fix.
 */

#include <stdlib.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "../src/diminuto_ipc.h"

#define FQDN4 "prairiethorn.org"
#define FQDN46 "google.com"
#define IPV4 "205.178.189.131"
#define IPV6 "2607:f8b0:400f:805::200e"
#define PORT "8888"
#define COLONPORT ":8888"
#define TCP "http"
#define COLONTCP ":http"
#define UDP "tftp"
#define COLONUDP ":tftp"
#define EITHER "time"
#define COLONEITHER ":time"
#define UNDEFINED "undefinedthing"
#define COLONUNDEFINED ":undefinedthing"
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

#define PREFACE \
    char * endpoint = (char *)0; \
    diminuto_ipv4_buffer_t ipv4buffer; \
    diminuto_ipv6_buffer_t ipv6buffer; \
    diminuto_port_buffer_t tcpbuffer; \
    diminuto_port_buffer_t udpbuffer; \
    diminuto_endpoint_buffer_t endpointbuffer; \
    diminuto_ipc_endpoint_t parse = { DIMINUTO_IPC_TYPE_UNSPECIFIED, }; \
    int rc = -1; \
    memset(&ipv4buffer, 0, sizeof(ipv4buffer)); \
    memset(&ipv6buffer, 0, sizeof(ipv6buffer)); \
    memset(&tcpbuffer, 0, sizeof(tcpbuffer)); \
    memset(&udpbuffer, 0, sizeof(udpbuffer)); \
    memset(&endpointbuffer, 0, sizeof(endpointbuffer));

#define DISPLAY() \
    COMMENT("endpoint=\"%s\" type=%s ipv4=%s ipv6=%s tcp=%s udp=%s local=\"%s\" rc=%d string=\"%s\"", \
        endpoint, \
        type2string(parse.type), \
        diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), \
        diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), \
        diminuto_ipc_port2string(parse.tcp, tcpbuffer, sizeof(tcpbuffer)), \
        diminuto_ipc_port2string(parse.udp, udpbuffer, sizeof(udpbuffer)), \
        parse.local, \
        rc, \
        diminuto_ipc_endpoint2string(&parse, endpointbuffer, sizeof(endpointbuffer)))

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
        ADVISE(diminuto_ipc4_compare(&(_pp_->ipv4), &(_IPV4_)) == 0); \
        ADVISE(diminuto_ipc6_compare(&(_pp_->ipv6), &(_IPV6_)) == 0); \
        EXPECT(_pp_->tcp == (_TCP_)); \
        EXPECT(_pp_->udp == (_UDP_)); \
        EXPECT(_pp_->local[0] == '\0'); \
        EXPECT(_pp_->type == AF_INET6); \
    } while (0)

#define VALIDATEINET4(_POINTER_, _IPV4_, _IPV6_, _TCP_, _UDP_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(rc == 0); \
        EXPECT(!diminuto_ipc4_is_unspecified(&(_pp_->ipv4))); \
        EXPECT(!diminuto_ipc6_is_unspecified(&(_pp_->ipv6))); \
        ADVISE(diminuto_ipc4_compare(&(_pp_->ipv4), &(_IPV4_)) == 0); \
        ADVISE(diminuto_ipc6_compare(&(_pp_->ipv6), &(_IPV6_)) == 0); \
        EXPECT(_pp_->tcp == (_TCP_)); \
        EXPECT(_pp_->udp == (_UDP_)); \
        EXPECT(_pp_->local[0] == '\0'); \
        EXPECT(_pp_->type == AF_INET); \
    } while (0)

#define VALIDATEINET6(_POINTER_, _IPV4_, _IPV6_, _TCP_, _UDP_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(rc == 0); \
        EXPECT(!diminuto_ipc4_is_unspecified(&(_pp_->ipv4)) == 0); \
        EXPECT(!diminuto_ipc6_is_unspecified(&(_pp_->ipv6)) == 0); \
        ADVISE(diminuto_ipc4_compare(&(_pp_->ipv4), &(_IPV4_)) == 0); \
        ADVISE(diminuto_ipc6_compare(&(_pp_->ipv6), &(_IPV6_)) == 0); \
        EXPECT(_pp_->tcp == (_TCP_)); \
        EXPECT(_pp_->udp == (_UDP_)); \
        EXPECT(_pp_->local[0] == '\0'); \
        EXPECT(_pp_->type == AF_INET6); \
    } while (0)

/*
 * The exact value returned upon success will depend on
 * specifics of the file system on which the test is
 * being run.
 */

#define VERIFYUNIX(_POINTER_, _PATH_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(rc == 0); \
        EXPECT(diminuto_ipc4_compare(&(_pp_->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) == 0); \
        EXPECT(diminuto_ipc6_compare(&(_pp_->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) == 0); \
        EXPECT(_pp_->tcp == 0); \
        EXPECT(_pp_->udp == 0); \
        EXPECT((((_PATH_)[0] == '\0') && (_pp_->local[0] == '\0')) || (_pp_->local[0] == '/')); \
        EXPECT(_pp_->type == AF_UNIX); \
    } while (0)

#define VERIFYFAIL(_POINTER_) \
    do { \
        EXPECT(rc < 0); \
        EXPECT(parse.type == DIMINUTO_IPC_TYPE_UNSPECIFIED); \
    } while (0); \

int main(int argc, char * argv[])
{
    const char fqdn4[] = FQDN4;
    const char fqdn6[] = FQDN46;
    diminuto_ipv4_t unspecified4;
    diminuto_ipv6_t unspecified6;
    diminuto_ipv4_t localhost4;
    diminuto_ipv6_t localhost6;
    diminuto_ipv4_t fqdn44;
    diminuto_ipv6_t fqdn46;
    diminuto_ipv4_t fqdn64;
    diminuto_ipv6_t fqdn66;
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
    diminuto_port_t undefinedtcp;
    diminuto_port_t undefinedudp;
    diminuto_port_t colonporttcp;
    diminuto_port_t colonportudp;
    diminuto_port_t colontcptcp;
    diminuto_port_t colontcpudp;
    diminuto_port_t colonudptcp;
    diminuto_port_t colonudpudp;
    diminuto_port_t coloneithertcp;
    diminuto_port_t coloneitherudp;
    diminuto_port_t colonundefinedtcp;
    diminuto_port_t colonundefinedudp;

    SETLOGMASK();

    if ((argc > 1) && (strcmp(argv[1], "-d") == 0)) {
        extern int diminuto_ipc_endpoint_debug(int);
        (void)diminuto_ipc_endpoint_debug(!0);
    }

    COMMENT("FQDN4=\"%s\"\n", FQDN4);
    COMMENT("FQDN46=\"%s\"\n", FQDN46);
    COMMENT("IPV4=\"%s\"\n", IPV4);
    COMMENT("IPV6=\"%s\"\n", IPV6);
    COMMENT("PORT=\"%s\"\n", PORT);
    COMMENT("COLONPORT=\"%s\"\n", COLONPORT);
    COMMENT("TCP=\"%s\"\n", TCP);
    COMMENT("COLONTCP=\"%s\"\n", COLONTCP);
    COMMENT("UDP=\"%s\"\n", UDP);
    COMMENT("COLONUDP=\"%s\"\n", COLONUDP);
    COMMENT("EITHER=\"%s\"\n", EITHER);
    COMMENT("COLONEITHER=\"%s\"\n", COLONEITHER);
    COMMENT("UNDEFINED=\"%s\"\n", UNDEFINED);
    COMMENT("COLONUNDEFINED=\"%s\"\n", COLONUNDEFINED);
    COMMENT("LOCAL=\"%s\"\n", LOCAL);

    unspecified4 = DIMINUTO_IPC4_UNSPECIFIED;
    unspecified6 = DIMINUTO_IPC6_UNSPECIFIED;
    localhost4 = diminuto_ipc4_address("localhost");
    localhost6 = diminuto_ipc6_address("localhost");
    fqdn44 = diminuto_ipc4_address(fqdn4);
    fqdn46 = diminuto_ipc6_address(fqdn6);
    address4 = diminuto_ipc4_address(IPV4);
    address46 = diminuto_ipc6_address("::ffff:" IPV4);
    address6 = diminuto_ipc6_address(IPV6);
    ephemeral = 0;
    porttcp = diminuto_ipc_port(PORT, "tcp");
    portudp = diminuto_ipc_port(PORT, "udp");
    tcptcp = diminuto_ipc_port(TCP, "tcp");
    tcpudp = diminuto_ipc_port(TCP, "udp");
    udptcp = diminuto_ipc_port(UDP, "tcp");
    udpudp = diminuto_ipc_port(UDP, "udp");
    eithertcp = diminuto_ipc_port(EITHER, "tcp");
    eitherudp = diminuto_ipc_port(EITHER, "udp");
    undefinedtcp = diminuto_ipc_port(UNDEFINED, "tcp");
    undefinedudp = diminuto_ipc_port(UNDEFINED, "udp");
    colonporttcp = diminuto_ipc_port(COLONPORT, "tcp");
    colonportudp = diminuto_ipc_port(COLONPORT, "udp");
    colontcptcp = diminuto_ipc_port(COLONTCP, "tcp");
    colontcpudp = diminuto_ipc_port(COLONTCP, "udp");
    colonudptcp = diminuto_ipc_port(COLONUDP, "tcp");
    colonudpudp = diminuto_ipc_port(COLONUDP, "udp");
    coloneithertcp = diminuto_ipc_port(COLONEITHER, "tcp");
    coloneitherudp = diminuto_ipc_port(COLONEITHER, "udp");
    colonundefinedtcp = diminuto_ipc_port(COLONUNDEFINED, "tcp");
    colonundefinedudp = diminuto_ipc_port(COLONUNDEFINED, "udp");

    {
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        COMMENT("unspecified4=%s\n", diminuto_ipc4_address2string(unspecified4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("unspecified6=%s\n", diminuto_ipc6_address2string(unspecified6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("localhost4=%s\n", diminuto_ipc4_address2string(localhost4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("localhost6=%s\n", diminuto_ipc6_address2string(localhost6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("fqdn44=%s\n", diminuto_ipc4_address2string(fqdn44, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("fqdn46=%s\n", diminuto_ipc6_address2string(fqdn46, ipv6buffer, sizeof(ipv6buffer)));
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
        COMMENT("undefined tcp=%d\n", undefinedtcp);
        COMMENT("undefined udp=%d\n", undefinedudp);
        COMMENT("port colon tcp=%d\n", colonporttcp);
        COMMENT("port colon udp=%d\n", colonportudp);
        COMMENT("http colon tcp=%d\n", colontcptcp);
        COMMENT("http colon udp=%d\n", colontcpudp);
        COMMENT("tftp colon tcp=%d\n", colonudptcp);
        COMMENT("tftp colon udp=%d\n", colonudpudp);
        COMMENT("time colon tcp=%d\n", coloneithertcp);
        COMMENT("time colon udp=%d\n", coloneitherudp);
        COMMENT("undefined colon udp=%d\n", colonundefinedudp);
        COMMENT("undefined colon tcp=%d\n", colonundefinedtcp);
    }

    {
        TEST();
        ASSERT(ephemeral == 0);
        ASSERT(porttcp == 8888);
        ASSERT(portudp == 8888);
        ASSERT(tcptcp == 80);
        ASSERT((tcpudp == 80) || (tcpudp == 0));    /* Depends on distro. */
        ASSERT((udptcp == 69) || (udptcp == 0));    /* Depends on distro. */
        ASSERT(udpudp == 69);
        ASSERT(eithertcp == 37);
        ASSERT(eitherudp == 37);
        ASSERT(undefinedtcp == 0);
        ASSERT(undefinedudp == 0);
        STATUS();
    }

    {
        TEST();
        ASSERT(porttcp == colonporttcp);
        ASSERT(portudp == colonportudp);
        ASSERT(tcptcp == colontcptcp);
        ASSERT(tcpudp == colontcpudp);
        ASSERT(udptcp == colonudptcp);
        ASSERT(udpudp == colonudpudp);
        ASSERT(eithertcp == coloneithertcp);
        ASSERT(eitherudp == coloneitherudp);
        ASSERT(undefinedtcp == colonundefinedtcp);
        ASSERT(undefinedudp == colonundefinedudp);
        STATUS();
    }

    {
        TEST();
        ASSERT(!diminuto_ipc_endpoint_ipv6);
        STATUS();
    }

    /*
     * Below are the sunny day scenarios I expect to succeed. To test
     * arbitary endpoint strings, see the endpoint utility.
     */


    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = PORT, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" PORT, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = TCP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" TCP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = UDP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" UDP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = EITHER, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" EITHER, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" PORT, &parse);
        DISPLAY();
        VERIFYINET4(&parse, localhost4, localhost6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" TCP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, localhost4, localhost6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" UDP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, localhost4, localhost6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" EITHER, &parse);
        DISPLAY();
        VERIFYINET4(&parse, localhost4, localhost6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn44, fqdn46, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4 ":0", &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn44, fqdn46, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4 ":" PORT, &parse);
        VALIDATEINET4(&parse, fqdn44, fqdn46, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4 ":" TCP, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn44, fqdn46, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4 ":" UDP, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn44, fqdn46, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4 ":" EITHER, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn44, fqdn46, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0", &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:0", &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" PORT, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" TCP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" UDP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:" EITHER, &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4, &parse);
        DISPLAY();
        VERIFYINET4(&parse, address4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":0", &parse);
        DISPLAY();
        VERIFYINET4(&parse, address4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" PORT, &parse);
        DISPLAY();
        VERIFYINET4(&parse, address4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" TCP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, address4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" UDP, &parse);
        DISPLAY();
        VERIFYINET4(&parse, address4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" EITHER, &parse);
        DISPLAY();
        VERIFYINET4(&parse, address4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN46, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn64, fqdn66, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN46 ":0", &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn64, fqdn66, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN46 ":" PORT, &parse);
        VALIDATEINET4(&parse, fqdn64, fqdn66, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN46 ":" TCP, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn64, fqdn66, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN46 ":" UDP, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn64, fqdn66, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN46 ":" EITHER, &parse);
        DISPLAY();
        VALIDATEINET4(&parse, fqdn64, fqdn66, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]", &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:0", &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" PORT, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, unspecified6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" TCP, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, unspecified6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" UDP, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, unspecified6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" EITHER, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, unspecified6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]", &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address46, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:0", &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address46, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" PORT, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address46, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" TCP, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address46, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" UDP, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address46, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" EITHER, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address46, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]", &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:0", &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" PORT, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address6, porttcp, portudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" TCP, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address6, tcptcp, tcpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" UDP, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address6, udptcp, udpudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" EITHER, &parse);
        DISPLAY();
        VERIFYINET6(&parse, unspecified4, address6, eithertcp, eitherudp);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0", &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":0", &parse);
        DISPLAY();
        VERIFYINET4(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "", &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "./" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "../" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ".//..///.////" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/run/" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/var/tmp/" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/var/tmp/../run/" LOCAL, &parse);
        DISPLAY();
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = LOCAL, &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/I-hope-this-directory-does-not-exist/" LOCAL, &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/var/tmp/", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "I-hope-this-domain-name-is-unresolvable.com", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "(I-hope-this-does-not-look-like-anything-valid)", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "undefinedthing", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":undefinedthing", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN4 ":undefinedthing", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":undefinedthing", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:undefinedthing", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        PREFACE;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:undefinedthing", &parse);
        DISPLAY();
        VERIFYFAIL(&parse);
        STATUS();
    }

    {
        /* Special cases. */

        diminuto_ipc_endpoint_t endpoint;
        diminuto_endpoint_buffer_t buffer; 
        char * result;

        TEST();

        memset(&endpoint, 0, sizeof(endpoint));
        endpoint.type = DIMINUTO_IPC_TYPE_IPV4;
        result = diminuto_ipc_endpoint2string(&endpoint, buffer, sizeof(buffer));
        ASSERT(result == buffer);
        COMMENT("PORTLESS \"%s\"\n", result);
        EXPECT(strcmp(result, "") == 0);

        memset(&endpoint, 0, sizeof(endpoint));
        endpoint.type = DIMINUTO_IPC_TYPE_IPV4;
        endpoint.tcp = 1234;
        result = diminuto_ipc_endpoint2string(&endpoint, buffer, sizeof(buffer));
        ASSERT(result == buffer);
        COMMENT("TCP \"%s\"\n", result);
        EXPECT(strcmp(result, ":1234") == 0);

        memset(&endpoint, 0, sizeof(endpoint));
        endpoint.type = DIMINUTO_IPC_TYPE_IPV4;
        endpoint.udp = 5678;
        result = diminuto_ipc_endpoint2string(&endpoint, buffer, sizeof(buffer));
        ASSERT(result == buffer);
        COMMENT("UDP \"%s\"\n", result);
        EXPECT(strcmp(result, ":5678") == 0);

        memset(&endpoint, 0, sizeof(endpoint));
        endpoint.type = DIMINUTO_IPC_TYPE_IPV4;
        endpoint.tcp = 1234;
        endpoint.udp = 5678;
        result = diminuto_ipc_endpoint2string(&endpoint, buffer, sizeof(buffer));
        ASSERT(result == buffer);
        COMMENT("TCPUDP \"%s\"\n", result);
        EXPECT(strcmp(result, ":1234:5678") == 0);

        STATUS();
    }

    EXIT();
}
