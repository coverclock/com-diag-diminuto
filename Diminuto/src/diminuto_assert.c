/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Assert feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Assert feature.
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_stacktrace.h"
#include "com/diag/diminuto/diminuto_core.h"
#include <string.h>

/*
 * We check the condition again, even though it may already have been done
 * by the macros in the header file, so that this can be called purely
 * as a function.
 */
void diminuto_assert_f(int condition, const char * string, const char * file, int line, int error, int fatal)
{
    int save = errno;

    if (!condition) {
        diminuto_log_log(fatal ? DIMINUTO_LOG_PRIORITY_ERROR : DIMINUTO_LOG_PRIORITY_WARNING, "%s@%d: diminuto_%s(%s) FAILED! %d=\"%s\"\n", file, line, fatal ? "assert" : "expect", string, error, (error != 0) ? strerror(error) : "");
        if (fatal) {
            (void)diminuto_stacktrace();
            (void)diminuto_core_enable();
            diminuto_core_fatal();
        }
    }
    errno = save;
}
