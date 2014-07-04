/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2011 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_datum.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
	diminuto_datum_value_t value;
	diminuto_datum_width_t width;

	EXPECT(WIDTH8  == sizeof(uint8_t));
	EXPECT(WIDTH16 == sizeof(uint16_t));
	EXPECT(WIDTH32 == sizeof(uint32_t));
	EXPECT(WIDTH64 == sizeof(uint64_t));

	width = WIDTH8;  EXPECT(width == WIDTH8);
	width = WIDTH16; EXPECT(width == WIDTH16);
	width = WIDTH32; EXPECT(width == WIDTH32);
	width = WIDTH64; EXPECT(width == WIDTH64);

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

	EXPECT(DIMINUTO_DATUM_WIDTH(8)  == WIDTH8);
	EXPECT(DIMINUTO_DATUM_WIDTH(16) == WIDTH16);
	EXPECT(DIMINUTO_DATUM_WIDTH(32) == WIDTH32);
	EXPECT(DIMINUTO_DATUM_WIDTH(64) == WIDTH64);

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
