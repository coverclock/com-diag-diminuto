/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Log feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Log feature.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_environment.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../src/diminuto_log.h"
#include "../src/diminuto_time.h"

/*******************************************************************************
 * LOCALS
 *****************************************************************************/

static const char * PRIORITIES[] = {
    "EMER",
    "ALRT",
    "CRIT",
    "EROR",
    "WARN",
    "NOTE",
    "INFO",
    "DBUG",
};

static const char ALL[] = DIMINUTO_LOG_MASK_VALUE_ALL;

static uint8_t initialized = 0;

/*******************************************************************************
 * PUBLIC GLOBALS
 *****************************************************************************/

diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

/*******************************************************************************
 * PRIVATE GLOBALS
 *****************************************************************************/

pthread_mutex_t diminuto_log_mutex = PTHREAD_MUTEX_INITIALIZER;

const char * diminuto_log_ident = DIMINUTO_LOG_IDENT_DEFAULT;

int diminuto_log_option = DIMINUTO_LOG_OPTION_DEFAULT;

int diminuto_log_facility = DIMINUTO_LOG_FACILITY_DEFAULT;

int diminuto_log_descriptor = DIMINUTO_LOG_DESCRIPTOR_DEFAULT;

FILE * diminuto_log_file = DIMINUTO_LOG_STREAM_DEFAULT;

const char * diminuto_log_mask_name = DIMINUTO_LOG_MASK_NAME_DEFAULT;

const char * diminuto_log_mask_path = DIMINUTO_LOG_MASK_PATH_DEFAULT;

diminuto_log_strategy_t diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_AUTOMATIC;

bool diminuto_log_cached = false;

diminuto_log_priority_t diminuto_log_priority = DIMINUTO_LOG_PRIORITY_DEFAULT;

diminuto_log_priority_t diminuto_log_error = DIMINUTO_LOG_PRIORITY_PERROR;

bool diminuto_log_error_suppress = false;

size_t diminuto_log_lost = 0;

/*******************************************************************************
 * BASE FUNCTIONS
 *****************************************************************************/

diminuto_log_mask_t diminuto_log_priority2mask(diminuto_log_priority_t priority)
{
    diminuto_log_mask_t mask = 0;

    switch (priority) {
    case DIMINUTO_LOG_PRIORITY_EMERGENCY:
        mask = DIMINUTO_LOG_MASK_EMERGENCY;
        break;
    case DIMINUTO_LOG_PRIORITY_ALERT:
        mask = DIMINUTO_LOG_MASK_ALERT;
        break;
    case DIMINUTO_LOG_PRIORITY_CRITICAL:
        mask = DIMINUTO_LOG_MASK_CRITICAL;
        break;
    case DIMINUTO_LOG_PRIORITY_ERROR:
        mask = DIMINUTO_LOG_MASK_ERROR;
        break;
    case DIMINUTO_LOG_PRIORITY_WARNING:
        mask = DIMINUTO_LOG_MASK_WARNING;
        break;
    case DIMINUTO_LOG_PRIORITY_NOTICE:
        mask = DIMINUTO_LOG_MASK_NOTICE;
        break;
    case DIMINUTO_LOG_PRIORITY_INFORMATION:
        mask = DIMINUTO_LOG_MASK_INFORMATION;
        break;
    case DIMINUTO_LOG_PRIORITY_DEBUG:
        mask = DIMINUTO_LOG_MASK_DEBUG;
        break;
    default:
        mask = DIMINUTO_LOG_MASK_ALL;
        break;
    }

    return mask;
}

diminuto_log_mask_t diminuto_log_initmask(diminuto_log_mask_t after) {
    diminuto_log_mask_t before = 0;

    DIMINUTO_LOG_SECTION_BEGIN;
        before = DIMINUTO_LOG_MASK;
        DIMINUTO_LOG_MASK = after;
    DIMINUTO_LOG_SECTION_END;

    return before;
}

