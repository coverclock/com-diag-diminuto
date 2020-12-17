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
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_bits.h"
#include <stdint.h>

#if 0

#define diminuto_bits_index(_TYPE_, _BIT_) \
    ((_BIT_) / diminuto_widthof(_TYPE_))

#define diminuto_bits_offset(_TYPE_, _BIT_) \
    ((_BIT_) % diminuto_widthof(_TYPE_))

#define diminuto_bits_mask(_TYPE_, _BIT_) \
    ((_TYPE_)(((_TYPE_)1) << diminuto_bits_offset(_TYPE_, _BIT_)))

#define diminuto_bits_get(_ARRAY_, _TYPE_, _BIT_) \
    ((((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) & diminuto_bits_mask(_TYPE_, _BIT_)) >> diminuto_bits_offset(_TYPE_, _BIT_))

#define diminuto_bits_set(_ARRAY_, _TYPE_, _BIT_, _VALUE_) \
    (((_ARRAY_)[diminuto_bits_index(_TYPE_, _BIT)]) |= diminuto_bits_mask(_TYPE_, _BIT_))

#endif

int main(void)
{
    SETLOGMASK();

    {
        int ii = 0;

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
        int ii = 0;

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
        int ii = 0;

        TEST();

        for (ii = 0; ii < diminuto_widthof(uint8_t); ++ii) {
            uint8_t mm = (uint8_t)1 << ii;
            ASSERT(diminuto_bits_mask(uint8_t, ii) == mm);
            ASSERT(diminuto_bits_mask(uint8_t, ii * diminuto_widthof(uint8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint16_t); ++ii) {
            uint16_t mm = (uint16_t)1 << ii;
            ASSERT(diminuto_bits_mask(uint16_t, ii) == mm);
            ASSERT(diminuto_bits_mask(uint16_t, ii * diminuto_widthof(uint16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint32_t); ++ii) {
            uint32_t mm = (uint32_t)1 << ii;
            ASSERT(diminuto_bits_mask(uint32_t, ii) == mm);
            ASSERT(diminuto_bits_mask(uint32_t, ii * diminuto_widthof(uint32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(uint64_t); ++ii) {
            uint64_t mm = (uint64_t)1 << ii;
            ASSERT(diminuto_bits_mask(uint64_t, ii) == mm);
            ASSERT(diminuto_bits_mask(uint64_t, ii * diminuto_widthof(uint64_t)) == 1);
        }

        /*
         * This is probably standard behavior but it surprised me: C promots
         * the RHS test value to an int and extends the sign (appropriately),
         * but does not do the same for the value generated by the software
         * under test. So I'm doing something right in the SWUT, but am
         * unable to reproduce it in the unit test program itself.
         */

        for (ii = 0; ii < diminuto_widthof(int8_t); ++ii) {
            int8_t mm = (int8_t)1 << ii;
            ASSERT(diminuto_bits_mask(int8_t, ii) == (mm & 0xff));
            ASSERT(diminuto_bits_mask(int8_t, ii * diminuto_widthof(int8_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int16_t); ++ii) {
            int16_t mm = (int16_t)1 << ii;
            ASSERT(diminuto_bits_mask(int16_t, ii) == (mm & 0xffff));
            ASSERT(diminuto_bits_mask(int16_t, ii * diminuto_widthof(int16_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int32_t); ++ii) {
            int32_t mm = (int32_t)1 << ii;
            ASSERT(diminuto_bits_mask(int32_t, ii) == (mm & 0xffffffff));
            ASSERT(diminuto_bits_mask(int32_t, ii * diminuto_widthof(int32_t)) == 1);
        }

        for (ii = 0; ii < diminuto_widthof(int64_t); ++ii) {
            int64_t mm = (int64_t)1 << ii;
            ASSERT(diminuto_bits_mask(int64_t, ii) == (mm & 0xffffffffffffffffLL));
            ASSERT(diminuto_bits_mask(int64_t, ii * diminuto_widthof(int64_t)) == 1);
        }

        STATUS();
    }

    EXIT();
}
