/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Log feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Log feature.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>

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

/*
 * Separate mutexen to keep from introducing incidential
 * serialization between unrelated operations in the application.
 * SEE ALSO diminuto_log_mutex in the GLOBALS section.
 */
static pthread_mutex_t mutexinit = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexopen = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexclose = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexstream = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexroute = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexhostname = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
 * GLOBALS
 *****************************************************************************/

pthread_mutex_t diminuto_log_mutex = PTHREAD_MUTEX_INITIALIZER;

diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

const char * diminuto_log_ident = DIMINUTO_LOG_IDENT_DEFAULT;

int diminuto_log_option = DIMINUTO_LOG_OPTION_DEFAULT;

int diminuto_log_facility = DIMINUTO_LOG_FACILITY_DEFAULT;

int diminuto_log_descriptor = DIMINUTO_LOG_DESCRIPTOR_DEFAULT;

FILE * diminuto_log_file = DIMINUTO_LOG_STREAM_DEFAULT;

const char * diminuto_log_mask_name = DIMINUTO_LOG_MASK_NAME_DEFAULT;

diminuto_log_strategy_t diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_AUTOMATIC;

bool diminuto_log_cached = false;

/*******************************************************************************
 * BASE FUNCTIONS
 *****************************************************************************/

diminuto_log_mask_t diminuto_log_setmask(void)
{
    const char * mask = (const char *)0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutexinit);

        if ((mask = getenv(diminuto_log_mask_name)) == (const char *)0) {
            /* Do nothing. */
        } else if (strcmp(mask, ALL) == 0) {
            DIMINUTO_LOG_MASK = DIMINUTO_LOG_MASK_ALL;
        } else {
            DIMINUTO_LOG_MASK = strtoul(mask, (char **)0, 0);
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return DIMINUTO_LOG_MASK;
}

void diminuto_log_open_syslog(const char * name, int option, int facility)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutexopen);

        if (name != (const char *)0) {
            diminuto_log_ident = name;
        }

#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
        if (!initialized) {
            openlog(diminuto_log_ident, option, facility);
            initialized = !0;
        }
#endif

        if (diminuto_log_cached) {
            /* Do nothing. */
        } else if (getpid() == getsid(0)) {
            diminuto_log_cached = true;
        } else if (getppid() == 1) {
            diminuto_log_cached = true;
        } else {
            /* Do nothing. */
        }

    DIMINUTO_CRITICAL_SECTION_END;
}

void diminuto_log_open(const char * name)
{
    diminuto_log_open_syslog(name, diminuto_log_option, diminuto_log_facility);
}

void diminuto_log_close(void)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutexclose);

#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
        if (initialized) {
            closelog();
            initialized = 0;
        }
#endif

    DIMINUTO_CRITICAL_SECTION_END;
}

FILE * diminuto_log_stream(void)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutexstream);

        if (diminuto_log_file != (FILE *)0) {
            /* Do nothing. */
        } else if ((diminuto_log_descriptor == STDOUT_FILENO) && (fileno(stdout) == STDOUT_FILENO)) {
            diminuto_log_file = stdout;
        } else if ((diminuto_log_descriptor == STDERR_FILENO) && (fileno(stderr) == STDERR_FILENO)) {
            diminuto_log_file = stderr;
        } else {
            diminuto_log_file = fdopen(diminuto_log_descriptor, "a");
        }

    DIMINUTO_CRITICAL_SECTION_END;

    return diminuto_log_file;
}

void diminuto_log_vsyslog(int priority, const char * format, va_list ap)
{
    diminuto_log_open((const char *)0);
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
    vsyslog(priority, format, ap);
#else
    __android_log_vprint(priority, diminuto_log_ident, format, ap);
#endif
}

