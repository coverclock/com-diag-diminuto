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
#include "com/diag/diminuto/diminuto_widthof.h"

int main(void)
{
    {
        TEST();
        EXPECT(widthof(long long) >= widthof(long));
        EXPECT(widthof(long) >= widthof(int));
        EXPECT(widthof(int) >= widthof(short));
        EXPECT(widthof(short) >= widthof(char));
        EXPECT(widthof(double) >= widthof(float));
        ADVISE(widthof(double) > widthof(float)); /* Not true on some targets. */
        STATUS();
    }
    {
        TEST();
        EXPECT(widthof(int64_t) == 64);
        EXPECT(widthof(uint64_t) == 64);
        EXPECT(widthof(int32_t) == 32);
        EXPECT(widthof(uint32_t) == 32);
        EXPECT(widthof(int16_t) == 16);
        EXPECT(widthof(uint16_t) == 16);
        EXPECT(widthof(int8_t) == 8);
        EXPECT(widthof(uint8_t) == 8);
        EXPECT(widthof(intptr_t) == widthof(void *));
        EXPECT(widthof(uintptr_t) == widthof(void *));
        STATUS();
    }
    EXIT();
}
