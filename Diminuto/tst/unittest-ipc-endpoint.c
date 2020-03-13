/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"

#define FQDN "diag.com"
#define IPV4 "205.178.189.131"
#define IPV6 "2607:f8b0:400f:805::200e"

#define VERIFY(_IPV4_, _IPV6_, _TCP_, _UDP_) \
    { \
        EXPECT(diminuto_ipc4_compare(&parse.ipv4, &(_IPV4_)) == 0); \
        EXPECT(diminuto_ipc6_compare(&parse.ipv6, &(_IPV6_)) == 0); \
        EXPECT(parse.tcp == (_TCP_)); \
        EXPECT(parse.udp == (_UDP_)); \
    }

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
    diminuto_port_t port80;
    diminuto_port_t httptcp;
    diminuto_port_t httpudp;

    SETLOGMASK();

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
    port80 = 80;
    httptcp = diminuto_ipc_port("http", "tcp");
    httpudp = diminuto_ipc_port("http", "udp");

    {
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        COMMENT("FQDN=\"%s\"\n", FQDN);
        COMMENT("unspecified4=%s\n", diminuto_ipc4_address2string(unspecified4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("unspecified6=%s\n", diminuto_ipc6_address2string(unspecified6, ipv4buffer, sizeof(ipv6buffer)));
        COMMENT("localhost4=%s\n", diminuto_ipc4_address2string(localhost4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("localhost6=%s\n", diminuto_ipc6_address2string(localhost6, ipv4buffer, sizeof(ipv6buffer)));
        COMMENT("fqdn4=%s\n", diminuto_ipc4_address2string(fqdn4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("fqdn6=%s\n", diminuto_ipc6_address2string(fqdn6, ipv4buffer, sizeof(ipv6buffer)));
        COMMENT("address4=%s\n", diminuto_ipc4_address2string(address4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("address46=%s\n", diminuto_ipc6_address2string(address46, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("address6=%s\n", diminuto_ipc6_address2string(address6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("ephemeral=%d\n", ephemeral);
        COMMENT("port80=%d\n", port80);
        COMMENT("httptcp=%d\n", httptcp);
        COMMENT("httpudp=%d\n", httpudp);
    }

    /*
     * Below are the sunny day scenarios I expect to succeed. To test
     * arbitary endpoint strings, see the endpoint utility.
     */

    {
        const char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, port80, port80);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, port80, port80);

        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(localhost4, localhost6, ephemeral, ephemeral);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(localhost4, localhost6, port80, port80);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(localhost4, localhost6, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN, &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(fqdn4, fqdn6, ephemeral, ephemeral);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(fqdn4, fqdn6, port80, port80);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(fqdn4, fqdn6, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4, &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(address4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(address4, unspecified6, port80, port80);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(address4, unspecified6, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, port80, port80);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, unspecified6, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, address46, ephemeral, ephemeral);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 ":80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, address46, port80, port80);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 ":http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, address46, httptcp, httpudp);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, address6, ephemeral, ephemeral);
        STATUS();
    }

    {
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:80", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, address6, port80, port80);
        STATUS();
    }

    {
        TEST();
        char * endpoint;
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        diminuto_ipc_endpoint_t parse;
        int rc;

        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:http", &parse);
        EXPECT(rc == 0);
        COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
        VERIFY(unspecified4, address6, httptcp, httpudp);
        STATUS();
    }


    EXIT();
}