void diminuto_log_vwrite(int fd, int priority, const char * format, va_list ap)
{
    diminuto_ticks_t now;
    int year, month, day, hour, minute, second;
    diminuto_ticks_t nanosecond;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
    int rc;
    char * pointer = buffer;
    size_t space = sizeof(buffer);
    size_t total = 0;
    char * bufferp = buffer;
    static char hostname[DIMINUTO_LOG_HOSTNAME_MAXIMUM] = { '\0', };

    now = diminuto_time_clock();
    diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &nanosecond);

    /*
     * Note that the hostname is cached; subsequent log emissions
     * will not reflect any hostname changes (which are unlikely
     * without the system being rebooted anyway).
     */

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutexhostname);

        if (hostname[0] != '\0') {
            /* Do nothing. */
        } else if (gethostname(hostname, sizeof(hostname)) < 0) {
            strncpy(hostname, "localhost", sizeof(hostname));
        } else {
            hostname[sizeof(hostname) - 1] = '\0';
        }

    DIMINUTO_CRITICAL_SECTION_END;

    /*
     * Prepending an ISO8601 timestamp allows us to sort-merge logs from
     * different computers. However, strict ordering of events is not
     * guaranteed, even on the same computer, especially with multiprocessor
     * targets. Bracketing special fields allows us to more easily filter logs.
     *
     * yyyy-mm-ddThh:mm:ss.uuuuuuuuuZ "hostname" <priority> [pid] {tid} ...
     */

    rc = snprintf(pointer, space, "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9lluZ \"%s\" <%s> [%lld] {%llx} ", year, month, day, hour, minute, second, (long long unsigned int)nanosecond, hostname, PRIORITIES[priority & 0x7], (signed long long int)getpid(), (unsigned long long int)pthread_self());
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
     * threads can't intermingle the texts of messages.
     */

    DIMINUTO_CRITICAL_SECTION_BEGIN(&diminuto_log_mutex);

        for (pointer = buffer; total > 0; total -= rc) {
            rc = write(fd, pointer, total);
            if (rc < 0) {
                break; /* What are we going to do, log an error message? */
            } else if (rc == 0) {
                break; /* Far end closed, which is the caller's problem. */
            } else if (rc > total) {
                break; /* Should never happen. */
            } else {
                pointer += rc; /* Nominal. */
            }
        }

    DIMINUTO_CRITICAL_SECTION_END;

}

/*******************************************************************************
 * ROUTING FUNCTIONS
 *****************************************************************************/

void diminuto_log_vlog(int priority, const char * format, va_list ap)
{
    int tolog = -1;

    DIMINUTO_COHERENT_SECTION_BEGIN;

        if (diminuto_log_strategy == DIMINUTO_LOG_STRATEGY_STDERR) {
            tolog = 0;
        } else if (diminuto_log_strategy == DIMINUTO_LOG_STRATEGY_SYSLOG) {
            tolog = !0;
        } else if (diminuto_log_cached) {
            tolog = !0;
        }  else {

            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutexroute);

                if ((diminuto_log_cached = (getpid() == getsid(0)))) {
                    tolog = !0;
                } else if ((diminuto_log_cached = (getppid() == 1))) {
                    tolog = !0;
                } else {
                    tolog = 0;
                }

            DIMINUTO_CRITICAL_SECTION_END;

        }

    DIMINUTO_COHERENT_SECTION_END;

    if (tolog) {
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
    int save = errno;
    va_list ap;

    va_start(ap, format);
    diminuto_log_vsyslog(priority, format, ap);
    va_end(ap);
    errno = save;
}

void diminuto_log_write(int fd, int priority, const char * format, ...)
{
    int save = errno;
    va_list ap;

    va_start(ap, format);
    diminuto_log_vwrite(fd, priority, format, ap);
    va_end(ap);
    errno = save;
}

void diminuto_log_log(int priority, const char * format, ...)
{
    int save = errno;
    va_list ap;

    va_start(ap, format);
    diminuto_log_vlog(priority, format, ap);
    va_end(ap);
    errno = save;
}

void diminuto_log_emit(const char * format, ...)
{
    int save = errno;
    va_list ap;

    va_start(ap, format);
    diminuto_log_vlog(DIMINUTO_LOG_PRIORITY_DEFAULT, format, ap);
    va_end(ap);
    errno = save;
}

/*******************************************************************************
 * ERROR NUMBER FUNCTIONS
 *****************************************************************************/

void diminuto_serror_f(const char * f, int l, const char * s)
{
    int save = errno;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
    char * ep = (char *)0;

    ep = strerror_r(save, buffer, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
    diminuto_log_syslog(DIMINUTO_LOG_PRIORITY_ERROR, "%s@%d: %s: \"%s\" (%d)\n", f, l, s, ep, save);
    errno = save;
}

void diminuto_perror_f(const char * f, int l, const char * s)
{
    int save = errno;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
    char * ep = (char *)0;

    ep = strerror_r(save, buffer, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "%s@%d: %s: \"%s\" (%d)\n", f, l, s, ep, save);
    errno = save;
}
