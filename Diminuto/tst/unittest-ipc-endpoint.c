/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"

#define VERIFY(_IPV4_, _IPV6_, _TCP_, _UDP_) \
	{ \
		EXPECT(diminuto_ipc4_compare(&parse.ipv4, &(_IPV4_)) == 0); \
		EXPECT(diminuto_ipc6_compare(&parse.ipv6, &(_IPV6_)) == 0); \
		EXPECT(parse.tcp == (_TCP_)); \
		EXPECT(parse.udp == (_UDP_)); \
	}

int main(int argc, char * argv[])
{
	diminuto_ipv4_t unspecified4;
	diminuto_ipv6_t unspecified6;
	diminuto_ipv4_t localhost4;
	diminuto_ipv6_t localhost6;
	diminuto_ipv4_t google4;
	diminuto_ipv6_t google6;
	diminuto_ipv4_t address4;
	diminuto_ipv6_t address46;
	diminuto_ipv6_t address6;
	diminuto_port_t ephemeral;
	diminuto_port_t port80;
	diminuto_port_t httptcp;
	diminuto_port_t httpudp;

	unspecified4 = DIMINUTO_IPC4_UNSPECIFIED;
	unspecified6 = DIMINUTO_IPC6_UNSPECIFIED;
	localhost4 = diminuto_ipc4_address("localhost");
	localhost6 = diminuto_ipc6_address("localhost");
	google4 = diminuto_ipc4_address("google.com");
	google6 = diminuto_ipc6_address("google.com");
	address4 = diminuto_ipc4_address("172.217.1.206");
	address46 = diminuto_ipc6_address("::ffff:172.217.1.206");
	address6 = diminuto_ipc6_address("2607:f8b0:400f:805::200e");
	ephemeral = 0;
	port80 = 80;
	httptcp = diminuto_ipc_port("http", "tcp");
	httpudp = diminuto_ipc_port("http", "udp");

    SETLOGMASK();

    /*
     * Below are the sunny day scenarios I expect to succeed. To test
     * arbitary endpoint strings, see the endpoint utility.
     */

    {
    	const char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "google.com", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(google4, google6, ephemeral, ephemeral);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "google.com:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(google4, google6, port80, port80);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "google.com:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(google4, google6, httptcp, httpudp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "172.217.1.206", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(address4, unspecified6, ephemeral, ephemeral);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "172.217.1.206:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(address4, unspecified6, port80, port80);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "172.217.1.206:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(address4, unspecified6, httptcp, httpudp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
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
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "[::ffff:172.217.1.206]", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(unspecified4, address46, ephemeral, ephemeral);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "[::ffff:172.217.1.206]:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(unspecified4, address46, port80, port80);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "[::ffff:172.217.1.206]:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(unspecified4, address46, httptcp, httpudp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "[2607:f8b0:400f:805::200e]", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(unspecified4, address6, ephemeral, ephemeral);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_endpoint(endpoint = "[2607:f8b0:400f:805::200e]:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(unspecified4, address6, port80, port80);
    	STATUS();
    }

    {
    	TEST();
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_endpoint_t parse;
    	int rc;

    	rc = diminuto_ipc_endpoint(endpoint = "[2607:f8b0:400f:805::200e]:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
		VERIFY(unspecified4, address6, httptcp, httpudp);
    	STATUS();
    }


    EXIT();
}
