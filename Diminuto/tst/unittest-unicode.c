/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Unicode characters feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Unicode characters feature.
 */

#include <locale.h>
#include <stdio.h>
#include "com/diag/diminuto/diminuto_unicode.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(void)
{
    SETLOGMASK();

    {
        int rc;
        const char * locale;

        TEST();

        rc = setenv("LC_ALL", "en_US.UTF-8", 0);
        ASSERT(rc >= 0);

        locale = setlocale(LC_ALL, "");
        ASSERT(locale != (const char *)0);

        STATUS();
    }

    {
        TEST();

        fprintf(stderr, "Degree    '%lc'\n", DIMINUTO_UNICODE_DEGREE);
        fprintf(stderr, "PlusMinus '%lc'\n", DIMINUTO_UNICODE_PLUSMINUS);

        STATUS();
    }

    EXIT();
}
