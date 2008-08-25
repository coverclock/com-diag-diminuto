/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include "diminuto_daemonize.h"
#include "diminuto_delay.h"
#include "diminuto_lock.h"
#include "diminuto_log.h"
#include <errno.h>

static const char * file = "/tmp/unittest-daemonize.pid";

int main(int argc, char ** argv)
{
    int xc = 0;
    int rc;
    char dummy;

    do {

        diminuto_coreable();

	errno = 0;
        diminuto_perror("unittest-daemonize: parent");

        rc = diminuto_lock(file);
        if (rc != 0) { xc = 100; break; }

        rc = diminuto_lock(file);
        if (rc == 0) { xc = 101; break; }

        rc = diminuto_unlock(file);
        if (rc != 0) { xc = 102; break; }

        rc = diminuto_unlock(file);
        if (rc == 0) { xc = 103; break; }

        rc = diminuto_daemonize(file);
        if (rc != 0) { xc = 104; break; }

        /*
         * This delay is necessary for the parent to get the SIGUSR1 signal
         * from the daemon child; otherwise the child daemon exits before the
         * parent gets back into the run state and sends a SIGCHLD signal to
         * the parent instead. This wouldn't be necessary in a normal daemon
         * child which keeps running. It also gives the tester a chance to
         * verify the presense of the lock file.
         */

        diminuto_delay(10 * 1000000, 0);

        rc = diminuto_unlock(file);
        if (rc != 0) { xc = 105; break; }

	errno = 0;
        diminuto_perror("unittest-daemonize: child");

    } while (0);

    return xc;
}
