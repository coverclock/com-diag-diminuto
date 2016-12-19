/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        char ** ifvp;
        char ** ifp;
        diminuto_ipv4_t * v4vp;
        diminuto_ipv4_t * v4p;
        diminuto_ipv6_t * v6vp;
        diminuto_ipv6_t * v6p;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        ASSERT((v4vp = diminuto_ipc4_interface("invalid")) != (diminuto_ipv4_t *)0);
        ASSERT(*v4vp == DIMINUTO_IPC4_UNSPECIFIED);
        free(v4vp);

        ASSERT((v6vp = diminuto_ipc6_interface("invalid")) != (diminuto_ipv6_t *)0);
        ASSERT(memcmp(v6vp, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(*v6vp)) == 0);
        free(v6vp);

        ASSERT((ifvp = diminuto_ipc_interfaces()) != (char **)0);

        for (ifp = ifvp; *ifp != (char *)0; ++ifp) {


            ASSERT((v4vp = diminuto_ipc4_interface(*ifp)) != (diminuto_ipv4_t *)0);
            ASSERT((v6vp = diminuto_ipc6_interface(*ifp)) != (diminuto_ipv6_t *)0);

            if (*v4vp != DIMINUTO_IPC4_UNSPECIFIED) {
                /* Do nothing. */
            } else if (memcmp(v6vp, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(*v6vp)) != 0) {
                /* Do nothing. */
            } else {
                DIMINUTO_LOG_DEBUG("%s %s\n", DIMINUTO_LOG_HERE, *ifp);
                continue;
            }

            for (v4p = v4vp; *v4p != DIMINUTO_IPC4_UNSPECIFIED; ++v4p) {
                DIMINUTO_LOG_DEBUG("%s %s %s\n", DIMINUTO_LOG_HERE, *ifp, diminuto_ipc4_dotnotation(*v4p, buffer, sizeof(buffer)));
            }

            for (v6p = v6vp; memcmp(v6p, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(*v6p)) != 0; ++v6p) {
                DIMINUTO_LOG_DEBUG("%s %s %s\n", DIMINUTO_LOG_HERE, *ifp, diminuto_ipc6_colonnotation(*v6p, buffer, sizeof(buffer)));
            }

            free(v4vp);
            free(v6vp);
        }

        free(ifvp);

        STATUS();
    }

    EXIT();
}

