/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the bitfield feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the bitfield feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdint.h>

union Union {
    int32_t word;
    struct Struct {
        int bit : 1;
        int slack : 31;
    } bitfield;
} integer;

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        integer.word = 0;
        integer.bitfield.bit = 1; /* Produces -pedantic warning. Why? */

        if (integer.word == 0x80000000) { 
            DIMINUTO_LOG_INFORMATION("%s\n", DIMINUTO_LOG_HERE "high-to-low");
        } else if (integer.word == 0x00000001) {
            DIMINUTO_LOG_INFORMATION("%s\n", DIMINUTO_LOG_HERE "low-to-high");
        } else {
            DIMINUTO_LOG_INFORMATION("%s 0x%8.8x\n", DIMINUTO_LOG_HERE, integer.word);
        }

        STATUS();
    }
            
    EXIT();
}

