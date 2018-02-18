//* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * Ported from the Desperado::Service class.
 */

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_log.h"

/*
 * 80
 * :80
 * :http
 * localhost
 * localhost:80
 * localhost:http
 * google.com
 * google.com:80
 * google.com:http
 * 172.217.1.206
 * 172.217.1.206:80
 * 172.217.1.206:http
 * [::]
 * [::]:80
 * [::]:http
 * [::ffff:172.217.1.206]
 * [::ffff:172.217.1.206]:80
 * [::ffff:172.217.1.206]:http
 * [2607:f8b0:400f:805::200e]
 * [2607:f8b0:400f:805::200e]:80
 * [2607:f8b0:400f:805::200e]:http
 */

static inline const char * pc(const char * str)
{
	return (*str == '\0') ? "" : (*str == '\t') ? " " : str;
}

static inline const char * ps(const char * str)
{
	return (str == (const char *)0) ? "" : str;
}

typedef enum State {
	S_START		= '<',
	S_DIGIT		= 'N',
	S_PERIOD	= '.',
	S_LETTER	= 'A',
	S_COLON		= ':',
	S_IPV6		= '6',
	S_IPV4		= '4',
	S_PORT		= 'P',
	S_HOST		= 'H',
	S_SERVICE	= 'S',
	S_NEXT		= '-',
	S_STOP		= '>',
} state_t;

int diminuto_ipc_parse(const char * endpoint, diminuto_ipc_parse_t * parse)
{
	int rc = -1;
	state_t state = S_START;
	char * buffer = (char *)0;
	char * here = (char *)0;
	char * mark = (char *)0;
	char * host = (char *)0;
	char * ipv4 = (char *)0;
	char * ipv6 = (char *)0;
	char * port = (char *)0;
	char * service = (char *)0;

	parse->ipv4 = DIMINUTO_IPC4_UNSPECIFIED;
	memcpy(&(parse->ipv6), &DIMINUTO_IPC6_UNSPECIFIED, sizeof(parse->ipv6));
	parse->tcp = 0;
	parse->udp = 0;

	do {

		buffer = strdup(endpoint);
		if (buffer == (char *)0) { break; }
		here = buffer;

		do {

			DIMINUTO_LOG_DEBUG("diminuto_ipc_parse: ch='%.1s' st=%c\n", pc(here), state);

			switch (state) {

			case S_START:
				if ((*here == ' ') || (*here == '\t')) {
					here += 1;
				} else if (*here == ':') {
					here += 1;
					state = S_COLON;
				} else if (*here == '[') {
					here += 1;
					mark = here;
					state = S_IPV6;
				} else if (('0' <= *here) && (*here <= '9')) {
					mark = here;
					here += 1;
					state = S_DIGIT;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					mark = here;
					here += 1;
					state = S_LETTER;
				} else {
					state = S_STOP;
				}
				break;

			case S_DIGIT:
				if (*here == '.') {
					here += 1;
					state = S_PERIOD;
				} else if (*here == '-') {
					here += 1;
					state = S_LETTER;
				} else if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					here += 1;
					state = S_LETTER;
				} else if (*here == '\0') {
					port = mark;
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_PERIOD:
				if (*here == '.') {
					here += 1;
				} else if (*here == '-') {
					here += 1;
					state = S_HOST;
				} else if (*here == ':') {
					ipv4 = mark;
					*here = '\0';
					here += 1;
					state = S_COLON;
				} else if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					here += 1;
					state = S_LETTER;
				} else if (*here == '\0') {
					ipv4 = mark;
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_LETTER:
				if (*here == '.') {
					here += 1;
					state = S_HOST;
				} else if (*here == '-') {
					here += 1;
					state = S_HOST;
				} else if (*here == ':') {
					host = mark;
					*here = '\0';
					here += 1;
					state = S_COLON;
				} else if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					here += 1;
				} else if (*here == '\0') {
					host = mark;
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_HOST:
				if (*here == '-') {
					here += 1;
				} else if (*here == '.') {
					here += 1;
				} else if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					here += 1;
				} else if (*here == ':') {
					*(here++) = '\0';
					host = mark;
					state = S_COLON;
				} else if (*here == '\0') {
					host = mark;
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_IPV6:
				if (*here == ']') {
					*(here++) = '\0';
					ipv6 = mark;
					state = S_NEXT;
				} else if (*here == ':') {
					here += 1;
				} else if (*here == '.') {
					here += 1;
				} else if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if ((('a' <= *here) && (*here <= 'f')) || (('A' <= *here) && (*here <= 'F'))) {
					here += 1;
				} else {
					state = S_STOP;
				}
				break;

			case S_NEXT:
				if (*here == ':') {
					here += 1;
					state = S_COLON;
				} else if (*here == '\0') {
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_COLON:
				if (('0' <= *here) && (*here <= '9')) {
					mark = here;
					here += 1;
					state = S_PORT;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					mark = here;
					here += 1;
					state = S_SERVICE;
				} else if (*here == '\0') {
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_PORT:
				if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if (*here == '\0') {
					port = mark;
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			case S_SERVICE:
				if (('0' <= *here) && (*here <= '9')) {
					here += 1;
				} else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
					here += 1;
				} else if (*here == '\0') {
					service = mark;
					state = S_STOP;
					rc = 0;
				} else {
					state = S_STOP;
				}
				break;

			default:
				state = S_STOP;
				break;

			}

		} while (state != S_STOP);

		DIMINUTO_LOG_DEBUG("diminuto_ipc_parse: ch='%.1s' st=%c\n", pc(here), state);

		if (ipv4 != (char *)0) {
			parse->ipv4 = diminuto_ipc4_address(ipv4);
		} else if (ipv6 != (char *)0) {
			parse->ipv6 = diminuto_ipc6_address(ipv6);
		} else if (host != (char *)0) {
			parse->ipv4 = diminuto_ipc4_address(host);
			parse->ipv6 = diminuto_ipc6_address(host);
		} else {
			/* Do nothing. */
		}

		if (service != (char *)0) {
			parse->tcp = diminuto_ipc_port(service, "tcp");
			parse->udp = diminuto_ipc_port(service, "udp");
		} else if (port != (char *)0) {
			parse->tcp = atoi(port);
			parse->udp = parse->tcp;
		} else {
			/* Do nothing. */
		}

		DIMINUTO_LOG_INFORMATION("diminuto_ipc_parse: endpoint=\"%s\" host=\"%s\" ipv4=\"%s\" ipv6=\"%s\" service=\"%s\" port=\"%s\" rc=%d IPV4=%s IPv6=%s TCP=%d UDP=%d\n", endpoint, ps(host), ps(ipv4), ps(ipv6), ps(service), ps(port), rc);

	} while (0);

	if (buffer != (char *)0) {
		free(buffer);
	}

	return rc;
}
