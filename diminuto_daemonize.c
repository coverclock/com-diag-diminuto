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

#define SIGNAL SIGUSR1

typedef void (*sighandler_t)(int);

static void handler(int signum)
{
    exit(signum == SIGNAL ? 0 : signum + 1);
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
    sighandler_t sig;

    do {

        /* Make sure we are not already a daemon. */

        ppid = getppid();
        if (ppid < 0) {
            result = -2;
            diminuto_perror("diminuto_daemonize: getppid");
            break;
        }
        if (ppid == 1) {
            result = 0;
            break;
        }

        /* Establish a way for the daemon child to notify us. */

        sig = signal(SIGNAL, handler);
        if (sig == SIG_ERR) {
            result = -3;
            diminuto_perror("diminuto_daemonize: signal(SIGUSR1)");
            break;
        }

        sig = signal(SIGCHLD, handler);
        if (sig == SIG_ERR) {
            result = -4;
            diminuto_perror("diminuto_daemonize: signal(SIGCHLD)");
            break;
        }

        sig = signal(SIGALRM, handler);
        if (sig == SIG_ERR) {
            result = -5;
            diminuto_perror("diminuto_daemonize: signal(SIGALRM)");
            break;
        }

        /* Fork off the daemon child. */

        pid = fork();
        if (pid > 0) {
            alarm(10);
            pause();
            exit(1);
        }
        if (pid < 0) {
            result = -6;
            diminuto_perror("diminuto_daemonize: fork");
            break;
        }

        /* Create a lock file. */

        if (file != (char *)0) {
            rc = diminuto_lock(file);
            if (rc < 0) {
                result = rc;
                diminuto_perror("diminuto_daemonize: diminuto_lock");
                break;
            }
        }

        /* Find our parent. */

        ppid = getppid();
        if (ppid < 0) {
            result = -7;
            diminuto_perror("diminuto_daemonize: getppid");
            break;
        }

        /* Reset our file mask. */

        umask(0);

        /* Orphan us from our controlling terminal and process group. */

        sid = setsid();
        if (sid < 0) {
            result = -8;
            diminuto_perror("diminuto_daemonize: setsid");
            break;
        }

        /* Change to a directory that will always be there. */

        rc = chdir("/");
        if (rc < 0) {
            result = -9;
            diminuto_perror("diminuto_daemonize: chdir");
            break;
        }

        /* Close all file descriptors except the big three. */

        rc = getdtablesize();
        for (fd = 3; fd < rc; ++fd) {
            close(fd);
        }

        /* Dissociate ourselves from any signal handlers. */

        sig = signal(SIGCHLD, SIG_DFL);
        if (sig == SIG_ERR) {
            result = -10;
            diminuto_perror("diminuto_daemonize: signal(SIGCHLD)");
            break;
        }

        sig = signal(SIGTSTP, SIG_IGN);
        if (sig == SIG_ERR) {
            result = -11;
            diminuto_perror("diminuto_daemonize: signal(SIGTSTP)");
        }

        sig = signal(SIGTTOU, SIG_IGN);
        if (sig == SIG_ERR) {
            result = -12;
            diminuto_perror("diminuto_daemonize: signal(SIGTTOU)");
            break;
        }

        sig = signal(SIGTTIN, SIG_IGN);
        if (sig == SIG_ERR) {
            result = -13;
            diminuto_perror("diminuto_daemonize: signal(SIGTTIN)");
            break;
        }

        sig = signal(SIGHUP, SIG_IGN);
        if (sig == SIG_ERR) {
            result = -14;
            diminuto_perror("diminuto_daemonize: signal(SIGHUP)");
            break;
        }

        sig = signal(SIGTERM, SIG_DFL);
        if (sig == SIG_ERR) {
            result = -15;
            diminuto_perror("diminuto_daemonize: signal(SIGTERM)");
            break;
        }

        /* Redirect the big three to /dev/null. */

        fp = freopen("/dev/null", "r", stdin);
        if (fp == (FILE *)0) {
            result = -16;
            diminuto_perror("diminuto_daemonize: freopen(stdin)");
            break;
        }

        fflush(stdout);

        fp = freopen("/dev/null", "w", stdout);
        if (fp == (FILE *)0) {
            result = -17;
            diminuto_perror("diminuto_daemonize: freopen(stdout)");
            break;
        }

        fflush(stderr);

        fp = freopen("/dev/null", "w", stderr);
        if (fp == (FILE *)0) {
            result = -18;
            diminuto_perror("diminuto_daemonize: freopen(stderr)");
            break;
        }

        /* Notify the parent that we were successful. */

        rc = kill(ppid, SIGNAL);
        if (rc < 0) {
            result = -19;
            diminuto_perror("diminuto_daemonize: kill");
            break;
        }

        result = 0;

    } while (0);

    return result;
}
