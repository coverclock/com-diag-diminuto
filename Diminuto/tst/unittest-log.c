/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Log feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Log feature. Because the Unit Test framework
 * uses the Log feature, this unit test is not built on top of the Unit Test
 * framework.
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

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "unittest-log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

int main(int argc, char ** argv)
{
    {
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEFAULT;
    }

    {

        diminuto_log_mask = 0;
        assert(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "~0", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == DIMINUTO_LOG_MASK_ALL);
        assert(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == 0);
        assert(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0xff", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == 255);
        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

    }

    {
        char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
        int ii;

        assert(diminuto_log_priority == DIMINUTO_LOG_PRIORITY_DEFAULT);

        diminuto_log_log(diminuto_log_priority, DIMINUTO_LOG_HERE "HERE");
        for (ii = 0; ii < sizeof(buffer) - 1; ++ii) {
            buffer[ii] = (ii % ('~' - ' ' + 1)) + ' ';
        }
        buffer[sizeof(buffer) - 1] = '\0';
        diminuto_log_log(diminuto_log_priority, "%s%s%s", DIMINUTO_LOG_HERE, "THERE", buffer);

    }

    {
        FILE * fp;
        int fd;

        assert(diminuto_log_descriptor == STDERR_FILENO);
        assert(diminuto_log_file == (FILE *)0);

        fp = diminuto_log_stream();
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp == stderr);
        assert(fileno(fp) == fileno(stderr));

        assert(diminuto_log_descriptor == STDERR_FILENO);
        assert(diminuto_log_file == stderr);
        assert(fileno(diminuto_log_file) == fileno(stderr));
        assert(fileno(diminuto_log_file) == STDERR_FILENO);

        diminuto_log_descriptor = STDOUT_FILENO;
        fp = diminuto_log_stream();
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp == stdout);
        assert(fileno(fp) == fileno(stdout));

        assert(diminuto_log_descriptor == STDOUT_FILENO);
        assert(diminuto_log_file == stdout);
        assert(fileno(diminuto_log_file) == fileno(stdout));
        assert(fileno(diminuto_log_file) == STDOUT_FILENO);

        fd = dup(STDOUT_FILENO);
        diminuto_log_descriptor = fd;
        fp = diminuto_log_stream();
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp != stdout);

        assert(diminuto_log_descriptor == fd);
        assert(diminuto_log_file != stdout);
        assert(fileno(diminuto_log_file) != fileno(stdout));
        assert(fileno(diminuto_log_file) != STDOUT_FILENO);

        diminuto_log_descriptor = STDERR_FILENO;
        diminuto_log_file = (FILE *)0;

        fp = diminuto_log_stream();
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp == stderr);
        assert(fileno(fp) == fileno(stderr));

        assert(diminuto_log_descriptor == STDERR_FILENO);
        assert(diminuto_log_file == stderr);
        assert(fileno(diminuto_log_file) == fileno(stderr));
        assert(fileno(diminuto_log_file) == STDERR_FILENO);

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

        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;
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
        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;
        diminuto_log_emit("END\n");

    }

    {
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

    }

    {
        char hostname[DIMINUTO_LOG_HOSTNAME_MAXIMUM];

        assert(sizeof(hostname) >= sizeof("localhost"));

    }

    fprintf(stderr, "%s[%d]: SUCCESS.\n", __FILE__, __LINE__);

    exit(0);
}
