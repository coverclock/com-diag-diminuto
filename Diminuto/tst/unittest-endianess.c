/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_endianess.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>

int main(void)
{
    int littleendian;
    SETLOGMASK();
    {
        littleendian = diminuto_littleendian();
        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "%s\n", littleendian ? "little-endian" : "big-endian");
    }
    {
#if !defined(__BYTE_ORDER__)
#   warning __BYTE_ORDER__ not defined!
#elif !defined(__ORDER_LITTLE_ENDIAN__)
#   warning __ORDER_LITTLE_ENDIAN__ not defined!
#elif !defined(__ORDER_BIG_ENDIAN__)
#   warning __ORDER_BIG_ENDIAN__ not defined!
#else
        if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
            DIMINUTO_LOG_INFORMATION("%s\n", DIMINUTO_LOG_HERE "little-endian");
        }
        if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
            DIMINUTO_LOG_INFORMATION("%s\n", DIMINUTO_LOG_HERE "big-endian");
        }
        ASSERT(((littleendian) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))||((!littleendian) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)));
        ASSERT(__ORDER_LITTLE_ENDIAN__ != __ORDER_BIG_ENDIAN__);
    }
#endif
    {
        diminuto_endianess_t endianess;
        int ii;
        for (ii = 0; ii < sizeof(endianess.bytes); ++ii) {
            endianess.bytes[ii] = ii;
            DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "bytes[%d]=0x%2.2x\n", ii, endianess.bytes[ii]);
        }
        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "word=0x%*.*x\n", sizeof(endianess.bytes) * 2, sizeof(endianess.bytes) * 2, endianess.word);
    }
    EXIT();
}
