/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Stack Trace feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Stack Trace feature.
 */

#include "com/diag/diminuto/diminuto_stacktrace.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include "com/diag/diminuto/diminuto_error.h"
#include <errno.h>
#include <unistd.h>

#if 0

#   warning backtrace(3) not implemented on this platform!

int diminuto_stacktrace_fd(void ** buffer, size_t size, int fd)
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace_fd");

    return -1;
}

int diminuto_stacktrace()
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace");

    return -1;
}

#elif defined(COM_DIAG_DIMINUTO_PLATFORM_UCLIBC)

#   warning backtrace(3) not implemented in uClibc!

int diminuto_stacktrace_fd(void ** buffer, size_t size, int fd)
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace_fd");

    return -1;
}

int diminuto_stacktrace()
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace");

    return -1;
}

#elif defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)

#   warning backtrace(3) not implemented in Bionic!

int diminuto_stacktrace_fd(void ** buffer, size_t size, int fd)
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace_fd");

    return -1;
}

int diminuto_stacktrace()
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace");

    return -1;
}

#elif defined(COM_DIAG_DIMINUTO_PLATFORM_CYGWIN)

#   warning backtrace(3) not implemented in Cygwin!

int diminuto_stacktrace_fd(void ** buffer, size_t size, int fd)
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace_fd");

    return -1;
}

int diminuto_stacktrace()
{
    errno = EPERM;
    diminuto_perror("diminuto_stacktace");

    return -1;
}

#else

#include <execinfo.h>

int diminuto_stacktrace_fd(void ** buffer, size_t size, int fd)
{
    int rc = 0;

    rc = backtrace(buffer, size);
    if ((0 <= rc) && (rc < size)) {
        /* Do nothing. */
    } else {
        if (rc >= size) { errno = E2BIG; }
        diminuto_perror("diminuto_stacktace_fd: backtrace");
    }
    backtrace_symbols_fd(buffer, rc, fd);

    return rc;
}

int diminuto_stacktrace()
{
    void * buffer[DIMINUTO_STACKTRACE_SIZE];

    return diminuto_stacktrace_fd(buffer, countof(buffer), STDERR_FILENO);
}

#endif