diminuto_log_mask_t diminuto_log_setmask(void)
{
    const char * string = (const char *)0;
    char * end = (char *)0;
    long value = -1;

    string = diminuto_getenv(diminuto_log_mask_name);
    if (string == (const char *)0) {
        /* Do nothing: not an error. */
    } else if (strncmp(string, ALL, sizeof(ALL)) == 0) {
        DIMINUTO_LOG_SECTION_BEGIN;
            DIMINUTO_LOG_MASK = DIMINUTO_LOG_MASK_ALL;
        DIMINUTO_LOG_SECTION_END;
    } else if ((value = strtol(string, &end, 0)) < 0) {
        errno = ERANGE;
        perror(string);
    } else if ((end == (char *)0) || (*end != '\0')) {
        errno = EINVAL;
        perror(string);
    } else {
        DIMINUTO_LOG_SECTION_BEGIN;
            DIMINUTO_LOG_MASK = value;
        DIMINUTO_LOG_SECTION_END;
    }

    return DIMINUTO_LOG_MASK;
}

diminuto_log_mask_t diminuto_log_importmask(const char * path)
{
    diminuto_path_t filename = { '\0', };
    char buffer[81] = { '\0', }; /* "0377\n", "255\n", "0xff\n", "~0\n" */
    FILE * fp = (FILE *)0;
    long value = -1;
    char * end = (char *)0;

    if (path == (const char *)0) {
        const char * home = (const char *)0;
        home = diminuto_getenv("HOME");
        if (home == (const char *)0) {
            home = "."; /* Not an error (but unlikely). */
        }
        (void)snprintf(filename, sizeof(filename), "%s/%s", home, diminuto_log_mask_path);
        filename[sizeof(filename) - 1] = '\0';
        path = filename;
    }

    /*
     * Note that fgets(3) includes the newline at the end of the data.
     */

    if (diminuto_fs_type(path) == DIMINUTO_FS_TYPE_NONE) {
        /* Do nothing: not an error. */
    } else if ((fp = fopen(path, "r")) == (FILE *)0) {
        perror(path);
    } else if (fgets(buffer, sizeof(buffer), fp) == (char *)0) {
        perror(path);
    } else if (strncmp(buffer, ALL, sizeof(ALL) - 1) == 0) {
        DIMINUTO_LOG_SECTION_BEGIN;
            DIMINUTO_LOG_MASK = DIMINUTO_LOG_MASK_ALL;
        DIMINUTO_LOG_SECTION_END;
    } else if ((value = strtol(buffer, &end, 0)) < 0) {
        errno = ERANGE;
        perror(buffer);
    } else if ((end == (char *)0) || ((*end != '\0') && (*end != '\n') && (*end != ' ') && (*end != '\t') && (*end != '#'))) {
        errno = EINVAL;
        perror(buffer);
    } else {
        DIMINUTO_LOG_SECTION_BEGIN;
            DIMINUTO_LOG_MASK = value;
        DIMINUTO_LOG_SECTION_END;
    }

    if (fp != (FILE *)0) {
        (void)fclose(fp);
    }

    return DIMINUTO_LOG_MASK;
}

void diminuto_log_open_syslog(const char * name, int option, int facility)
{
    DIMINUTO_LOG_SECTION_BEGIN;
        if (name != (const char *)0) {
            diminuto_log_ident = name;
        }

#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
        if (!initialized) {
            openlog(diminuto_log_ident, option, facility);
            initialized = !0;
        }
#endif

        /*
         * I wuold have liked to have sached the Process IDentifier,
         * but if the calling application does a fork, the PID of the
         * new process will no longer be cached value. But once the
         * the calling process becomes a daemon (which is whqt is being
         * determined below), there is no going back; we assume any
         * child process of the daemon will be a deamon.
         */

        if (diminuto_log_cached) {
            /* Do nothing. */
        } else if (getpid() == getsid(0)) {
            diminuto_log_cached = true;
        } else if (getppid() == 1) {
            diminuto_log_cached = true;
        } else {
            /* Do nothing. */
        }
    DIMINUTO_LOG_SECTION_END;
}

void diminuto_log_open(const char * name)
{
    diminuto_log_open_syslog(name, diminuto_log_option, diminuto_log_facility);
}

void diminuto_log_close(void)
{
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)

    DIMINUTO_LOG_SECTION_BEGIN;
        if (initialized) {
            closelog();
            initialized = 0;
        }
    DIMINUTO_LOG_SECTION_END;

#endif
}

