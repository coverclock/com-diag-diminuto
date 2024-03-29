/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Daemon feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Daemon feature.
 *
 * REFERENCES
 *
 * http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize
 */

#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../src/diminuto_daemon.h"

diminuto_daemon_test_t diminuto_daemon_testing = DIMINUTO_DAEMON_TEST_NONE;

diminuto_daemon_test_t diminuto_daemon_test(diminuto_daemon_test_t test)
{
    int tested;

    tested = diminuto_daemon_testing;
    diminuto_daemon_testing = test;

    return tested;
}

int diminuto_daemon_install(int signum, void (*handler)(int))
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;

    if (sigaction(signum, &action, (struct sigaction *)0) < 0) {
        diminuto_serror("diminuto_daemon: sigaction");
        return -1;
    }

    return 0;
}

void diminuto_daemon_prepare(void)
{
    if (fflush(stdout) == EOF) {
        diminuto_serror("diminuto_daemon: fflush(stdout)");
    }

    if (fflush(stderr) == EOF) {
        diminuto_serror("diminuto_daemon: fflush(stderr)");
    }
}

int diminuto_daemon_verify(void)
{
    int rc = -1;
    pid_t pid = -1;

    if (diminuto_daemon_testing == DIMINUTO_DAEMON_TEST_GETPPID) {
        /* Do nothing. */
    } else if ((pid = getppid()) < 0) {
        diminuto_perror("diminuto_daemon: getppid");
    } else if (diminuto_daemon_testing == DIMINUTO_DAEMON_TEST_INIT) {
        rc = 1;
    } else if (pid == 1) {
        rc = 1;
    } else {
        rc = 0;
    }

    return rc;
}

/*
 * Using strace(1), grep(1) for "clone" not "fork" in the output of script(1).
 */
pid_t diminuto_daemon_fork(void)
{
    pid_t pid = -1;
    int rc = -1;
    int status = -1;

    if (diminuto_daemon_testing == DIMINUTO_DAEMON_TEST_FORK) {
        /* Do nothing. */
    } else if ((pid = fork()) < 0) {
        diminuto_perror("diminuto_daemon: fork");
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        while (!0) {
            if (diminuto_daemon_testing == DIMINUTO_DAEMON_TEST_WAITPID) {
                break;
            } else if ((rc = waitpid(pid, &status, 0)) > 0) {
                break; /* Nominal. */
            } else if (rc == 0) {
                break; /* Should never happen. */
            } else if (errno == EINTR) {
                continue; /* Ignore. */
            } else if (errno == ECHILD) {
                break; /* Caller probably handled SIGCHLD. */
            } else {
                diminuto_serror("diminuto_daemon: waitpid");
                break; /* Error. */
            }
        }
        if (rc <= 0) {
            pid = -1;
        } else if (!WIFEXITED(status)) {
            pid = -1;
        } else if (WEXITSTATUS(status) != 0) {
            pid = -1;
        } else {
            /* Do nothing. */
        }
    }

    return pid;
}

/*
 * Using strace(1), grep(1) for "clone" not "fork" in the output of script(1).
 */
pid_t diminuto_daemon_refork(void)
{
    pid_t pid = -1;

    if (diminuto_daemon_testing == DIMINUTO_DAEMON_TEST_REFORK) {
        /* Do nothing. */
    } else if ((pid = fork()) < 0) {
        diminuto_serror("diminuto_daemon: refork");
    } else {
        /* Do nothing. */
    }

    return pid;
}

/*
 * So why do we do all this FILE stuff? I saw an interesting bug
 * the other day while helping someone else debug their code. The
 * developer had closed file descriptors 0, 1, and 2, daemonized
 * using his own technique, and then opened sockets to communicate with
 * other processes. But a function in an underlying framework continued
 * to use printf to emit debug messages. The FILE stdout was still
 * using file descriptor 1, but that was now a socket for some
 * completely different purpose. The receiver got all confused
 * because this application was sending it debug output instead of
 * messages. Wackiness ensued. So here we redirect 0, 1, and 2 to
 * /dev/null, then we make sure that stdin, stdout, and stderr are
 * also redirected to 0, 1, and 2. And then we close everything else.
 * We have to be careful not to leave the file descriptor associated
 * with sending stuff to syslog closed; sure wish there was an API call
 * to tell what that file descriptor was.
 */
