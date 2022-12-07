/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Bits feature.
 *
 * C99 (6.3.1.1): "If an int can represent all values of the original
 * type, the value is converted to an int; otherwise, it is converted
 * to an unsigned int. These are called the integer promotions. All
 * other types are unchanged by the integer promotions." What I don't
 * understand yet is why C promotes the RHS with sign extension but
 * not the LHS.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_bits.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <stdint.h>
#include <endian.h>
#include <stdio.h>

static inline uint8_t htobe8(uint8_t host_8bits) { return host_8bits; }

#define REMARK(_TYPE_, _HTOBEF_) \
        { \
                _TYPE_ mm = DIMINUTO_BITS_MASK(_TYPE_); \
                _TYPE_ ol = DIMINUTO_BITS_OR(_TYPE_, 0); \
                _TYPE_ oh = DIMINUTO_BITS_OR(_TYPE_, diminuto_widthof(_TYPE_) - 1); \
                _TYPE_ al = DIMINUTO_BITS_AND(_TYPE_, 0); \
                _TYPE_ ah = DIMINUTO_BITS_AND(_TYPE_, diminuto_widthof(_TYPE_) - 1); \
                fprintf(stderr, "%sint%zu_t: offset=(%zu..%zu)\n", diminuto_issigned(_TYPE_) ? "" : "u", diminuto_widthof(_TYPE_), \
                    DIMINUTO_BITS_OFFSET(_TYPE_, 0), DIMINUTO_BITS_OFFSET(_TYPE_, diminuto_widthof(_TYPE_) - 1)); \
                mm = _HTOBEF_(mm); \
                ol = _HTOBEF_(ol); \
                oh = _HTOBEF_(oh); \
                al = _HTOBEF_(al); \
                ah = _HTOBEF_(ah); \
                fprintf(stderr, "%sint%zu_t: mask:\n", diminuto_issigned(_TYPE_) ? "" : "u", diminuto_widthof(_TYPE_)); \
                diminuto_dump(stderr, &mm, sizeof(_TYPE_)); \
                fprintf(stderr, "%sint%zu_t: or:\n", diminuto_issigned(_TYPE_) ? "" : "u", diminuto_widthof(_TYPE_)); \
                diminuto_dump(stderr, &ol, sizeof(_TYPE_)); \
                diminuto_dump(stderr, &oh, sizeof(_TYPE_)); \
                fprintf(stderr, "%sint%zu_t: and:\n", diminuto_issigned(_TYPE_) ? "" : "u", diminuto_widthof(_TYPE_)); \
                diminuto_dump(stderr, &al, sizeof(_TYPE_)); \
                diminuto_dump(stderr, &ah, sizeof(_TYPE_)); \
        }

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        REMARK(uint8_t,  htobe8);
        REMARK(uint16_t, htobe16);
        REMARK(uint32_t, htobe32);
        REMARK(uint64_t, htobe64);

        REMARK(int8_t,  htobe8);
        REMARK(int16_t, htobe16);
        REMARK(int32_t, htobe32);
        REMARK(int64_t, htobe64);

        STATUS();
    }

    {
        TEST();

        ASSERT(DIMINUTO_BITS_COUNT(uint8_t,  131) == 17);
        ASSERT(DIMINUTO_BITS_COUNT(uint16_t, 137) == 9);
        ASSERT(DIMINUTO_BITS_COUNT(uint32_t, 139) == 5);
        ASSERT(DIMINUTO_BITS_COUNT(uint64_t, 149) == 3);

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(uint8_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(uint8_t, ii * diminuto_widthof(uint8_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(uint16_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(uint16_t, ii * diminuto_widthof(uint16_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(uint32_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(uint32_t, ii * diminuto_widthof(uint32_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(uint64_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(uint64_t, ii * diminuto_widthof(uint64_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(int8_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(int8_t, ii * diminuto_widthof(int8_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(int16_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(int16_t, ii * diminuto_widthof(int16_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(int32_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(int32_t, ii * diminuto_widthof(int32_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            ASSERT(DIMINUTO_BITS_INDEX(int64_t, ii) == 0);
            ASSERT(DIMINUTO_BITS_INDEX(int64_t, ii * diminuto_widthof(int64_t)) == ii);
        }

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t oo = ii % diminuto_widthof(uint8_t);
            ASSERT(DIMINUTO_BITS_OFFSET(uint8_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(uint8_t, ii * diminuto_widthof(uint8_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t oo = ii % diminuto_widthof(uint16_t);
            ASSERT(DIMINUTO_BITS_OFFSET(uint16_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(uint16_t, ii * diminuto_widthof(uint16_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t oo = ii % diminuto_widthof(uint32_t);
            ASSERT(DIMINUTO_BITS_OFFSET(uint32_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(uint32_t, ii * diminuto_widthof(uint32_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t oo = ii % diminuto_widthof(uint64_t);
            ASSERT(DIMINUTO_BITS_OFFSET(uint64_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(uint64_t, ii * diminuto_widthof(uint64_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t oo = ii % diminuto_widthof(int8_t);
            ASSERT(DIMINUTO_BITS_OFFSET(int8_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(int8_t, ii * diminuto_widthof(int8_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t oo = ii % diminuto_widthof(int16_t);
            ASSERT(DIMINUTO_BITS_OFFSET(int16_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(int16_t, ii * diminuto_widthof(int16_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t oo = ii % diminuto_widthof(int32_t);
            ASSERT(DIMINUTO_BITS_OFFSET(int32_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(int32_t, ii * diminuto_widthof(int32_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t oo = ii % diminuto_widthof(int64_t);
            ASSERT(DIMINUTO_BITS_OFFSET(int64_t, ii) == oo);
            ASSERT(DIMINUTO_BITS_OFFSET(int64_t, ii * diminuto_widthof(int64_t)) == 0);
        }

        STATUS();
    }

    {
        TEST();

        ASSERT(DIMINUTO_BITS_MASK(uint8_t) == (uint8_t)0xff);
        ASSERT(DIMINUTO_BITS_MASK(uint16_t) == (uint16_t)0xffff);
        ASSERT(DIMINUTO_BITS_MASK(uint32_t) == (uint32_t)0xffffffff);
        ASSERT(DIMINUTO_BITS_MASK(uint64_t) == (uint64_t)0xffffffffffffffffLL);

        ASSERT(DIMINUTO_BITS_MASK(int8_t) == (int8_t)0xff);
        ASSERT(DIMINUTO_BITS_MASK(int16_t) == (int16_t)0xffff);
        ASSERT(DIMINUTO_BITS_MASK(int32_t) == (int32_t)0xffffffff);
        ASSERT(DIMINUTO_BITS_MASK(int64_t) == (int64_t)0xffffffffffffffffLL);

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t mm = (uint8_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(uint8_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(uint8_t, ii * diminuto_widthof(uint8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t mm = (uint16_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(uint16_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(uint16_t, ii * diminuto_widthof(uint16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t mm = (uint32_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(uint32_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(uint32_t, ii * diminuto_widthof(uint32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t mm = (uint64_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(uint64_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(uint64_t, ii * diminuto_widthof(uint64_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t mm = ((int8_t)1) << ii;
            ASSERT(DIMINUTO_BITS_OR(int8_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(int8_t, ii * diminuto_widthof(int8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t mm = (int16_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(int16_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(int16_t, ii * diminuto_widthof(int16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t mm = (int32_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(int32_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(int32_t, ii * diminuto_widthof(int32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t mm = (int64_t)1 << ii;
            ASSERT(DIMINUTO_BITS_OR(int64_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_OR(int64_t, ii * diminuto_widthof(int64_t)) == 1);
        }

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t mm = ~((uint8_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(uint8_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_AND(uint8_t, ii * diminuto_widthof(uint8_t)) == 0xfeU);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t mm = ~((uint16_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(uint16_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_AND(uint16_t, ii * diminuto_widthof(uint16_t)) == 0xfffeU);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t mm = ~((uint32_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(uint32_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_AND(uint32_t, ii * diminuto_widthof(uint32_t)) == 0xfffffffeU);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t mm = ~((uint64_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(uint64_t, ii) == mm);
            ASSERT(DIMINUTO_BITS_AND(uint64_t, ii * diminuto_widthof(uint64_t)) == 0xfffffffffffffffeULL);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t mm = ~((int8_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(int8_t, ii) == mm);
            ASSERT(~DIMINUTO_BITS_AND(int8_t, ii * diminuto_widthof(int8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t mm = ~((int16_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(int16_t, ii) == mm);
            ASSERT(~DIMINUTO_BITS_AND(int16_t, ii * diminuto_widthof(int16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t mm = ~((int32_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(int32_t, ii) == mm);
            ASSERT(~DIMINUTO_BITS_AND(int32_t, ii * diminuto_widthof(int32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t mm = ~((int64_t)1 << ii);
            ASSERT(DIMINUTO_BITS_AND(int64_t, ii) == mm);
            ASSERT(~DIMINUTO_BITS_AND(int64_t, ii * diminuto_widthof(int64_t)) == 1);
        }

        STATUS();
    }

    {
        uint32_t array[3] = { 0, };
        int ii;

        TEST();

        ASSERT(array[0] == 0x00000000);
        ASSERT(array[1] == 0x00000000);
        ASSERT(array[2] == 0x00000000);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(DIMINUTO_BITS_GET(uint32_t, ii, array) == 0);
        }

        ASSERT(array[0] == 0x00000000);
        ASSERT(array[1] == 0x00000000);
        ASSERT(array[2] == 0x00000000);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            DIMINUTO_BITS_SET(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0xffffffff);
        ASSERT(array[1] == 0xffffffff);
        ASSERT(array[2] == 0xffffffff);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(DIMINUTO_BITS_GET(uint32_t, ii, array) == 1);
        }

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            DIMINUTO_BITS_CLEAR(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0x00000000);
        ASSERT(array[1] == 0x00000000);
        ASSERT(array[2] == 0x00000000);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(DIMINUTO_BITS_GET(uint32_t, ii, array) == 0);
        }

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 3) {
            DIMINUTO_BITS_SET(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0x49249249);
        ASSERT(array[1] == 0x92492492);
        ASSERT(array[2] == 0x24924924);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(DIMINUTO_BITS_GET(uint32_t, ii, array) == (((ii % 3) == 0) ? 1 : 0 ));
        }

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 4) {
            DIMINUTO_BITS_CLEAR(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0x48248248);
        ASSERT(array[1] == 0x82482482);
        ASSERT(array[2] == 0x24824824);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(DIMINUTO_BITS_GET(uint32_t, ii, array) == (((ii % 3) != 0) ? 0 : ((ii % 4) == 0) ? 0 : 1 ));
        }

        STATUS();
    }

    EXIT();
}