FILE * diminuto_log_stream(void)
{
    DIMINUTO_LOG_SECTION_BEGIN;

        /*
         * If the file number of the log FILE object does not match
         * that of the log file descriptor, close it (to be reopened
         * later using the log file descriptor), unless that log FILE
         * object happends to be stderr or stdout, or unless the file
         * number of the log FILE object happends to be the file number
         * of stderr or stdout. (Log never closes stdout or stderr, either
         * via their FILE objects or their file descriptors.)
         */

        if (diminuto_log_file == (FILE *)0) {
            /* Do nothing. */
        } else if (diminuto_log_descriptor == fileno(diminuto_log_file)) {
            /* Do nothing. */
        } else if (diminuto_log_file == stderr) {
            diminuto_log_file = (FILE *)0;
        } else if (diminuto_log_file == stdout) {
            diminuto_log_file = (FILE *)0;
        } else if (fileno(diminuto_log_file) == STDERR_FILENO) {
            diminuto_log_file = (FILE *)0;
        } else if (fileno(diminuto_log_file) == STDOUT_FILENO) {
            diminuto_log_file = (FILE *)0;
        } else if (fclose(diminuto_log_file) == EOF) {
            /*
             * perror() does not depend on diminuto_log_stream()
             * nor the value of diminuto_log_file.
             */
            perror("diminuto_log_stream: fclose");
            diminuto_log_file = (FILE *)0;
        } else {
            diminuto_log_file = (FILE *)0;
        }

        /*
         * If we don't have a log FILE object, make one from the log file
         * descriptor, unless the log file descriptor happens to be the
         * file number of stderr or stdout, in which case just use those.
         */

        if (diminuto_log_file != (FILE *)0) {
            /* Do nothing. */
        } else if (diminuto_log_descriptor == fileno(stderr)) {
            diminuto_log_file = stderr;
        } else if (diminuto_log_descriptor == fileno(stdout)) {
            diminuto_log_file = stdout;
        } else if ((diminuto_log_file = fdopen(diminuto_log_descriptor, "a")) == (FILE *)0) {
            /*
             * perror() does not depend on diminuto_log_stream()
             * nor the value of diminuto_log_file.
             */
            perror("diminuto_log_stream: fdopen");
        } else {
            /* Do nothing. */
        }
    DIMINUTO_LOG_SECTION_END;

    return diminuto_log_file;
}

void diminuto_log_vsyslog(int priority, const char * format, va_list ap)
{
    diminuto_log_open((const char *)0);
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        vsyslog(priority, format, ap);
    DIMINUTO_ENVIRONMENT_READER_END;
#else
    __android_log_vprint(priority, diminuto_log_ident, format, ap);
#endif
}

