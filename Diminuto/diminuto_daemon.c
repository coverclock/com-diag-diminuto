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
#include <fcntl.h>

static int alarmed = 0;

static void diminuto_daemon_handler(int signum)
{
    if (signum == SIGALRM) {
        alarmed = !0;
    } else if (signum == SIGUSR1) {
        exit(0);
    } else {
        /* Do nothing. */
    }
}

static int diminuto_daemon_install(int signum)
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = diminuto_daemon_handler;

    if (sigaction(signum, &action, (struct sigaction *)0) < 0) {
        diminuto_serror("diminuto_daemon_install: sigaction");
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

int diminuto_daemon_test(const char * name, const char * file, int fail)
{
    int signum = SIGALRM;
    pid_t ppid = -1;
    pid_t spid = -1;
    pid_t pid = -1;
    int fds = 0;
    int fd = -1;

    /*
     *  PARENT
     */

    do {

        /* Make sure we are not already a daemon. */

        if ((ppid = getppid()) < 0) {
            diminuto_serror("diminuto_daemon: getppid");
            break;
        }

        if (ppid == 1) {
            return 0;
        }

        /* This will be the process that the daemon signals. */

        if ((spid = getpid()) < 0) {
        	diminuto_serror("diminuto_daemon: getpid");
        	break;
        }

        /* Establish a way for the daemon child to notify us. */

        if (diminuto_daemon_install(SIGUSR1) < 0) {
            break;
        }

        if (diminuto_daemon_install(SIGALRM) < 0) {
            break;
        }

        if (diminuto_daemon_install(SIGCHLD) < 0) {
            break;
        }

        /* Flush output before we mess with it. */

        (void)fflush(stdout);
        (void)fflush(stderr);

        /* Fork off the daemon child. */

        alarmed = 0;

        if ((pid = fork()) < 0) {
            diminuto_serror("diminuto_daemon: fork");
            break;
        }

    } while (0);

	/*
	 * N.B. We may get a SIGUSR1 (success) or a SIGALRM (failure)
	 *      from the child at any time, including before we even
	 *      examine the return code from the fork(). We try not
	 *      to wait for ten seconds below if the child has already
	 *      signaled us, but there is no reliable way to be sure.
	 */

	/* Wait for the daemon child to signal us. */

	if (pid != 0) {
		if (pid > 0) {
			if (!alarmed) {
				alarm(10);
				while (!alarmed) {
					pause();
				}
			}
		}
		return -1;
	}

    /*
     *  CHILD
     */

    do {

        /* Orphan us from our controlling terminal and process group. */

        if (setsid() < 0) {
            diminuto_serror("diminuto_daemon: setsid");
            break;
        }

        /* Fork again to abandon any hope of having a controlling terminal. */

        if ((pid = fork()) < 0) {
            diminuto_serror("diminuto_daemon: fork");
            break;
        }

        if (pid > 0) {
        	exit(0);
        }

        /* Dissociate ourselves from any problematic signal handlers. */

        if (diminuto_daemon_ignore(SIGUSR1) < 0) {
            break;
        }

        if (diminuto_daemon_ignore(SIGALRM) < 0) {
            break;
        }

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

        /* Reset our file mask. */

        umask(0);

        /* Change to a directory that will always be there. */

        if (chdir("/") < 0) {
            diminuto_serror("diminuto_daemon: chdir");
            break;
        }

        /*
         * So why do we do all this stuff below? I saw an interesting bug
         * the other day while helping someone else debug their code. The
         * developer had closed file descriptors 0, 1, and 2, daemonized
         * using his own technique, and then opened sockets to communicate with
         * other processes. But a function in an underlying framework continued
         * to use printf to emit debug messages. The FILE stdout was still
         * using file descriptor 1, but that was now a socket for some
         * completely different purpose. The receiver got all confused
         * because this application was sending it debug output instead of
         * messages. Wackiness ensued. So here we redirect 0, 1, and 2 to
         * /dev/null, and then we make sure that stdin, stdout, and stderr are
         * also redirected to /dev/null. And then we close everything else.
         * We have to be careful not to close the file descriptor associated
         * with sending stuff to syslog.
         */

        /* Redirect the big three descriptors to /dev/null. */

        if ((fd = open("/dev/null", O_RDONLY, 0)) < 0) {
        	diminuto_serror("diminuto_daemon: open(STDIN_FILENO)");
        	break;
        } else if (fd == STDIN_FILENO) {
        	/* Do nothing. */
        } else if (dup2(fd, STDIN_FILENO) < 0) {
        	diminuto_serror("diminuto_daemon: du2(STDIN_FILENO)");
        	break;
        } else if (close(fd) < 0) {
        	diminuto_serror("diminuto_daemon: close(STDIN_FILENO)");
        	break;
        } else {
        	/* Do nothing. */
        }

        if ((fd = open("/dev/null", O_WRONLY, 0)) < 0) {
        	diminuto_serror("diminuto_daemon: open(STDOUT_FILENO)");
        	break;
        } else if (fd == STDOUT_FILENO) {
        	/* Do nothing. */
        } else if (dup2(fd, STDOUT_FILENO) < 0) {
        	diminuto_serror("diminuto_daemon: du2(STDOUT_FILENO)");
        	break;
        } else if (close(fd) < 0) {
        	diminuto_serror("diminuto_daemon: close(STDOUT_FILENO)");
        	break;
        } else {
        	/* Do nothing. */
        }

        if ((fd = open("/dev/null", O_WRONLY, 0)) < 0) {
        	diminuto_serror("diminuto_daemon: open(STDERR_FILENO)");
        	break;
        } else if (fd == STDERR_FILENO) {
        	/* Do nothing. */
        } else if (dup2(fd, STDERR_FILENO) < 0) {
        	diminuto_serror("diminuto_daemon: du2(STDERR_FILENO)");
        	break;
        } else if (close(fd) < 0) {
        	diminuto_serror("diminuto_daemon: close(STDERR_FILENO)");
        	break;
        } else {
        	/* Do nothing. */
        }

        /* Redirect the big three FILEs to /dev/null. */

        if (fileno(stdin) == STDIN_FILENO) {
        	/* Do nothing. */
        } else if (freopen("/dev/null", "r", stdin) == (FILE *)0) {
            diminuto_serror("diminuto_daemon: freopen(stdin)");
            break;
        } else {
        	/* Do nothing. */
        }

        if (fileno(stdout) == STDOUT_FILENO) {
        	/* Do nothing. */
        } else if (freopen("/dev/null", "w", stdout) == (FILE *)0) {
            diminuto_serror("diminuto_daemon: freopen(stdout)");
            break;
        } else {
        	/* Do nothing. */
        }

        if (fileno(stdout) == STDOUT_FILENO) {
        	/* Do nothing. */
        } else if (freopen("/dev/null", "w", stderr) == (FILE *)0) {
            diminuto_serror("diminuto_daemon: freopen(stderr)");
            break;
        } else {
        	/* Do nothing. */
        }

        /* Close the system log socket. */

        closelog();

        /* Close all unused file descriptors. */

        fds = getdtablesize();
        for (fd = 0; fd < fds; ++fd) {
        	if (fd == STDIN_FILENO) {
        		/* Do nothing. */
        	} else if (fd == STDOUT_FILENO) {
        		/* Do nothing. */
        	} else if (fd == STDERR_FILENO) {
        		/* Do nothing. */
        	} else if (fd == fileno(stdin)) {
        		/* Do nothing. */
        	} else if (fd == fileno(stdout)) {
        		/* Do nothing. */
        	} else if (fd == fileno(stderr)) {
        		/* Do nothing. */
        	} else {
        		(void)close(fd);
        	}
        }

        /* Open a new system log socket. */

        openlog((name != (const char *)0) ? name : "(diminuto_daemon)", LOG_CONS | LOG_PID, LOG_USER);

        /* If forced to fail, fail now. */

        if (fail) {
        	errno = 0;
            diminuto_serror("diminuto_daemon: fail");
        	break;
        }

        /* Create a lock file. */

        if (file == (char *)0) {
            /* Do nothing. */
        } else if (diminuto_lock_lock(file) < 0) {
            diminuto_serror("diminuto_daemon: diminuto_lock");
            break;
        } else {
            /* Do nothing. */
        }

        signum = SIGUSR1;

    } while (0);

    /*
     * DAEMON
     */

    do {

        /* Notify our parent. */

        if (kill(spid, signum) < 0) {
        	diminuto_serror("diminuto_daemon: kill");
        	break;
        }

        /* Force a context switch so it sees it. */

        if (diminuto_yield() < 0) {
        	diminuto_serror("diminuto_daemon: diminuto_yield");
        	break;
        }

        if (signum != SIGUSR1) {
        	break;
        }

        return 0;

    } while (0);

    exit(1);
}

int diminuto_daemon(const char * name, const char * file)
{
	return diminuto_daemon_test(name, file, 0);
}
