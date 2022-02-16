/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Pipe feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Pipe feature.
 *
 * EXAMPLE
 *
 * unittest-pipe | cat > /dev/null &
 *
 * pkill cat
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_pipe.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int diminuto_pipe_debug;

static const char BUFFER[] = "unittest-pipe | cat > /dev/null & sleep 1 & pkill -x -n cat\n";

int main(int argc, char ** argv)
{
    int rc = 0;

    SETLOGMASK();

    TEST();

    diminuto_pipe_debug = !0;

    rc = diminuto_pipe_install(!0);
    ASSERT(rc == 0);

    rc = diminuto_pipe_check();
    ASSERT(rc == 0);

    while (!0) {

        (void)write(STDOUT_FILENO, BUFFER, sizeof(BUFFER));

        rc = diminuto_pipe_check();
        ASSERT(rc >= 0);
        if (rc > 0) { break; }

    }

    rc = diminuto_pipe_check();
    ASSERT(rc == 0);

    STATUS();

    EXIT();
}
