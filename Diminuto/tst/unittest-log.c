/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * It's useful to run this as
 *
 *	setsid unittest-log
 *
 * or
 *
 *	( ( unittest-log & ) & ) &
 *
 * which exercises the code that detects whether or not it's a deamon
 * by its session identifier or who its parent process is.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "unittest-log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    {
        TEST();

        diminuto_log_mask = 0;
        ASSERT(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "~0", !0) == 0);
        diminuto_log_setmask();
        ASSERT(diminuto_log_mask == DIMINUTO_LOG_MASK_ALL);
        ASSERT(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0", !0) == 0);
        diminuto_log_setmask();
        ASSERT(diminuto_log_mask == 0);
        ASSERT(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0xff", !0) == 0);
        diminuto_log_setmask();
        ASSERT(diminuto_log_mask == 255);
        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

        STATUS();
    }

    {
        char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
        int ii;

        TEST();

        diminuto_log_log(DIMINUTO_LOG_PRIORITY_DEFAULT, DIMINUTO_LOG_HERE "HERE");
        for (ii = 0; ii < sizeof(buffer) - 1; ++ii) {
            buffer[ii] = (ii % ('~' - ' ' + 1)) + ' ';
        }
        buffer[sizeof(buffer) - 1] = '\0';
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_DEFAULT, "%s%s%s", DIMINUTO_LOG_HERE, "THERE", buffer);

        STATUS();

    }

    {
        FILE * fp;

        TEST();

        fp = diminuto_log_stream();
        ASSERT(fp != (FILE *)0);
        ASSERT(fp == diminuto_log_file);
        ASSERT((diminuto_log_descriptor != STDOUT_FILENO) || (fp == stdout));
        ASSERT((diminuto_log_descriptor != STDERR_FILENO) || (fp == stderr));

        STATUS();
    }

    if (argc < 2) {
        /* Do nothing. */
    } else if (strcmp(argv[1], "daemon") == 0) {
        int rc;
        diminuto_log_emit("DAEMONIZING\n");
        if ((rc = diminuto_daemon(argv[0])) == 0) {
            diminuto_log_emit("DAEMONIZED\n");
        } else {
            diminuto_perror("diminuto_daemon");
            exit(1);
        }
    } else if (strcmp(argv[1], "automatic") == 0) {
        diminuto_log_emit("AUTOMATIC\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_AUTOMATIC;
    } else if (strcmp(argv[1], "stderr") == 0) {
        diminuto_log_emit("STDERR\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_STDERR;
    } else if (strcmp(argv[1], "syslog") == 0) {
        diminuto_log_emit("SYSLOG\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_SYSLOG;
    } else {
        fprintf(stderr, "usage: %s [ automatic | daemon | stderr | syslog ]\n", argv[0]);
        exit(1);
    }

    {

        TEST();

        diminuto_log_emit("BEGIN\n");
        yes();
        no();
        maybe();
        diminuto_log_emit("PUBLIC DEFAULT\n");
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC ALL\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC EMERGENCY\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC ALERT\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC CRITICAL\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC ERROR\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC WARNING\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC NOTICE\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC INFORMATION\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC DEBUG\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
        all();
        none();
        mine();
        diminuto_log_emit("PUBLIC NONE\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE DEFAULT\n");
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE ALL\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE NONE\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE EMERGENCY\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE ALERT\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE CRITICAL\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE ERROR\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE WARNING\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE NOTICE\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE INFORMATION\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE DEBUG\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
        all();
        none();
        mine();
        diminuto_log_emit("PRIVATE NONE\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
        all();
        none();
        mine();
        diminuto_log_emit("END\n");

        STATUS();
    }

    {
        TEST();
        const static int ERRNO[] = { E2BIG, EACCES, EADDRINUSE, EAGAIN, EBADF, EBUSY, ECHILD, ECONNREFUSED, EEXIST, EINVAL, };
        char buffer[sizeof("ERRNO[XXXXXXXXXX]")];
        int ii;
        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d]", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_perror(buffer);
        }
        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d]", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_serror(buffer);
        }
        STATUS();
    }

    EXIT();
}
