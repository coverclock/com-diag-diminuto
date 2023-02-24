/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Endpoint portion of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Endpoint portion of the IPC feature.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int diminuto_ipc_endpoint_ipv6 = 0;

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
 * ""
 * "0"
 * "80"
 * ":0"
 * ":80"
 * "http"
 * ":http"
 * "localhost"
 * "localhost:80"
 * "localhost:http"
 * "google.com"
 * "google.com:80"
 * "google.com:http"
 * "0.0.0.0"
 * "0.0.0.0:80"
 * "0.0.0.0:http"
 * "172.217.1.206"
 * "172.217.1.206:80"
 * "172.217.1.206:http"
 * "[::]"
 * "[::]:80"
 * "[::]:http"
 * "[::ffff:172.217.1.206]"
 * "[::ffff:172.217.1.206]:80"
 * "[::ffff:172.217.1.206]:http"
 * "[2607:f8b0:400f:805::200e]"
 * "[2607:f8b0:400f:805::200e]:80"
 * "[2607:f8b0:400f:805::200e]:http"
 * "./path"
 * "/tmp/path"
 * "home/sock/path"
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
    diminuto_path_t buffer = { '\0', };
    bool is_ipv4 = false;
    bool is_ipv6 = false;
    bool is_ipcl = false;
    diminuto_ipv4_buffer_t buffer4 = { '\0', };
    diminuto_ipv6_buffer_t buffer6 = { '\0', };

    endpoint->type = DIMINUTO_IPC_TYPE_UNSPECIFIED;
    endpoint->ipv4 = DIMINUTO_IPC4_UNSPECIFIED;
    memcpy(&(endpoint->ipv6), &DIMINUTO_IPC6_UNSPECIFIED, sizeof(endpoint->ipv6));
    endpoint->tcp = 0;
    endpoint->udp = 0;
    endpoint->local[0] = '\0';

    do {

        (void)strncpy(buffer, string, sizeof(buffer));
        if (buffer[sizeof(buffer) - 1] != '\0') { break; }
        here = &(buffer[0]);

        do {

            if (debug) {
                DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint%p: ch='%.1s' st=%c\n", endpoint,  pc(here), state);
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
                } else if (*here == '/') {
                    mark = here;
                    here += 1;
                    state = S_PATH;
                } else if (*here == '.') {
                    mark = here;
                    here += 1;
                    state = S_PATH;
                } else if (*here == '\0') {
                    mark = here;
                    path = mark;
                    state = S_STOP;
                    rc = 0;
                } else {
                    mark = here;
                    here += 1;
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
                    here += 1;
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
                    is_ipv4 = true;
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
                    is_ipv4 = true;
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
                    here += 1;
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
                    *here = '\0';
                    here += 1;
                    fqdn = mark;
                    state = S_COLON;
                } else if (*here == '/') {
                    here += 1;
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
                    is_ipv6 = true;
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

        DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint@%p: endpoint=\"%s\" name=\"%s\" fqdn=\"%s\" ipv4=\"%s\" ipv6=\"%s\" service=\"%s\" port=\"%s\" path=\"%s\"\n", endpoint, string, ps(name), ps(fqdn), ps(ipv4), ps(ipv6), ps(service), ps(port), ps(path));

        /*
         * If (rc < 0) at this point, our simple little parser
         * found a syntax error, even it recognized it. No
         * point in continuing.
         */

        if (rc < 0) {
            errno = EINVAL;
            diminuto_perror(string);
            return rc;
        }

        /*
         * If the endpoint included a FQDN, we resolve the addresses
         * (there may be both an IPv4 and an IPv6 address) using the
         * usual DNS mechanism. This can take awhile (seconds; the latency
         * is often noticable, particularly on the initial call when the
         * underlying glibc DNS infrastructure initializes), or may
         * fail altogether. For this not to be an error, the FQDN must
         * must resolve to at least one of the IPv4 or IPv6 addresses.
         */

        if (fqdn != (char *)0) {
            endpoint->ipv4 = diminuto_ipc4_address(fqdn);
            endpoint->ipv6 = diminuto_ipc6_address(fqdn);
            if (!diminuto_ipc6_is_unspecified(&(endpoint->ipv6))) {
                is_ipv6 = true;
            } else if (!diminuto_ipc4_is_unspecified(&(endpoint->ipv4))) {
                is_ipv4 = true;
            } else {
                DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint@%p: fqdn=\"%s\" invalid!\n", endpoint, fqdn);
                rc = -2;
            }
        } else if (ipv6 != (char *)0) {
            endpoint->ipv6 = diminuto_ipc6_address(ipv6);
            is_ipv6 = true;
        } else if (ipv4 != (char *)0) {
            endpoint->ipv4 = diminuto_ipc4_address(ipv4);
            is_ipv4 = true;
        } else {
            /* Do nothing. */
        }

        /*
         * Something that looks like it might be a service name could also be
         * a host name that is not a domain name, e.g. "localhost". We first
         * try to resolve such names as host names unless it's really clear
         * that they were meant as service names. It's an error thought to
         * have such a name and not have it to resolve to something.
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
            } else if (!diminuto_ipc6_is_unspecified(&(endpoint->ipv6))) {
                /* Do nothing. */
            } else if (!diminuto_ipc4_is_unspecified(&(endpoint->ipv4))) {
                /* Do nothing. */
            } else {
                endpoint->tcp = diminuto_ipc_port(name, "tcp");
                endpoint->udp = diminuto_ipc_port(name, "udp");
                if (endpoint->tcp != 0) {
                    /* Do nothing. */
                } else if (endpoint->udp != 0) {
                    /* Do nothing. */
                } else {
                    DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint@%p: name=\"%s\" invalid!\n", endpoint, name);
                    rc = -3;
                }
            }
        }

        /*
         * Similarly, we try to resolve the service name, but
         * it is an error for such a name to not resolve.
         * Note that it is perfectly okay for the port number
         * to be zero, indicating an ephemeral port.
         */

        if (service != (char *)0) {
            endpoint->tcp = diminuto_ipc_port(service, "tcp");
            endpoint->udp = diminuto_ipc_port(service, "udp");
            if (endpoint->tcp != 0) {
                /* Do nothing. */
            } else if  (endpoint->udp != 0) {
                /* Do nothing. */
            } else {
                DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint@%p: service=\"%s\" invalid!\n", endpoint, service);
                rc = -4;
            }
        } else if (port != (char *)0) {
            endpoint->tcp = atoi(port);
            endpoint->udp = endpoint->tcp;
        } else {
            /* Do nothing. */
        }

        /*
         * If the endpoint is a pathname for a Local (UNIX domain) socket,
         * then we try to resolve all the soft links in the path to get an
         * absolute path name. All but the final component in the path (the
         * actual file name in the parent directory) must exist. UNIX domain
         * socket names have a much shorter length limitation than file
         * system paths, so the resulting Local address must conform to
         * that limitation. There is no requirement that the final path
         * component exists, just as there is no requirement for IPv4 or IPv6
         * addresses that such a host exists at that address. This path
         * resolution is necessary so that UNIX domain socket names that
         * may have resulted from different soft links can be compared.
         * The empty string ("") is a special case and represents an
         * unnamed Local socket.
         */

        if (path == (char *)0) {
            /* Do nothing. */
        } else if (diminuto_ipcl_is_unnamed(path)) {
            endpoint->local[0] = '\0';
            is_ipcl = true;
        } else if (diminuto_ipcl_path(path, endpoint->local, sizeof(endpoint->local)) != (char *)0) {
            is_ipcl = true;
        } else {
            DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint@%p: path=\"%s\" invalid!\n", endpoint, path);
            rc = -5;
        }

        /*
         * If (rc < 0) at this point, we had a semantic error.
         * No point in continuing.
         */

        if (rc < 0) {
            errno = EINVAL;
            diminuto_perror(string);
            return rc;
        }

        /*
         * If the endpoint included a FQDN, there may be both
         * an IPv4 and an IPv6 address resolved for it; in that case
         * the type is IPV6 (which reflects my own bias), except in
         * those cases where the IPv6 address is merely an IPv4
         * address mapped into IPv6 format, in which case we use
         * IPv4 rather than using the IPv6 stack for IPv4 connections
         * (which is entirely doable). The caller can always check the
         * IPv4 and IPv6 address fields explicitly. If just an IPv4
         * address resolved, the type is IPV4. In either case, there
         * may be a service, and that service could have resolved to be
         * either a TCP or UDP port or both. Ephemeral ports are a
         * special case where everything is zero but the parser succeeded;
         * examples: "[::]", "0.0.0.0", "0".
         */

        if (!diminuto_ipc6_is_unspecified(&(endpoint->ipv6))) {
            if (diminuto_ipc4_is_unspecified(&(endpoint->ipv4))) {
                is_ipv6 = true;
                is_ipv4 = false;
            } else if (diminuto_ipc6_is_v4mapped(&(endpoint->ipv6))) {
                is_ipv6 = false;
                is_ipv4 = true;
            } else {
                is_ipv6 = !!diminuto_ipc_endpoint_ipv6;
                is_ipv4 = !diminuto_ipc_endpoint_ipv6;
            }
        } else if (!diminuto_ipc4_is_unspecified(&(endpoint->ipv4))) {
            is_ipv6 = false;
            is_ipv4 = true;
        } else if (is_ipv4 == is_ipv6) {
            is_ipv6 = !!diminuto_ipc_endpoint_ipv6;
            is_ipv4 = !diminuto_ipc_endpoint_ipv6;
        } else {
            /* Do nothing: no ambiguity. */
        }

        /*
         * We finally decide on what kind of endpoint this is.
         * There is some ambiguity: for example, "google.com" could actually
         * be the name of a UNIX domain socket (which may not yet exist)
         * in the current directory.
         */

        if (is_ipcl) {
            endpoint->type = DIMINUTO_IPC_TYPE_LOCAL;
        } else if (is_ipv6) {
            endpoint->type = DIMINUTO_IPC_TYPE_IPV6;
        } else if (is_ipv4) {
            endpoint->type = DIMINUTO_IPC_TYPE_IPV4;
        } else {
            /* Do nothing. */
        }

        DIMINUTO_LOG_DEBUG("diminuto_ipc_endpoint@%p: endpoint=\"%s\" type=%s ipv4=%s ipv6=%s tcp=%d udp=%d local=\"%s\"\n",
            endpoint,
            string,
            (endpoint->type == DIMINUTO_IPC_TYPE_IPV4) ? "IPv4" :
                (endpoint->type == DIMINUTO_IPC_TYPE_IPV6) ? "IPv6" :
                    (endpoint->type == DIMINUTO_IPC_TYPE_LOCAL) ? "Local" :
                        "Unspecified",
            diminuto_ipc4_address2string(endpoint->ipv4, buffer4, sizeof(buffer4)),
            diminuto_ipc6_address2string(endpoint->ipv6, buffer6, sizeof(buffer6)),
            endpoint->tcp,
            endpoint->udp,
            diminuto_ipcl_path2string(endpoint->local));

    } while (0);

    return 0;
}

