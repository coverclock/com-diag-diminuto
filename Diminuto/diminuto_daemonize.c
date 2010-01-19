/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * @see http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize
 */

#include "diminuto_daemonize.h"
#include "diminuto_lock.h"
#include "diminuto_delay.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

static void diminuto_handler(int signum)
{
    exit(signum == SIGUSR1 ? 0 : signum + 1);
}

int diminuto_daemonize(const char * file)
{
    int result = -1;
    uid_t uid;
    uid_t euid;
    pid_t ppid;
    pid_t pid;
    pid_t sid;
    int rc;
    FILE * fp;
    int fd;

    do {

        /*
         *  PARENT
         */

        /* Make sure we are not already a daemon. */

        if ((ppid = getppid())  < 0) {
            result = -2;
            diminuto_perror("diminuto_daemonize: getppid");
            break;
        } else if (ppid == 1) {
            result = 0;
            break;
        }

        /* Establish a way for the daemon child to notify us. */

        if (signal(SIGUSR1, diminuto_handler) == SIG_ERR) {
            result = -3;
            diminuto_perror("diminuto_daemonize: signal(SIGUSR1)");
            break;
        }

        if (signal(SIGCHLD, diminuto_handler) == SIG_ERR) {
            result = -4;
            diminuto_perror("diminuto_daemonize: signal(SIGCHLD)");
            break;
        }

        if (signal(SIGALRM, diminuto_handler) == SIG_ERR) {
            result = -5;
            diminuto_perror("diminuto_daemonize: signal(SIGALRM)");
            break;
        }

        /* Fork off the daemon child and wait for it to signal us. */

        if ((pid = fork()) > 0) {
            alarm(10);
            pause();
            exit(1);
        } else if (pid < 0) {
            result = -6;
            diminuto_perror("diminuto_daemonize: fork");
            break;
        }

        /*
         *  CHILD
         */

        /* Create a lock file. */

        if (file == (char *)0) {
            /* Do nothing. */
        } else if ((rc = diminuto_lock(file)) < 0) {
            result = -7;
            diminuto_perror("diminuto_daemonize: diminuto_lock");
            break;
        }

        /* Find our parent. */

        if ((ppid = getppid()) < 0) {
            result = -8;
            diminuto_perror("diminuto_daemonize: getppid");
            break;
        }

        /* Reset our file mask. */

        umask(0);

        /* Orphan us from our controlling terminal and process group. */

        if (setsid() < 0) {
            result = -9;
            diminuto_perror("diminuto_daemonize: setsid");
            break;
        }

        /* Change to a directory that will always be there. */

        if (chdir("/") < 0) {
            result = -10;
            diminuto_perror("diminuto_daemonize: chdir");
            break;
        }

        /* Close all file descriptors except the big three. */

        rc = getdtablesize();
        for (fd = 3; fd < rc; ++fd) {
            (void)close(fd);
        }

        /* Dissociate ourselves from any signal handlers. */

        if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
            result = -11;
            diminuto_perror("diminuto_daemonize: signal(SIGCHLD)");
            break;
        }

        if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
            result = -12;
            diminuto_perror("diminuto_daemonize: signal(SIGTSTP)");
            break;
        }

        if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) {
            result = -13;
            diminuto_perror("diminuto_daemonize: signal(SIGTTOU)");
            break;
        }

        if (signal(SIGTTIN, SIG_IGN) == SIG_ERR) {
            result = -14;
            diminuto_perror("diminuto_daemonize: signal(SIGTTIN)");
            break;
        }

        if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
            result = -15;
            diminuto_perror("diminuto_daemonize: signal(SIGHUP)");
            break;
        }

        if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {
            result = -16;
            diminuto_perror("diminuto_daemonize: signal(SIGTERM)");
            break;
        }

        /* Redirect the big three to /dev/null. */

        if (freopen("/dev/null", "r", stdin) == (FILE *)0) {
            result = -17;
            diminuto_perror("diminuto_daemonize: freopen(stdin)");
            break;
        }

        fflush(stdout);

        if (freopen("/dev/null", "w", stdout) == (FILE *)0) {
            result = -18;
            diminuto_perror("diminuto_daemonize: freopen(stdout)");
            break;
        }

        fflush(stderr);

        if (freopen("/dev/null", "w", stderr) == (FILE *)0) {
            result = -19;
            diminuto_perror("diminuto_daemonize: freopen(stderr)");
            break;
        }

        /* Notify the parent that we were successful. */

        if (kill(ppid, SIGUSR1) < 0) {
            result = -20;
            diminuto_perror("diminuto_daemonize: kill");
            break;
        }

        /* Force a context switch so our parent to sees our notification. */

        if ((rc = diminuto_yield()) != 0) {
            result = -21;
            diminuto_perror("diminuto_daemonize: diminuto_yield");
            break;
        }

        result = 0;

    } while (0);

    return result;
}
