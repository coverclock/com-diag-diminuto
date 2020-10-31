/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2009 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Number feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Number feature.
 */

#include "com/diag/diminuto/diminuto_number.h"
#include <stdlib.h>
#include <errno.h>

const char * diminuto_number_unsigned(const char * s, diminuto_unsigned_t * p)
{
    diminuto_unsigned_t n;
    char * e;

    n = strtoull(s, &e, 0);

    if (p != (diminuto_unsigned_t *)0) { *p = n; }
    if (*e != '\0') { errno = EINVAL; }

    return e;
}

const char * diminuto_number_signed(const char * s, diminuto_signed_t * p)
{
    diminuto_signed_t n;
    char * e;

    n = strtoll(s, &e, 0);

    if (p != (diminuto_signed_t *)0) { *p = n; }
    if (*e != '\0') { errno = EINVAL; }

    return e;
}