const char * diminuto_ipc_endpoint2string(const diminuto_ipc_endpoint_t * endpoint, void * buffer, size_t length)
{
    char * string = (char *)buffer;
    diminuto_port_t port = 0;
    diminuto_ip_buffer_t addressbuffer = { '\0', };
    diminuto_port_buffer_t portbuffer = { '\0', };

    if (length > 0) {

        if (endpoint->tcp != 0) {
            port = endpoint->tcp;
        } else if (endpoint->udp != 0) {
            port = endpoint->udp;
        } else {
            /* Do nothing. */
        }

        string[0] = '\0';
        switch (endpoint->type) {
        case DIMINUTO_IPC_TYPE_LOCAL:
            (void)strncpy(string, endpoint->local, length);
            break;
        case DIMINUTO_IPC_TYPE_IPV6:
            (void)snprintf(string, length, "[%s]%s%s", diminuto_ipc6_address2string(endpoint->ipv6, addressbuffer, sizeof(addressbuffer)), (port != 0) ? ":" : "", (port != 0) ? diminuto_ipc_port2string(port, portbuffer, sizeof(portbuffer)) : "");
            break;
        case DIMINUTO_IPC_TYPE_IPV4:
            (void)snprintf(string, length, "%s%s%s", diminuto_ipc4_address2string(endpoint->ipv4, addressbuffer, sizeof(addressbuffer)), (port != 0) ? ":" : "", (port != 0) ? diminuto_ipc_port2string(port, portbuffer, sizeof(portbuffer)) : "");
            break;
        default:
            break;
        }
        string[length - 1] = '\0';

    }

    return string;
}
