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

#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

static int received = 0;

static int diminuto_daemon_received(void)
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

static void diminuto_daemon_handler(int signum)
{
    if ((signum == SIGALRM) || (signum == SIGCHLD)) {
        received = signum;
    } else if (signum == SIGUSR1) {
        exit(0);
    } else {
        /* Do nothing: not one of our signals. */
    }
}

static int diminuto_daemon_install(int signum)
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = diminuto_daemon_handler;

    if (sigaction(signum, &action, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_daemon_install: sigaction");
        return -1;
    }

    return 0;
}

static int diminuto_daemon_ignore(int signum)
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_IGN;

    if (sigaction(signum, &action, (struct sigaction *)0) < 0) {
        diminuto_serror("diminuto_daemon_ignore: sigaction");
        return -1;
    }

    return 0;
}

static int diminuto_daemon_default(int signum)
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_DFL;

    if (sigaction(signum, &action, (struct sigaction *)0) < 0) {
        diminuto_serror("diminuto_daemon_default: sigaction");
        return -1;
    }

    return 0;
}

int diminuto_daemon(const char * file)
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
            diminuto_perror("diminuto_daemon: getppid");
            break;
        }

        if (ppid == 1) {
            return 0;
        }

        /* Establish a way for the daemon child to notify us. */

        if (diminuto_daemon_install(SIGUSR1) < 0) {
            break;
        }

        if (diminuto_daemon_install(SIGCHLD) < 0) {
            break;
        }

        if (diminuto_daemon_install(SIGALRM) < 0) {
            break;
        }

        /* Fork off the daemon child. */

        received = 0;

        if ((pid = fork()) < 0) {
            diminuto_perror("diminuto_daemon: fork");
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
            if (diminuto_daemon_received() == 0) {
                alarm(10);
                if (diminuto_daemon_received() == 0) {
                    pause();
                }
            }
            errno = ETIMEDOUT;
            diminuto_perror("diminuto_daemon: alarm");
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
            diminuto_perror("diminuto_daemon: getppid");
            break;
        }

        /* Create a lock file. */

        if (file == (char *)0) {
            /* Do nothing: no lock file. */
        } else if (diminuto_lock_lock(file) < 0) {
            diminuto_perror("diminuto_daemon: diminuto_lock");
            break;
        } else {
            /* Do nothing. */
        }

        /* Reset our file mask. */

        umask(0);

        /* Change to a directory that will always be there. */

        if (chdir("/") < 0) {
            diminuto_serror("diminuto_daemon: chdir");
            break;
        }

        /* Orphan us from our controlling terminal and process group. */

        if (setsid() < 0) {
            diminuto_perror("diminuto_daemon: setsid");
            break;
        }

        /*
         * From this point on, assume no terminal output.
         */

        /* Close all file descriptors except the big three. */

        fds = getdtablesize();
        for (fd = 3; fd < fds; ++fd) {
            (void)close(fd);
        }

        /* Dissociate ourselves from any signal handlers. */

        if (diminuto_daemon_default(SIGCHLD) < 0) {
            break;
        }

        if (diminuto_daemon_ignore(SIGTSTP) < 0) {
            break;
        }

        if (diminuto_daemon_ignore(SIGTTOU) < 0) {
            break;
        }

        if (diminuto_daemon_ignore(SIGTTIN) < 0) {
            break;
        }

        if (diminuto_daemon_ignore(SIGHUP) < 0) {
            break;
        }

        if (diminuto_daemon_default(SIGTERM) < 0) {
            break;
        }

        /* Redirect the big three to /dev/null. */

        if (freopen("/dev/null", "r", stdin) == (FILE *)0) {
            diminuto_serror("diminuto_daemon: freopen(stdin)");
            break;
        }

        fflush(stdout);

        if (freopen("/dev/null", "w", stdout) == (FILE *)0) {
            diminuto_serror("diminuto_daemon: freopen(stdout)");
            break;
        }

        fflush(stderr);

        if (freopen("/dev/null", "w", stderr) == (FILE *)0) {
            diminuto_serror("diminuto_daemon: freopen(stderr)");
            break;
        }

        signum = SIGUSR1;

    } while (0);

    /* Notify our parent and force a context switch so it sees it. */

    if (ppid < 0) {
        /* Do nothing: no parent. */
    } else if (kill(ppid, signum) < 0) {
        signum = SIGALRM;
        diminuto_serror("diminuto_daemon: kill");
    } else if (diminuto_yield() < 0) {
        diminuto_serror("diminuto_daemon: diminuto_yield");
    }

    if (signum != SIGUSR1) {
        exit(1);
    }

    return 0;
}
