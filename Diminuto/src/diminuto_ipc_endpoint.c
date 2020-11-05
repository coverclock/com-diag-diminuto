/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Endpoint portion of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Endpoint portion of the IPC feature.
 */

#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>

static int debug = 0;

int diminuto_ipc_endpoint_debug(int now)
{
    int was;

    was = debug;
    debug = now;

    return was;
}

static inline const char * pc(const char * str)
{
    return (*str == '\0') ? "" : (*str == '\t') ? " " : str;
}

static inline const char * ps(const char * str)
{
    return (str == (const char *)0) ? "" : str;
}

/*
 * 80
 * :80
 * http
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
 * ./path
 * /tmp/path
 * home/sock/path
 */

typedef enum State {
    S_START		= '<',
    S_DIGIT		= 'N',
    S_PERIOD	= '.',
    S_LETTER	= 'A',
    S_COLON		= ':',
    S_IPV6		= '6',
    S_IPV4		= '4',
    S_PORT		= 'P',
    S_FQDN		= 'D',
    S_SERVICE	= 'S',
    S_HYPHEN    = '-',
    S_NEXT		= 'X',
    S_PATH      = 'U',
    S_STOP		= '>',
} state_t;

int diminuto_ipc_endpoint(const char * string, diminuto_ipc_endpoint_t * endpoint)
{
    int rc = -1;
    state_t state = S_START;
    char * here = (char *)0;
    char * mark = (char *)0;
    char * name = (char *)0;
    char * fqdn = (char *)0;
    char * ipv4 = (char *)0;
    char * ipv6 = (char *)0;
    char * port = (char *)0;
    char * service = (char *)0;
    char * path = (char *)0;
    char buffer[PATH_MAX] = { '\0', };

    endpoint->type = AF_UNSPEC;
    endpoint->ipv4 = DIMINUTO_IPC4_UNSPECIFIED;
    memcpy(&(endpoint->ipv6), &DIMINUTO_IPC6_UNSPECIFIED, sizeof(endpoint->ipv6));
    endpoint->tcp = 0;
    endpoint->udp = 0;
    endpoint->path = (const char *)0;

    do {

        (void)strncpy(buffer, string, sizeof(buffer));
        if (buffer[sizeof(buffer) - 1] != '\0') { break; }
        here = &(buffer[0]);

        do {

            if (debug) {
                DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint: ch='%.1s' st=%c\n", pc(here), state);
            }

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
                } else if (*here == '\0') {
                    mark = here;
                    state = S_STOP;
                    rc = 0;
                } else {
                    mark = here;
                    here += 1;
                    state = S_PATH;
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
                } else if (*here == '/') {
                    state = S_PATH;
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
                    state = S_FQDN;
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
                    state = S_FQDN;
                } else if (*here == '-') {
                    here += 1;
                    state = S_FQDN;
                } else if (*here == ':') {
                    fqdn = mark;
                    *here = '\0';
                    here += 1;
                    state = S_COLON;
                } else if (('0' <= *here) && (*here <= '9')) {
                    here += 1;
                } else if ((('a' <= *here) && (*here <= 'z')) || (('A' <= *here) && (*here <= 'Z'))) {
                    here += 1;
                } else if (*here == '/') {
                    state = S_PATH;
                } else if (*here == '\0') {
                    name = mark;
                    state = S_STOP;
                    rc = 0;
                } else {
                    state = S_STOP;
                }
                break;

            case S_FQDN:
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
                    fqdn = mark;
                    state = S_COLON;
                } else if (*here == '/') {
                    state = S_PATH;
                } else if (*here == '\0') {
                    fqdn = mark;
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

            case S_PATH:
                if (*here == '\0') {
                    path = mark;
                    state = S_STOP;
                    rc = 0;
                } else {
                    here += 1;
                }
                break;

            default:
                state = S_STOP;
                break;

            }

        } while (state != S_STOP);

        if (debug) {
            DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint: ch='%.1s' st=%c\n", pc(here), state);
        }

        if (debug) {
            DIMINUTO_LOG_INFORMATION("diminuto_ipc_endpoint: endpoint=\"%s\" name=\"%s\" fqdn=\"%s\" ipv4=\"%s\" ipv6=\"%s\" service=\"%s\" port=\"%s\" path=\"%s\" rc=%d\n", string, ps(name), ps(fqdn), ps(ipv4), ps(ipv6), ps(service), ps(port), ps(path), rc);
        }

        /*
         * If the endpoint included a FQDN, we resolve the addresses
         * (there may be both an IPv4 and an IPv6 address) using the
         * usual DNS mechanism. This can take awhile (seconds; the latency
         * is often noticable, particularly on the initial call when the
         * underlying glibc DNS infrastructure initializes), or may
         * fail altogether.
         */

        if (fqdn != (char *)0) {
            endpoint->ipv4 = diminuto_ipc4_address(fqdn);
            endpoint->ipv6 = diminuto_ipc6_address(fqdn);
        } else if (ipv4 != (char *)0) {
            endpoint->ipv4 = diminuto_ipc4_address(ipv4);
        } else if (ipv6 != (char *)0) {
            endpoint->ipv6 = diminuto_ipc6_address(ipv6);
        } else {
            /* Do nothing. */
        }

        /*
         * Something that looks like it might be a service name could also be
         * a host name that is not a domain name, e.g. "localhost". We first
         * try to resolve such names as host names unless it's really clear
         * that they were meant as service names.
         */

        if (fqdn != (char *)0) {
            /* Do nothing. */
        } else if (ipv4 != (char *)0) {
            /* Do nothing. */
        } else if (ipv6 != (char *)0) {
            /* Do nothing. */
        } else if (name == (char *)0) {
            /* Do nothing. */
        } else {
            endpoint->ipv4 = diminuto_ipc4_address(name);
            endpoint->ipv6 = diminuto_ipc6_address(name);
            if (service != (char *)0) {
                /* Do nothing. */
            } else if (port != (char *)0) {
                /* Do nothing. */
            } else if (diminuto_ipc6_compare(&(endpoint->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) != 0) {
                /* Do nothing. */
            } else if (diminuto_ipc4_compare(&(endpoint->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) != 0) {
                /* Do nothing. */
            } else {
                endpoint->tcp = diminuto_ipc_port(name, "tcp");
                endpoint->udp = diminuto_ipc_port(name, "udp");
            }
        }

        if (service != (char *)0) {
            endpoint->tcp = diminuto_ipc_port(service, "tcp");
            endpoint->udp = diminuto_ipc_port(service, "udp");
        } else if (port != (char *)0) {
            endpoint->tcp = atoi(port);
            endpoint->udp = endpoint->tcp;
        } else {
            /* Do nothing. */
        }

        /*
         * If the endpoint is a pathname for a UNIX domain socket, then we
         * must pass back a pointer into the original string that the caller
         * passed into us, not into the temporary buffer that we allocated
         * on the stack for parsing. So we compute the offset from the
         * beginning of our buffer and add it to the pointer to the string.
         * There is no requirement that a UNIX domain socket actually exists
         * with that path, just as there is no requirement for IPv4 or IPv6
         * addresses that such a host exists at that address. (There is a
         * requirement however that any FQDN resolves to an IPv4 or IPv6
         * address, although that address may not be reachable.) Since
         * file system paths can be just about anything, if an FQDN did not
         * did not resolve, we assume it was a UNIX domain socket path name.
         * This allows, unfortunately, for a great deal of ambiguity. For
         * example, a port number or service name could also be a path name.
         */

        if (path != (char *)0) {
            endpoint->path = string + (path - buffer);
        } else if (diminuto_ipc6_compare(&(endpoint->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) != 0) {
            /* Do nothing. */
        } else if (diminuto_ipc4_compare(&(endpoint->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) != 0) {
            /* Do nothing. */
        } else if (endpoint->tcp != 0) {
            /* Do nothing. */
        } else if (endpoint->udp != 0) {
            /* Do nothing. */
        } else if (fqdn != (char *)0) {
            endpoint->path = string + (fqdn - buffer);
        } else {
            /* Do nothing. */
        }

        /*
         * Now we finally figure out what kind of IPC connection we
         * have. If the endpoint included a FQDN, there may be both
         * an IPv4 and an IPv6 address resolved for it; in that case
         * the type is AF_INET6 (which reflects my own bias). The
         * caller can always check the IPv4 address field explicitly.
         * If just an IPv4 address resolved, the type is AF_INET4.
         * In either case, there may be a port, and that port could
         * have resolved to be either TCP or UDP or both. Finally,
         * ephemeral ports are a special case where everything is zero
         * but the parser succeeded; examples: "[::]", "0.0.0.0", "0".
         */

        if (endpoint->path != (char *)0) {
            endpoint->type = AF_UNIX;
        } else if (diminuto_ipc6_compare(&(endpoint->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) != 0) {
            endpoint->type = AF_INET6;
        } else if (diminuto_ipc4_compare(&(endpoint->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) != 0) {
            endpoint->type = AF_INET;
        } else if (endpoint->tcp != 0) {
            endpoint->type = AF_INET;
        } else if (endpoint->udp != 0) {
            endpoint->type = AF_INET;
        } else if (fqdn != (char *)0) {
            /* Do nothing. */
        } else if (service != (char *)0) {
            /* Do nothing. */
        } else if (rc < 0) {
            /* Do nothing. */
        } else {
            endpoint->type = AF_INET;
        }

    } while (0);

    /*
     * We return success in all but the most difficult of circumstances.
     */

    return rc;
}
