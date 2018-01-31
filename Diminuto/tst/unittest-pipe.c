
/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * E.G.
 *
 * unittest-pipe | cat > /dev/null &
 * pkill cat
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_pipe.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int diminuto_pipe_debug;

int main(int argc, char ** argv)
{
    int fd = 0;
    int rc = 0;
    uint8_t buffer[512];

    SETLOGMASK();

    TEST();

    diminuto_pipe_debug = !0;

    rc = diminuto_pipe_install(!0);
    ASSERT(rc == 0);

    rc = diminuto_pipe_check();
    ASSERT(rc == 0);

    fd = open("/dev/urandom", O_RDONLY);
    ASSERT(fd >= 0);

    while (!0) {

        rc = read(fd, buffer, sizeof(buffer));
        if (rc <= 0) { break; }

        (void)write(STDOUT_FILENO, buffer, rc);

        rc = diminuto_pipe_check();
        ASSERT(rc >= 0);
        if (rc > 0) { break; }

	}

    rc = close(fd);
    ASSERT(rc >= 0);

    rc = diminuto_pipe_check();
    ASSERT(rc == 0);

    EXIT();
}
