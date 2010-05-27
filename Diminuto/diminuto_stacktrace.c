/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_stacktrace.h"
#include <unistd.h>
#include <execinfo.h>

int diminuto_stacktrace_generic(void ** buffer, size_t size, int fd)
{
    int rc;
    backtrace_symbols_fd(buffer, rc = backtrace(buffer, size), fd);
    return rc;
}

void diminuto_stacktrace()
{
    void * buffer[DIMINUTO_STACKTRACE_SIZE];
    (void)diminuto_stacktrace_generic(buffer, sizeof(buffer) / sizeof(buffer[0]), STDERR_FILENO);
}
