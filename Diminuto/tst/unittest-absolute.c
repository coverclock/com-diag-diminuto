/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Absolute feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Absolute feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_absolute.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();
    {
        int8_t rr;
        TEST();
        EXPECT((rr=abs8(0x7f))==(int8_t)0x7f);
        EXPECT((rr=abs8(1))==(int8_t)1);
        EXPECT((rr=abs8(0))==(int8_t)0);
        EXPECT((rr=abs8(-1))==(int8_t)1);
        EXPECT((rr=abs8(0x81))==(int8_t)0x7f);
        EXPECT((rr=abs8(0x80))==(int8_t)0x80);
        STATUS();
    }
    {
        int16_t rr;
        TEST();
        EXPECT((rr=abs16(0x7fff))==(int16_t)0x7fff);
        EXPECT((rr=abs16(1))==(int16_t)1);
        EXPECT((rr=abs16(0))==(int16_t)0);
        EXPECT((rr=abs16(-1))==(int16_t)1);
        EXPECT((rr=abs16(0x8001))==(int16_t)0x7fff);
        EXPECT((rr=abs16(0x8000))==(int16_t)0x8000);
        STATUS();
    }
    {
        int32_t rr;
        TEST();
        EXPECT((rr=abs32(0x7fffffff))==(int32_t)0x7fffffff);
        EXPECT((rr=abs32(1))==(int32_t)1);
        EXPECT((rr=abs32(0))==(int32_t)0);
        EXPECT((rr=abs32(-1))==(int32_t)1);
        EXPECT((rr=abs32(0x80000001))==(int32_t)0x7fffffff);
        EXPECT((rr=abs32(0x80000000))==(int32_t)0x80000000);
        STATUS();
    }
    {
        int64_t rr;
        TEST();
        EXPECT((rr=abs64(0x7fffffffffffffff))==(int64_t)0x7fffffffffffffff);
        EXPECT((rr=abs64(1))==(int64_t)1);
        EXPECT((rr=abs64(0))==(int64_t)0);
        EXPECT((rr=abs64(-1))==(int64_t)1);
        EXPECT((rr=abs64(0x8000000000000001))==(int64_t)0x7fffffffffffffff);
        EXPECT((rr=abs64(0x8000000000000000))==(int64_t)0x8000000000000000);
        STATUS();
    }
    EXIT();
}
