/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Display the result of parsing the provided endpoint strings.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * endpoint [ -d | -D | ENDPOINT ] ...
 *
 * EXAMPLES
 *
 * endpoint localhost:80 google.com:http
 *
 * ABSTRACT
 *
 * Display the result of parsing the provided endpoint strings.
 */

#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int xc = 0;
    const char * program;
    int ii;
    diminuto_ipc_endpoint_t endpoint;
    diminuto_ipv4_buffer_t ipv4buffer;
    diminuto_ipv6_buffer_t ipv6buffer;
    char buffer[(sizeof(int) * 3)];
    extern int diminuto_ipc_endpoint_debug(int);

    diminuto_log_setmask();

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    for (ii = 1; ii < argc; ++ii) {

        if (strcmp(argv[ii], "-?") == 0) {
            fprintf(stderr, "usage: %s [ -? | -d | -D | STRING ] ...\n", program);
        } else if (strcmp(argv[ii], "-d") == 0) {
            (void)diminuto_ipc_endpoint_debug(!0);
        } else if (strcmp(argv[ii], "-D") == 0) {
            (void)diminuto_ipc_endpoint_debug(0);
        } else if (diminuto_ipc_endpoint(argv[ii], &endpoint) >= 0) {
            snprintf(buffer, sizeof(buffer), "%d", endpoint.type);
            buffer[sizeof(buffer) - 1] = '\0';
            printf("endpoint \"%s\" type %s ipv4 %s ipv6 %s tcp %d udp %d path \"%s\"\n",
                argv[ii],
                (endpoint.type == AF_INET) ? "AF_INET" : (endpoint.type == AF_UNIX) ? "AF_UNIX" : buffer,
                diminuto_ipc4_address2string(endpoint.ipv4, ipv4buffer, sizeof(ipv4buffer)),
                diminuto_ipc6_address2string(endpoint.ipv6, ipv6buffer, sizeof(ipv6buffer)),
                endpoint.tcp, endpoint.udp,
                (endpoint.path == (const char *)0) ? "" : endpoint.path);
        } else {
            printf("endpoint %s\n", argv[ii]);
        }

    }

    exit(xc);
}