void diminuto_log_vwrite(int fd, int priority, const char * format, va_list ap)
{
    diminuto_sticks_t now = -1;
    int year, month, day, hour, minute, second = 0;
    diminuto_ticks_t nanosecond = 0;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM] = { '\0', };
    int rc = -1;
    char * pointer = buffer;
    size_t space = sizeof(buffer);
    size_t total = 0;
    static char hostname[DIMINUTO_LOG_HOSTNAME_MAXIMUM] = { '\0', };

    /*
     * We use special versions of these functions from the private Time API
     * that do not log messages in the event of an error. This is to prevent
     * an infinite recursion (at least, until the stack runs out) if the
     * underlying C library functions or kernel system calls fail.
     */

    if ((now = diminuto_time_clock_logging()) == DIMINUTO_TIME_ERROR) {
        /* Do nothing. */
    } else if (diminuto_time_zulu_logging(now, &year, &month, &day, &hour, &minute, &second, &nanosecond) < 0) {
        now = DIMINUTO_TIME_ERROR;
    } else {
        /* Do nothing. */
    }

    /*
     * Note that the hostname is cached; subsequent log emissions
     * will not reflect any hostname changes (which are unlikely
     * without the system being rebooted anyway).
     */

    DIMINUTO_LOG_SECTION_BEGIN;

        if (hostname[0] != '\0') {
            /* Do nothing. */
        } else if (gethostname(hostname, sizeof(hostname)) < 0) {
            strncpy(hostname, "localhost", sizeof(hostname));
        } else {
            hostname[sizeof(hostname) - 1] = '\0';
        }

    DIMINUTO_LOG_SECTION_END;

    /*
     * Prepending an ISO8601 timestamp allows us to sort-merge logs from
     * different computers. However, strict ordering of events is not
     * guaranteed, even on the same computer, especially with multiprocessor
     * targets. Bracketing special fields allows us to more easily filter logs.
     *
     * yyyy-mm-ddThh:mm:ss.uuuuuuuuuZ "hostname" <priority> [pid] {tid} ...
     */

    if (now >= 0) {
        rc = snprintf(pointer, space, "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluZ \"%s\" <%s> [%lld] {%llx} ", year, month, day, hour, minute, second, (diminuto_llu_t)nanosecond, hostname, PRIORITIES[priority & 0x7], (diminuto_lld_t)getpid(), (diminuto_llu_t)pthread_self());
    } else {
        rc = snprintf(pointer, space, "****-**-**T**:**:**.*********T \"%s\" <%s> [%lld] {%llx} ", hostname, PRIORITIES[priority & 0x7], (diminuto_lld_t)getpid(), (diminuto_llu_t)pthread_self());
    }
    if (rc < 0) {
        rc = 0;
    } else if (rc >= space) {
        rc = space - 1;
    } else {
        /* Do nothing. */
    }
    pointer += rc;
    space -= rc;
    total += rc;

    rc = vsnprintf(pointer, space, format, ap);
    if (rc < 0) {
        rc = 0;
    } else if (rc >= space) {
        rc = space - 1;
    } else {
        /* Do nothing. */
    }
    pointer += rc;
    space -= rc;
    total += rc;

    if (space <= 1) {
        buffer[total - 1] = '\n';
    } else if (buffer[total - 1] != '\n') {
        buffer[total++] = '\n';
        buffer[total] = '\0';
    } else {
        /* Do nothing. */
    }

    /*
     * Serialize the emission of the log message so that processes and
     * threads can't intermingle the texts of messages. Note that if an
     * error occurs in this code block, the standard perror(3) function
     * is used to display (or not) an error message. This is to prevent
     * a recursive error condition. In the event that the error message
     * cannot be displayed (for example, there is no controlling terminal),
     * at least a counter is incremented.
     */

    DIMINUTO_LOG_SECTION_BEGIN;

        for (pointer = buffer; total > 0; total -= rc, pointer += rc) {
            rc = write(fd, pointer, total);
            if (rc == 0) {
                errno = ECOMM;
                perror("diminuto_log_vwrite");
                diminuto_log_lost += 1;
                break; /* Far end closed. */
            } else if (rc > total) {
                rc = total;
                continue; /* Should never happen. */
            } else if (rc > 0) {
                continue; /* Nominal. */
            } else if (errno == EINTR) {
                rc = 0;
                continue; /* Interrupted: retry. */
#if 0
            } else if (errno == EAGAIN) {
                rc = 0;
                continue; /* Temporary failure: retry. */
            } else if (errno == EWOULDBLOCK) {
                rc = 0;
                continue; /* Blocked: retry. */
#endif
            } else {
                perror("diminuto_log_vwrite");
                diminuto_log_lost += 1;
                break; /* Permanent failure. */
            }
        }

    DIMINUTO_LOG_SECTION_END;

}

/*******************************************************************************
 * ROUTING FUNCTIONS
 *****************************************************************************/