void diminuto_daemon_redirect(const char * path, int number, int flags, FILE ** filep, const char * mode)
{
    int fd = -1;

    /* Redirect the file descriptor to the path. */

    if ((fd = open(path, flags, 0)) < 0) {
        diminuto_serror("diminuto_daemon: open");
    } else if (fd == number) {
        /* Do nothing. */
    } else if (dup2(fd, number) < 0) {
        diminuto_serror("diminuto_daemon: dup2");
    } else if (close(fd) < 0) {
        diminuto_serror("diminuto_daemon: close");
    } else {
        /* Do nothing. */
    }

    /* Close the FILE stream if it isn't already the file descriptor. */

    if (fd < 0) {
        /* Do nothing. */
    } else if (*filep == (FILE *)0) {
        /* Do nothing. */
    } else if (fileno(*filep) == number) {
        /* Do nothing. */
    } else if (fclose(*filep) == 0) {
        /* Do nothing. */
    } else {
        diminuto_serror("diminuto_daemon: fclose");
    }

    /* Reopen the FILE stream and assign it to the descriptor. */

    if (fd < 0) {
        /* Do nothing. */
    } else if ((*filep != (FILE *)0) && (fileno(*filep) == number)) {
        /* Do nothing. */
    } else if ((*filep = fdopen(number, "r")) != (FILE *)0) {
        /* Do nothing. */
    } else {
        diminuto_serror("diminuto_daemon: fdopen");
    }

}

void diminuto_daemon_sanitize(const char * name, const char * path)
{
    long fds = 0;
    int fd = -1;

    /* Orphan ourselves from our original terminal session. */

    if (setsid() < 0) {
        diminuto_serror("diminuto_daemon: setsid");
    }

    /* Dissociate ourselves from any problematic signal handlers. */

    diminuto_daemon_ignore(SIGALRM);

    diminuto_daemon_default(SIGCHLD);

    diminuto_daemon_ignore(SIGTSTP);

    diminuto_daemon_ignore(SIGTTOU);

    diminuto_daemon_ignore(SIGTTIN);

    diminuto_daemon_ignore(SIGHUP);

    diminuto_daemon_default(SIGTERM);

    /* Reset our file mask (always succeeds). */

    (void)umask(0);

    /* Change to a directory that will always be there. */

    if (chdir("/") < 0) {
        diminuto_serror("diminuto_daemon: chdir");
    }

    /* Redirect the big three descriptors. */

    if (path != (const char *)0) {
        diminuto_daemon_redirect(path, STDIN_FILENO, O_RDONLY, &stdin, "r");
        diminuto_daemon_redirect(path, STDOUT_FILENO, O_WRONLY, &stdout, "w");
        diminuto_daemon_redirect(path, STDERR_FILENO, O_WRONLY, &stderr, "w");
    }

    /* Close the system log socket. */

    diminuto_log_close();

    /* Close all unused file descriptors. */

    fds = diminuto_fd_count();

    /*
     * Another approach to this I thought was interesting is if you know
     * you are going to do an exec(2), mark all of the file descriptors as
     * "close on exec" using fcntl(2). The caller could have done this
     * when using open(2) as well. Of course this is only applicable if you
     * are doing an exec(2), not just daemonizing.
     */

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
        } else if (close(fd) < 0) {
            /* Do nothing. */
        } else {
            /* Do nothing. */
        }
    }

    /* Open a new system log socket which will probably be fileno 3. */

    if (name != (const char *)0) {
        diminuto_log_open(name);
    }
}

