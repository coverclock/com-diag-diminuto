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
                _TYPE_ mm = diminuto_bits_mask(_TYPE_); \
                _TYPE_ ol = diminuto_bits_or(_TYPE_, 0); \
                _TYPE_ oh = diminuto_bits_or(_TYPE_, diminuto_widthof(_TYPE_) - 1); \
                _TYPE_ al = diminuto_bits_and(_TYPE_, 0); \
                _TYPE_ ah = diminuto_bits_and(_TYPE_, diminuto_widthof(_TYPE_) - 1); \
                fprintf(stderr, "%sint%zu_t: offset=(%zu..%zu)\n", diminuto_issigned(_TYPE_) ? "" : "u", diminuto_widthof(_TYPE_), \
                    diminuto_bits_offset(_TYPE_, 0), diminuto_bits_offset(_TYPE_, diminuto_widthof(_TYPE_) - 1)); \
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

        ASSERT(diminuto_bits_count(uint8_t,  131) == 17);
        ASSERT(diminuto_bits_count(uint16_t, 137) == 9);
        ASSERT(diminuto_bits_count(uint32_t, 139) == 5);
        ASSERT(diminuto_bits_count(uint64_t, 149) == 3);

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            ASSERT(diminuto_bits_index(uint8_t, ii) == 0);
            ASSERT(diminuto_bits_index(uint8_t, ii * diminuto_widthof(uint8_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            ASSERT(diminuto_bits_index(uint16_t, ii) == 0);
            ASSERT(diminuto_bits_index(uint16_t, ii * diminuto_widthof(uint16_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            ASSERT(diminuto_bits_index(uint32_t, ii) == 0);
            ASSERT(diminuto_bits_index(uint32_t, ii * diminuto_widthof(uint32_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            ASSERT(diminuto_bits_index(uint64_t, ii) == 0);
            ASSERT(diminuto_bits_index(uint64_t, ii * diminuto_widthof(uint64_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            ASSERT(diminuto_bits_index(int8_t, ii) == 0);
            ASSERT(diminuto_bits_index(int8_t, ii * diminuto_widthof(int8_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            ASSERT(diminuto_bits_index(int16_t, ii) == 0);
            ASSERT(diminuto_bits_index(int16_t, ii * diminuto_widthof(int16_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            ASSERT(diminuto_bits_index(int32_t, ii) == 0);
            ASSERT(diminuto_bits_index(int32_t, ii * diminuto_widthof(int32_t)) == ii);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            ASSERT(diminuto_bits_index(int64_t, ii) == 0);
            ASSERT(diminuto_bits_index(int64_t, ii * diminuto_widthof(int64_t)) == ii);
        }

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t oo = ii % diminuto_widthof(uint8_t);
            ASSERT(diminuto_bits_offset(uint8_t, ii) == oo);
            ASSERT(diminuto_bits_offset(uint8_t, ii * diminuto_widthof(uint8_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t oo = ii % diminuto_widthof(uint16_t);
            ASSERT(diminuto_bits_offset(uint16_t, ii) == oo);
            ASSERT(diminuto_bits_offset(uint16_t, ii * diminuto_widthof(uint16_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t oo = ii % diminuto_widthof(uint32_t);
            ASSERT(diminuto_bits_offset(uint32_t, ii) == oo);
            ASSERT(diminuto_bits_offset(uint32_t, ii * diminuto_widthof(uint32_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t oo = ii % diminuto_widthof(uint64_t);
            ASSERT(diminuto_bits_offset(uint64_t, ii) == oo);
            ASSERT(diminuto_bits_offset(uint64_t, ii * diminuto_widthof(uint64_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t oo = ii % diminuto_widthof(int8_t);
            ASSERT(diminuto_bits_offset(int8_t, ii) == oo);
            ASSERT(diminuto_bits_offset(int8_t, ii * diminuto_widthof(int8_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t oo = ii % diminuto_widthof(int16_t);
            ASSERT(diminuto_bits_offset(int16_t, ii) == oo);
            ASSERT(diminuto_bits_offset(int16_t, ii * diminuto_widthof(int16_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t oo = ii % diminuto_widthof(int32_t);
            ASSERT(diminuto_bits_offset(int32_t, ii) == oo);
            ASSERT(diminuto_bits_offset(int32_t, ii * diminuto_widthof(int32_t)) == 0);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t oo = ii % diminuto_widthof(int64_t);
            ASSERT(diminuto_bits_offset(int64_t, ii) == oo);
            ASSERT(diminuto_bits_offset(int64_t, ii * diminuto_widthof(int64_t)) == 0);
        }

        STATUS();
    }

    {
        int ii;

        TEST();

        ASSERT(diminuto_bits_mask(uint8_t) == (uint8_t)0xff);
        ASSERT(diminuto_bits_mask(uint16_t) == (uint16_t)0xffff);
        ASSERT(diminuto_bits_mask(uint32_t) == (uint32_t)0xffffffff);
        ASSERT(diminuto_bits_mask(uint64_t) == (uint64_t)0xffffffffffffffffLL);

        ASSERT(diminuto_bits_mask(int8_t) == (int8_t)0xff);
        ASSERT(diminuto_bits_mask(int16_t) == (int16_t)0xffff);
        ASSERT(diminuto_bits_mask(int32_t) == (int32_t)0xffffffff);
        ASSERT(diminuto_bits_mask(int64_t) == (int64_t)0xffffffffffffffffLL);

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t mm = (uint8_t)1 << ii;
            ASSERT(diminuto_bits_or(uint8_t, ii) == mm);
            ASSERT(diminuto_bits_or(uint8_t, ii * diminuto_widthof(uint8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t mm = (uint16_t)1 << ii;
            ASSERT(diminuto_bits_or(uint16_t, ii) == mm);
            ASSERT(diminuto_bits_or(uint16_t, ii * diminuto_widthof(uint16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t mm = (uint32_t)1 << ii;
            ASSERT(diminuto_bits_or(uint32_t, ii) == mm);
            ASSERT(diminuto_bits_or(uint32_t, ii * diminuto_widthof(uint32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t mm = (uint64_t)1 << ii;
            ASSERT(diminuto_bits_or(uint64_t, ii) == mm);
            ASSERT(diminuto_bits_or(uint64_t, ii * diminuto_widthof(uint64_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t mm = ((int8_t)1) << ii;
            ASSERT(diminuto_bits_or(int8_t, ii) == mm);
            ASSERT(diminuto_bits_or(int8_t, ii * diminuto_widthof(int8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t mm = (int16_t)1 << ii;
            ASSERT(diminuto_bits_or(int16_t, ii) == mm);
            ASSERT(diminuto_bits_or(int16_t, ii * diminuto_widthof(int16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t mm = (int32_t)1 << ii;
            ASSERT(diminuto_bits_or(int32_t, ii) == mm);
            ASSERT(diminuto_bits_or(int32_t, ii * diminuto_widthof(int32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t mm = (int64_t)1 << ii;
            ASSERT(diminuto_bits_or(int64_t, ii) == mm);
            ASSERT(diminuto_bits_or(int64_t, ii * diminuto_widthof(int64_t)) == 1);
        }

        STATUS();
    }

    {
        int ii;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t mm = ~((uint8_t)1 << ii);
            ASSERT(diminuto_bits_and(uint8_t, ii) == mm);
            ASSERT(diminuto_bits_and(uint8_t, ii * diminuto_widthof(uint8_t)) == 0xfeU);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t mm = ~((uint16_t)1 << ii);
            ASSERT(diminuto_bits_and(uint16_t, ii) == mm);
            ASSERT(diminuto_bits_and(uint16_t, ii * diminuto_widthof(uint16_t)) == 0xfffeU);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t mm = ~((uint32_t)1 << ii);
            ASSERT(diminuto_bits_and(uint32_t, ii) == mm);
            ASSERT(diminuto_bits_and(uint32_t, ii * diminuto_widthof(uint32_t)) == 0xfffffffeU);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t mm = ~((uint64_t)1 << ii);
            ASSERT(diminuto_bits_and(uint64_t, ii) == mm);
            ASSERT(diminuto_bits_and(uint64_t, ii * diminuto_widthof(uint64_t)) == 0xfffffffffffffffeULL);
        }

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t mm = ~((int8_t)1 << ii);
            ASSERT(diminuto_bits_and(int8_t, ii) == mm);
            ASSERT(~diminuto_bits_and(int8_t, ii * diminuto_widthof(int8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t mm = ~((int16_t)1 << ii);
            ASSERT(diminuto_bits_and(int16_t, ii) == mm);
            ASSERT(~diminuto_bits_and(int16_t, ii * diminuto_widthof(int16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t mm = ~((int32_t)1 << ii);
            ASSERT(diminuto_bits_and(int32_t, ii) == mm);
            ASSERT(~diminuto_bits_and(int32_t, ii * diminuto_widthof(int32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t mm = ~((int64_t)1 << ii);
            ASSERT(diminuto_bits_and(int64_t, ii) == mm);
            ASSERT(~diminuto_bits_and(int64_t, ii * diminuto_widthof(int64_t)) == 1);
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
            ASSERT(diminuto_bits_get(uint32_t, ii, array) == 0);
        }

        ASSERT(array[0] == 0x00000000);
        ASSERT(array[1] == 0x00000000);
        ASSERT(array[2] == 0x00000000);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            diminuto_bits_set(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0xffffffff);
        ASSERT(array[1] == 0xffffffff);
        ASSERT(array[2] == 0xffffffff);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(diminuto_bits_get(uint32_t, ii, array) == 1);
        }

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            diminuto_bits_clear(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0x00000000);
        ASSERT(array[1] == 0x00000000);
        ASSERT(array[2] == 0x00000000);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(diminuto_bits_get(uint32_t, ii, array) == 0);
        }

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 3) {
            diminuto_bits_set(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0x49249249);
        ASSERT(array[1] == 0x92492492);
        ASSERT(array[2] == 0x24924924);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(diminuto_bits_get(uint32_t, ii, array) == (((ii % 3) == 0) ? 1 : 0 ));
        }

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 4) {
            diminuto_bits_clear(uint32_t, ii, array);
        }

        ASSERT(array[0] == 0x48248248);
        ASSERT(array[1] == 0x82482482);
        ASSERT(array[2] == 0x24824824);

        for (ii = 0; ii < (diminuto_widthof(array[0]) * diminuto_countof(array)); ii += 1) {
            ASSERT(diminuto_bits_get(uint32_t, ii, array) == (((ii % 3) != 0) ? 0 : ((ii % 4) == 0) ? 0 : 1 ));
        }

        STATUS();
    }

    EXIT();
}