void diminuto_log_vlog(int priority, const char * format, va_list ap)
{
    int tolog = 0; /* <0==SUPPRESS, 0==STDERR, >0==SYSLOG */
    diminuto_fd_type_t type = DIMINUTO_FS_TYPE_UNKNOWN;

    DIMINUTO_LOG_SECTION_BEGIN;

        /*
         * Here is where we decide whether to route the log message
         * to standard error or to the system log. Once we decide to
         * route it to the system log, we never go back to logging to
         * standard error. We decide to log to the system log if we
         * are explicitly told to do so, if the caller's Process ID
         * equals its Session ID (indicating the caller is the session
         * leader, not a shell, implying the caller has no controlling
         * terminal), or if the parent of the caller has a Process ID
         * of 1 (indicating the caller's parent is the Init process, so
         * the caller is a daemon).
         */

        if (diminuto_log_strategy == DIMINUTO_LOG_STRATEGY_SUPPRESS) {
            tolog = -1;
        } else if (diminuto_log_strategy == DIMINUTO_LOG_STRATEGY_STDERR) {
            tolog = 0;
        } else if (diminuto_log_strategy == DIMINUTO_LOG_STRATEGY_SYSLOG) {
            tolog = 1;
        } else if (diminuto_log_strategy != DIMINUTO_LOG_STRATEGY_AUTOMATIC) {
            tolog = 1;
        } else if (diminuto_log_cached) {
            tolog = 1;
        } else if ((type = diminuto_fd_type(diminuto_log_descriptor)) == DIMINUTO_FS_TYPE_FILE) {
            /*
             * If the caller has redirected the log file descriptor
             * (nominally fd 2 for stderr) to a file, then they probably
             * want the log going to the file, even if the application
             * is a daemon or has no controlling terminal. Note that
             * redirecting the log to /dev/null, a common pattern when
             * deamonizing, fails this check. We don't cache this state
             * since the application can redirect the log descriptor
             * dynamically at run time.
             */
            tolog = 0;
#if 0
        } else if (type != DIMINUTO_FS_TYPE_TTY) {
            /*
             * The purpose of this check is to allow stderr of scripts
             * to be directed to a file even if they are detached from
             * their controlling terminal. A side effect is that redirecting
             * /dev/null also passes this test, and redirecting the standard
             * I/O descriptors to /dev/null is a common approach to
             * daemonization, in which case we *do* want to log to syslog.
             */
            tolog = 0;
#endif
        } else if ((diminuto_log_cached = (getpid() == getsid(0)))) {
            tolog = 1;
        } else if ((diminuto_log_cached = (getppid() == 1))) {
            tolog = 1;
        } else {
            tolog = 0;
        }

    DIMINUTO_LOG_SECTION_END;

    if (tolog < 0) {
        /* Do nothing. */
    } else if (tolog > 0) {
        diminuto_log_vsyslog(priority, format, ap);
    } else {
        diminuto_log_vwrite(diminuto_log_descriptor, priority, format, ap);
    }
}

/*******************************************************************************
 * VARIADIC FUNCTIONS
 *****************************************************************************/

void diminuto_log_syslog(int priority, const char * format, ...)
{
    int save = 0;
    va_list ap;

    save = errno;
    va_start(ap, format);
    diminuto_log_vsyslog(priority, format, ap);
    va_end(ap);
    errno = save;
}

void diminuto_log_write(int fd, int priority, const char * format, ...)
{
    int save = 0;
    va_list ap;

    save = errno;
    va_start(ap, format);
    diminuto_log_vwrite(fd, priority, format, ap);
    va_end(ap);
    errno = save;
}

void diminuto_log_log(int priority, const char * format, ...)
{
    int save = 0;
    va_list ap;

    save = errno;
    va_start(ap, format);
    diminuto_log_vlog(priority, format, ap);
    va_end(ap);
    errno = save;
}

void diminuto_log_emit(const char * format, ...)
{
    int save = 0;
    va_list ap;

    save = errno;
    va_start(ap, format);
    diminuto_log_vlog(diminuto_log_priority, format, ap);
    va_end(ap);
    errno = save;
}

/*******************************************************************************
 * ERROR NUMBER FUNCTIONS
 *****************************************************************************/

void diminuto_log_serror(const char * file, int line, const char * string)
{
    int save = 0;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM] = { '\0', };
    char * ep = (char *)0;

    if (!diminuto_log_error_suppress) {
        save = errno;
        ep = strerror_r(save, buffer, sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        diminuto_log_syslog(diminuto_log_error, "%s@%d: %s: \"%s\" (%d)\n", file, line, string, ep, save);
        errno = save;
    }
}

void diminuto_log_perror(const char * file, int line, const char * string)
{
    int save = 0;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM] = { '\0', };
    char * ep = (char *)0;

    if (!diminuto_log_error_suppress) {
        save = errno;
        ep = strerror_r(save, buffer, sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        diminuto_log_log(diminuto_log_error, "%s@%d: %s: \"%s\" (%d)\n", file, line, string, ep, save);
        errno = save;
    }
}