/*
 * It's possible the application *wants* to pass some file descriptors to
 * the daemon child. This implementation doesn't support that model. The
 * child is responsible for opening its own damn file descriptors. I think
 * if you have that kind of model, a better approach is to implement a
 * multi-threaded application, rather than an old school multi-process
 * application; or maybe a single threaded state machine that handles many
 * connections. Or use diminuto_service() instead of diminuto_daemon().
 */
int diminuto_daemon(const char * name)
{
    pid_t pid = -1;
    int rc = -1;

    /*
     * PARENT
     */

    /* Make sure we are not already a daemon. */

    if ((rc = diminuto_daemon_verify()) < 0) {
        _exit(1);
    } else if (rc) {
        return 0;
    } else {
        /* Do nothing. */
    }

    /* Flush output before we mess with it. */

    diminuto_daemon_prepare();

    /* Fork the first child and reap it. */

    if ((pid = diminuto_daemon_fork()) < 0) {
        _exit(1);
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        _exit(0);
    }

    /*
     * FIRST CHILD
     */

    /* Fork and exit so the second child is inherited by the init process. */

    if ((pid = diminuto_daemon_refork()) < 0) {
        _exit(1);
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        _exit(0);
    }

    /*
     * SECOND CHILD
     */

    /* Sanitize the context of the second child. */

    diminuto_daemon_sanitize(name, "/dev/null");

    return 0;
}

/*
 * Four years ago I ran into an issue in an Asterisk 11.5.0 system I was
 * working on for a client. Then, just the other day, when asked to
 * implement some minor features on this very same product (an Iridium
 * satellite communications system for business aircraft), I ran into
 * exactly the same issue in a slightly different context. Deja vu.
 * While spawning off a child process using the now-ancient version of
 * the Asterisk function ast_safe_system(), which uses fork(2) and exec(2)
 * from an equally ancient version of Linux, the resulting child process
 * was getting hung on the close(2) in a fast user-space mutex. The only fix
 * a colleague and I could come up with four years ago was not to do the
 * close. Which is highly troublesome, particularly in light of how many
 * file descriptors (including sockets and such) Asterisk keeps open.
 * The only factor that was really questionable is this was multi-
 * threaded code. I need to go look at the implementation of close(2) in
 * that old Linux kernel to understand this better. Anyway, this function
 * doesn't do any sanitization, so the daemon child inherits all the parents
 * context.
 */
int diminuto_service(void)
{
    pid_t pid = -1;

    /*
     * PARENT
     */

    /* Fork the first child and reap it. */

    if ((pid = diminuto_daemon_fork()) < 0) {
        return -1;
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        return !0;
    }

    /*
     * FIRST CHILD
     */

    /* Fork and exit so the second child is inherited by the init process. */

    if ((pid = diminuto_daemon_refork()) < 0) {
        _exit(1);
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        _exit(0);
    }

    /*
     * SECOND CHILD
     */

    /* This seems necessary for the service to use the system log. */

    diminuto_log_close();
    diminuto_log_open((char *)0);

    return 0;
}

int diminuto_system(const char * command)
{
    pid_t pid = -1;

    /*
     * PARENT
     */

    /* Fork the first child and reap it. */

    if ((pid = diminuto_daemon_fork()) < 0) {
        return -1;
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        return 0;
    }

    /*
     * FIRST CHILD
     */

    /* Fork and exit so the second child is inherited by the init process. */

    if ((pid = diminuto_daemon_refork()) < 0) {
        _exit(1);
    } else if (pid == 0) {
        /* Do nothing. */
    } else {
        _exit(0);
    }

    /*
     * SECOND CHILD
     */

    /* Sanitize the context of the second child. */

    diminuto_daemon_sanitize((const char *)0, (const char *)0);

    /* Run the command; if successful, the execl(2) never returns. */

    execl("/bin/sh", "bin/sh", "-c", command, (char *)0);
    diminuto_serror("diminuto_system: execl");
    _exit(1);
}
