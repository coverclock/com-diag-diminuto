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

/*
 * We check the condition again, even though it has been done already
 * in the macros in the header file, so that this can be called purely
 * as a function.
 */

void diminuto_assert_f(int condition, const char * string, const char * file, int line, int error, int flag)
{
    if (!condition) {
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "%s@%d: CONDITION (%s) FAILED! %d=\"%s\"\n", file, line, string, error, error ? strerror(error) : "");
        if (flag) {
            abort();
        }
    }
}
