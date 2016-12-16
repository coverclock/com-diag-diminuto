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

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        const char ** ifvp;
        const char ** ifp;

        TEST();

        ASSERT((ifvp = diminuto_ipc_interfaces()) != (const char **)0);

        for (ifp = ifvp; *ifp != (const char *)0; ++ifp) {
            DIMINUTO_LOG_DEBUG("%s \"%s\"\n", DIMINUTO_LOG_HERE, *ifp);
        }

        free(ifvp);

        STATUS();
    }

    EXIT();
}

