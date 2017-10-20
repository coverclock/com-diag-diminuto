/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
        integer.bitfield.bit = 1;

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

