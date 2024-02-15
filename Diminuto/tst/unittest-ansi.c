/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the ANSI escape sequence feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the ANSI escape sequence feature.
 */

#include <stdio.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_ansi.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(void)
{
    SETLOGMASK();

    diminuto_ticks_t ticks;
    ticks = diminuto_delay_frequency();

    {
        TEST();

        ASSERT(strlen(DIMINUTO_ANSI_POSITION_CURSOR(2,3)) > 0);
        ASSERT(strlen(DIMINUTO_ANSI_ERASE_SCREEN) > 0);
        ASSERT(strlen(DIMINUTO_ANSI_ERASE_LINE) > 0);

        STATUS();
    }

    {
        TEST();

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(1,1), stdout);
        fputs(DIMINUTO_ANSI_ERASE_SCREEN, stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(1,1), stdout);
        fputs("1,1", stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(1,10), stdout);
        fputs("1,10", stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(10,1), stdout);
        fputs("10,1", stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(10,10), stdout);
        fputs("10,10", stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(1,1), stdout);
        fputs(DIMINUTO_ANSI_ERASE_LINE, stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(10,1), stdout);
        fputs(DIMINUTO_ANSI_ERASE_LINE, stdout);
        fflush(stdout);

        diminuto_delay(ticks, 0);
        fputs(DIMINUTO_ANSI_POSITION_CURSOR(1, 1), stdout);
        fflush(stdout);

        STATUS();
    }

    EXIT();
}
