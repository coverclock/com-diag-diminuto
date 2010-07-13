/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * @see http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize
 */

#include "diminuto_daemon.h"
#include "diminuto_lock.h"
#include "diminuto_delay.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

static int received = 0;

static int diminuto_received(void)
{
    int signum;
    sigset_t set;
    sigset_t was;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, &was);

    signum = received;
    received = 0;

    sigprocmask(SIG_SETMASK, &was, (sigset_t *)0);

    return signum;
}

static void diminuto_handler(int signum)
{
    if ((signum == SIGALRM) || (signum == SIGCHLD)) {
        received = signum;
    } else if (signum == SIGUSR1) {
        exit(0);
    } else {
        /* Do nothing: not one of our signals. */
    }
}

int diminuto_daemon_enable(const char * file)
{
    pid_t ppid = -1;
    pid_t pid = -1;
    int signum = SIGALRM;
    int fds = 0;
    int fd = -1;

    /*
     *  PARENT
     */

    do {

        /* Make sure we are not already a daemon. */

        if ((ppid = getppid())  < 0) {
            diminuto_serror("diminuto_daemonize: getppid");
            break;
        }

        if (ppid == 1) {
            return 0;
        }

        /* Establish a way for the daemon child to notify us. */

        if (signal(SIGUSR1, diminuto_handler) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGUSR1)");
            break;
        }

        if (signal(SIGCHLD, diminuto_handler) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGCHLD)");
            break;
        }

        if (signal(SIGALRM, diminuto_handler) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGALRM)");
            break;
        }

        /* Fork off the daemon child. */

        received = 0;

        if ((pid = fork()) < 0) {
            diminuto_serror("diminuto_daemonize: fork");
            break;
        }

        /*
         * N.B. We may get a SIGUSR1 (success) or a SIGALRM (failure)
         *      from the child at any time, including before we even
         *      examine the return code from the fork(). We try not
         *      to wait for ten seconds below if the child has already
         *      signaled us, but there is no way to be sure.
         */

        /* Wait for the daemon child to signal us. */

        if (pid > 0) {
            if (diminuto_received() == 0) {
                alarm(10);
                if (diminuto_received() == 0) {
                    pause();
                }
            }
            errno = ETIMEDOUT;
            diminuto_serror("diminuto_daemonize: alarm");
            break;
        }

    } while (0);

    /* If we're not the child, we've failed. */

    if (pid != 0) {
        return -1;
    }

    /*
     *  CHILD
     */

    do {

        /* Find our parent. */

        if ((ppid = getppid()) < 0) {
            diminuto_serror("diminuto_daemonize: getppid");
            break;
        }

        /* Create a lock file. */

        if (file == (char *)0) {
            /* Do nothing: no lock file. */
        } else if (diminuto_lock_lock(file) < 0) {
            diminuto_serror("diminuto_daemonize: diminuto_lock");
            break;
        }

        /* Reset our file mask. */

        umask(0);

        /* Orphan us from our controlling terminal and process group. */

        if (setsid() < 0) {
            diminuto_serror("diminuto_daemonize: setsid");
            break;
        }

        /* Change to a directory that will always be there. */

        if (chdir("/") < 0) {
            diminuto_serror("diminuto_daemonize: chdir");
            break;
        }

        /* Close all file descriptors except the big three. */

        fds = getdtablesize();
        for (fd = 3; fd < fds; ++fd) {
            (void)close(fd);
        }

        /* Dissociate ourselves from any signal handlers. */

        if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGCHLD)");
            break;
        }

        if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGTSTP)");
            break;
        }

        if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGTTOU)");
            break;
        }

        if (signal(SIGTTIN, SIG_IGN) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGTTIN)");
            break;
        }

        if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGHUP)");
            break;
        }

        if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {
            diminuto_serror("diminuto_daemonize: signal(SIGTERM)");
            break;
        }

        /* Redirect the big three to /dev/null. */

        if (freopen("/dev/null", "r", stdin) == (FILE *)0) {
            diminuto_serror("diminuto_daemonize: freopen(stdin)");
            break;
        }

        fflush(stdout);

        if (freopen("/dev/null", "w", stdout) == (FILE *)0) {
            diminuto_serror("diminuto_daemonize: freopen(stdout)");
            break;
        }

        fflush(stderr);

        if (freopen("/dev/null", "w", stderr) == (FILE *)0) {
            diminuto_serror("diminuto_daemonize: freopen(stderr)");
            break;
        }

        signum = SIGUSR1;

    } while (0);

    /* Notify our parent and force a context switch so it sees it. */

    if (ppid < 0) {
        /* Do nothing: no parent. */
    } else if (kill(ppid, signum) < 0) {
        signum = SIGALRM;
        diminuto_serror("diminuto_daemonize: kill");
    } else if (diminuto_yield() < 0) {
        diminuto_serror("diminuto_daemonize: diminuto_yield");
    }

    if (signum != SIGUSR1) {
        exit(1);
    }

    return 0;
}
