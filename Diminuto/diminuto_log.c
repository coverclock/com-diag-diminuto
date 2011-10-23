/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>

diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

char * diminuto_log_ident = DIMINUTO_LOG_IDENT_DEFAULT;

int diminuto_log_option = DIMINUTO_LOG_OPTION_DEFAULT;

int diminuto_log_facility = DIMINUTO_LOG_FACILITY_DEFAULT;

static const char * levels[] = {
    "EMRG",
    "ALRT",
    "CRIT",
    "EROR",
    "WARN",
    "NOTC",
    "INFO",
    "DBUG"
};

static int initialized = 0;

void diminuto_log_vsyslog(int priority, const char * format, va_list ap)
{
    if (!initialized) {
        openlog(diminuto_log_ident, diminuto_log_option, diminuto_log_facility);
        initialized = !0;
    }
    vsyslog(priority, format, ap);
}

void diminuto_log_vlog(int priority, const char * format, va_list ap)
{
    if (getppid() != 1) {
        const char * level = "UNKN";
        level = levels[priority & 0x7];
        fprintf(stderr, "[%d] %s ", getpid(), level);
        vfprintf(stderr, format, ap);
    } else {
        diminuto_log_vsyslog(priority, format, ap);
    }
}

void diminuto_log_syslog(int priority, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vsyslog(priority, format, ap);
    va_end(ap);
}

void diminuto_log_log(int priority, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vlog(priority, format, ap);
    va_end(ap);
}

void diminuto_log_emit(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vlog(DIMINUTO_LOG_PRIORITY_DEFAULT, format, ap);
    va_end(ap);
}

void diminuto_serror(const char * s)
{
    diminuto_log_syslog(DIMINUTO_LOG_PRIORITY_ERROR, "%s: %s\n", s, strerror(errno));
}

void diminuto_perror(const char * s)
{
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "%s: %s\n", s, strerror(errno));
}
