/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_fletcher.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(void)
{
    SETLOGMASK();

    {
        uint8_t a = 0xa5;
        uint8_t b = 0x5a;
        uint16_t c;

        TEST();

        EXPECT(a == 0xa5);
        EXPECT(b == 0x5a);

        c = diminuto_fletcher_16((const void *)0, 0, &a, &b);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "a=0x%2.2x b=0x%2.2x c=0x%4.4x\n", a, b, c);
        EXPECT(c = 0x5aa5);

        EXPECT(a == 0xa5);
        EXPECT(b == 0x5a);

        STATUS();
    }

    {
        uint8_t a = 0;
        uint8_t b = 0;
        uint16_t c;
        const uint8_t buffer[] = { 0xa5 };

        TEST();

        EXPECT(a == 0);
        EXPECT(b == 0);

        c = diminuto_fletcher_16(&buffer, sizeof(buffer), &a, &b);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "a=0x%2.2x b=0x%2.2x c=0x%4.4x\n", a, b, c);
        EXPECT(c == 0xa5a5);
 
        EXPECT(a == 0xa5);
        EXPECT(b == 0xa5);

        TEST();

        STATUS();
    }

    {
        uint8_t a = 0;
        uint8_t b = 0;
        uint16_t c;
        const uint8_t buffer[] = { 0xa5, 0x5a };

        TEST();

        EXPECT(a == 0);
        EXPECT(b == 0);

        c = diminuto_fletcher_16(&buffer, sizeof(buffer), &a, &b);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "a=0x%2.2x b=0x%2.2x c=0x%4.4x\n", a, b, c);
        EXPECT(c == 0xa500);
 
        EXPECT(a == 0x00);
        EXPECT(b == 0xa5);

        TEST();

        STATUS();
    }

    {
        uint8_t a = 0;
        uint8_t b = 0;
        uint16_t c;
        const uint8_t buffer[] = { 'q', 'w', 'e', 'r', 't', 'y' };

        TEST();

        EXPECT(a == 0);
        EXPECT(b == 0);

        c = diminuto_fletcher_16(&buffer, sizeof(buffer), &a, &b);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "a=0x%2.2x b=0x%2.2x c=0x%4.4x\n", a, b, c);
        EXPECT(c == 0x4dae);
 
        EXPECT(a == 0xae);
        EXPECT(b == 0x4d);

        TEST();

        STATUS();
    }

    EXIT();
}
