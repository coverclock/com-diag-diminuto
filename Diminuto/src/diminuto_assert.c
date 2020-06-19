/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <stdlib.h>
#include <string.h>

void diminuto_assert_f(int cond, const char * string, const char * file, int line, int error, int code)
{
    if (!cond) {
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "%s@%d: ASSERT (%s) FAILED! [%d] \"%s\" {%d}\n", file, line, string, error, error ? strerror(error) : "", code);
        if (code) {
            exit(code);
        }
    }
}
