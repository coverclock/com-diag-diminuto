/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2011 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_datum.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    diminuto_datum_value_t value;
    diminuto_datum_width_t width;

    EXPECT(DIMINUTO_DATUM_WIDTH8  == sizeof(uint8_t));
    EXPECT(DIMINUTO_DATUM_WIDTH16 == sizeof(uint16_t));
    EXPECT(DIMINUTO_DATUM_WIDTH32 == sizeof(uint32_t));
    EXPECT(DIMINUTO_DATUM_WIDTH64 == sizeof(uint64_t));

    width = DIMINUTO_DATUM_WIDTH8;  EXPECT(width == DIMINUTO_DATUM_WIDTH8);
    width = DIMINUTO_DATUM_WIDTH16; EXPECT(width == DIMINUTO_DATUM_WIDTH16);
    width = DIMINUTO_DATUM_WIDTH32; EXPECT(width == DIMINUTO_DATUM_WIDTH32);
    width = DIMINUTO_DATUM_WIDTH64; EXPECT(width == DIMINUTO_DATUM_WIDTH64);

    EXPECT(sizeof(value.value8)  == sizeof(uint8_t));
    EXPECT(sizeof(value.value16) == sizeof(uint16_t));
    EXPECT(sizeof(value.value32) == sizeof(uint32_t));
    EXPECT(sizeof(value.value64) == sizeof(uint64_t));

    value.value8  = 0xa5;                  EXPECT(value.value8  == 0xa5U);
    value.value16 = 0xa5a5;                EXPECT(value.value16 == 0xa5a5U);
    value.value32 = 0xa5a5a5a5UL;          EXPECT(value.value32 == 0xa5a5a5a5UL);
    value.value64 = 0xa5a5a5a5a5a5a5a5ULL; EXPECT(value.value64 == 0xa5a5a5a5a5a5a5a5ULL);

    EXPECT(DIMINUTO_DATUM_ALIGNMENT(8)  == 0x0);
    EXPECT(DIMINUTO_DATUM_ALIGNMENT(16) == 0x1);
    EXPECT(DIMINUTO_DATUM_ALIGNMENT(32) == 0x3);
    EXPECT(DIMINUTO_DATUM_ALIGNMENT(64) == 0x7);

    EXPECT(DIMINUTO_DATUM_WIDTH(8)  == DIMINUTO_DATUM_WIDTH8);
    EXPECT(DIMINUTO_DATUM_WIDTH(16) == DIMINUTO_DATUM_WIDTH16);
    EXPECT(DIMINUTO_DATUM_WIDTH(32) == DIMINUTO_DATUM_WIDTH32);
    EXPECT(DIMINUTO_DATUM_WIDTH(64) == DIMINUTO_DATUM_WIDTH64);

    EXPECT(sizeof(DIMINUTO_DATUM_TYPE(8))  == sizeof(uint8_t));
    EXPECT(sizeof(DIMINUTO_DATUM_TYPE(16)) == sizeof(uint16_t));
    EXPECT(sizeof(DIMINUTO_DATUM_TYPE(32)) == sizeof(uint32_t));
    EXPECT(sizeof(DIMINUTO_DATUM_TYPE(64)) == sizeof(uint64_t));

    value.value8  = 0xa5;                  EXPECT(value.DIMINUTO_DATUM_VALUE(8)  == 0xa5U);
    value.value16 = 0xa5a5;                EXPECT(value.DIMINUTO_DATUM_VALUE(16) == 0xa5a5U);
    value.value32 = 0xa5a5a5a5UL;          EXPECT(value.DIMINUTO_DATUM_VALUE(32) == 0xa5a5a5a5UL);
    value.value64 = 0xa5a5a5a5a5a5a5a5ULL; EXPECT(value.DIMINUTO_DATUM_VALUE(64) == 0xa5a5a5a5a5a5a5a5ULL);

    EXIT();
}
