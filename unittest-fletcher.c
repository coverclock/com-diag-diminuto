/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_fletcher.h"

int main(void)
{
    uint8_t a;
    uint8_t b;
    uint8_t buffer[4];
    void * pointer;

    pointer = diminuto_fletcher16_buffer((void *)0, 0, (uint8_t *)0, (uint8_t *)0);
    ASSERT(pointer == (void *)0);

    a = 0;
    b = 0;

    pointer = diminuto_fletcher16_buffer((void *)0, 0, &a, &b);
    ASSERT(pointer == (void *)0);
    ASSERT(a == 0);
    ASSERT(b == 0);

    buffer[0] = '\x0';
    buffer[1] = '\x1';
    buffer[2] = '\x3';
    buffer[3] = '\x5';

    pointer = diminuto_fletcher16_buffer(&(buffer[0]), 1, &a, &b);
    ASSERT(pointer == &(buffer[1]));
    ASSERT(a == 0);
    ASSERT(b == 0);

    pointer = diminuto_fletcher16_buffer(&(buffer[1]), 3, &a, &b);
    ASSERT(pointer == &(buffer[4]));
    ASSERT(a == 9);
    ASSERT(b == 14);

    a = 0;
    b = 0;

    pointer = diminuto_fletcher16_buffer(&(buffer[0]), 4, &a, &b);
    ASSERT(pointer == &(buffer[4]));
    ASSERT(a == 9);
    ASSERT(b == 14);

    EXIT();
}
