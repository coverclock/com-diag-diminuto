/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>
#include "com/diag/diminuto/diminuto_unittest.h"

int main(void)
{
    {
        TEST();
        EXPECT(sizeof(long long) >= sizeof(long));
        EXPECT(sizeof(long) >= sizeof(int));
        EXPECT(sizeof(int) >= sizeof(short));
        EXPECT(sizeof(short) >= sizeof(char));
        EXPECT(sizeof(double) >= sizeof(float));
        ADVISE(sizeof(double) > sizeof(float)); /* Not true on some targets. */
        STATUS();
    }
    {
        TEST();
        EXPECT(sizeof(int64_t) == 8);
        EXPECT(sizeof(uint64_t) == 8);
        EXPECT(sizeof(int32_t) == 4);
        EXPECT(sizeof(uint32_t) == 4);
        EXPECT(sizeof(int16_t) == 2);
        EXPECT(sizeof(uint16_t) == 2);
        EXPECT(sizeof(int8_t) == 1);
        EXPECT(sizeof(uint8_t) == 1);
        EXPECT(sizeof(intptr_t) == sizeof(void *));
        EXPECT(sizeof(uintptr_t) == sizeof(void *));
        STATUS();
    }
    EXIT();
}
