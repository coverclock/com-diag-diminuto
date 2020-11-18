/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the integer Generics feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the integer Generics feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include <stdint.h>

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        EXPECT(issigned(int64_t));
        EXPECT(!issigned(uint64_t));
        EXPECT(issigned(int32_t));
        EXPECT(!issigned(uint32_t));
        EXPECT(issigned(int16_t));
        EXPECT(!issigned(uint16_t));
        EXPECT(issigned(int8_t));
        EXPECT(!issigned(uint8_t));

        STATUS();
    }

    {
        TEST();

        EXPECT(minimumof(int64_t)  == (int64_t)0x8000000000000000LL);
        EXPECT(minimumof(uint64_t) == (uint64_t)0x0000000000000000ULL);
        EXPECT(minimumof(int32_t)  == (int32_t)0x80000000L);
        EXPECT(minimumof(uint32_t) == (uint32_t)0x00000000UL);
        EXPECT(minimumof(int16_t)  == (int16_t)0x8000);
        EXPECT(minimumof(uint16_t) == (uint16_t)0x0000U);
        EXPECT(minimumof(int8_t)   == (int8_t)0x80);
        EXPECT(minimumof(uint8_t)  == (uint8_t)0x00);

        STATUS();
    }

    {
        TEST();

        EXPECT(maximumof(int64_t)  == (int64_t)0x7fffffffffffffffLL);
        EXPECT(maximumof(uint64_t) == (uint64_t)0xffffffffffffffffULL);
        EXPECT(maximumof(int32_t)  == (int32_t)0x7fffffffL);
        EXPECT(maximumof(uint32_t) == (uint32_t)0xffffffffUL);
        EXPECT(maximumof(int16_t)  == (int16_t)0x7fff);
        EXPECT(maximumof(uint16_t) == (uint16_t)0xffffU);
        EXPECT(maximumof(int8_t)   == (int8_t)0x7f);
        EXPECT(maximumof(uint8_t)  == (uint8_t)0xffU);

        STATUS();
    }

#define CHARACTERIZE(_TYPE_) \
    { \
        COMMENT("sizeof(" #_TYPE_ ")=%zu\n", sizeof(_TYPE_)); \
        COMMENT("widthof(" #_TYPE_ ")=%zu\n", widthof(_TYPE_)); \
        COMMENT("issigned(" #_TYPE_ ")=%d\n", issigned(_TYPE_)); \
        COMMENT("minimumof(" #_TYPE_ ")=0x%llx\n", (long long unsigned)minimumof(_TYPE_)); \
        COMMENT("maximummof(" #_TYPE_ ")=0x%llx\n", (long long unsigned)maximumof(_TYPE_)); \
    }

    {
        CHARACTERIZE(char);
        CHARACTERIZE(signed char);
        CHARACTERIZE(unsigned char);
        CHARACTERIZE(short);
        CHARACTERIZE(signed short);
        CHARACTERIZE(unsigned short);
        CHARACTERIZE(int);
        CHARACTERIZE(signed int);
        CHARACTERIZE(unsigned int);
        CHARACTERIZE(long);
        CHARACTERIZE(signed long);
        CHARACTERIZE(unsigned long);
        CHARACTERIZE(long long);
        CHARACTERIZE(signed long long);
        CHARACTERIZE(unsigned long long);
        CHARACTERIZE(intptr_t);
        CHARACTERIZE(uintptr_t);
    }
    EXIT();
}

