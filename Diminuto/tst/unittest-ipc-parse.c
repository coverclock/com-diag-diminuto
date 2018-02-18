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

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
    	const char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = ":80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = ":http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_parse(endpoint = "localhost", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_parse(endpoint = "localhost:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_parse(endpoint = "localhost:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_parse(endpoint = "google.com", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

        TEST();
    	rc = diminuto_ipc_parse(endpoint = "google.com:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "google.com:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "172.217.1.206", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "172.217.1.206:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "172.217.1.206:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[::]", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[::]:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[::]:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[::ffff:172.217.1.206]", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[::ffff:172.217.1.206]:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[::ffff:172.217.1.206]:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[2607:f8b0:400f:805::200e]", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	TEST();
    	rc = diminuto_ipc_parse(endpoint = "[2607:f8b0:400f:805::200e]:80", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }

    {
    	TEST();
    	char * endpoint;
    	char ipv4buffer[sizeof("255.255.255.255")];
    	char ipv6buffer[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")];
    	diminuto_ipc_parse_t parse;
    	int rc;

    	rc = diminuto_ipc_parse(endpoint = "[2607:f8b0:400f:805::200e]:http", &parse);
    	EXPECT(rc == 0);
		COMMENT("diminuto_ipc_parse: endpoint=\"%s\" ipv4=%s ipv6=%s tcp=%d udp=%d\n", endpoint, diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), parse.tcp, parse.udp);
    	STATUS();
    }


    EXIT();
}
