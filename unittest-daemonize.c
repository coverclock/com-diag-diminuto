/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include "diminuto_daemonize.h"
#include "diminuto_delay.h"
#include "diminuto_lock.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <syslog.h>

static const char * file = "/tmp/unittest-daemonize.pid";

int main(int argc, char ** argv)
{
    int xc = 0;
    int rc;
    pid_t pid1;
    pid_t pid2;
    pid_t pid3;
    pid_t pid4;

    do {

        diminuto_coreable();

        pid1 = getpid();
        if (pid1 < 0) { xc = 100; break; }

        pid2 = diminuto_locked(file);
        if (pid2 > 0) { xc = 101; break; }

        rc = diminuto_lock(file);
        if (rc != 0) { xc = 102; break; }

        pid2 = diminuto_locked(file);
        if (pid2 < 0) { xc = 103; break; }
        if (pid1 != pid2) { xc =104; break; }

        diminuto_log(LOG_NOTICE, "unittest-daemonize: parent %d\n", pid2);

        rc = diminuto_lock(file);
        if (rc == 0) { xc = 105; break; }

        rc = diminuto_unlock(file);
        if (rc != 0) { xc = 106; break; }

        pid2 = diminuto_locked(file);
        if (pid2 > 0) { xc = 107; break; }

        rc = diminuto_unlock(file);
        if (rc == 0) { xc = 108; break; }

        rc = diminuto_daemonize(file);
        if (rc != 0) { xc = 109; break; }

        pid3 = getpid();
        if (pid3 < 0) { xc = 110; break; }

        rc = diminuto_lock(file);
        if (rc == 0) { xc = 111; break; }

        pid4 = diminuto_locked(file);
        if (pid4 < 0) { xc = 112; break; }
        if (pid3 != pid4) { xc =113; break; }

        /*
         * This delay is necessary for the parent to get the SIGUSR1 signal
         * from the daemon child; otherwise the daemon child exits before the
         * parent gets back into the run state and sends a SIGCHLD signal to
         * the parent instead. This wouldn't be necessary in a normal daemon
         * child which keeps running. It also gives the tester a chance to
         * verify the presence and contents of the lock file.
         */

        diminuto_delay(10 * 1000000, 0);

        rc = diminuto_unlock(file);
        if (rc != 0) { xc = 114; break; }

        diminuto_log(LOG_NOTICE, "unittest-daemonize: child %d\n", pid4);

    } while (0);

    return xc;
}
