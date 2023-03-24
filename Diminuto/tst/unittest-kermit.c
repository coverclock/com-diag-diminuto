/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Kermit feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Kermit feature.
 */

#include "com/diag/diminuto/diminuto_kermit.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <ctype.h>

int main(void)
{
    SETLOGMASK();

    {
        uint8_t dd;
        unsigned char cc;
        uint8_t rr;

        TEST();

        for (dd = 0; dd <= 94; ++dd) {
            cc = diminuto_kermit_tochar(dd);
            ASSERT(cc == (dd + 32));
            ASSERT(isprint(cc));
            rr = diminuto_kermit_unchar(cc);
            ASSERT(dd == rr);
        }

        STATUS();
    }

    {
        uint16_t crc;
        unsigned char a;
        unsigned char b;
        unsigned char c;

        TEST();

        /*
         * da Cruz, p. 29, "6.3 Alternate Block Check Types"
         */

        crc = 0154321;
        a = '\0';
        b = '\0';
        c = '\0';
        diminuto_kermit_crc2chars(crc, &a, &b, &c);
        ASSERT(a == '-');
        ASSERT(b == 'C');
        ASSERT(c == '1');

        STATUS();
    }

    EXIT();
}

