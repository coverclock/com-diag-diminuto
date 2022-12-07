/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the integer Min Max Of and Type Of features.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the integer Min Max Of and Type Of features.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_typeof.h"

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

    {
        int64_t int64;
        uint64_t uint64;
        int32_t int32;
        uint32_t uint32;
        int16_t int16;
        uint16_t uint16;
        int8_t int8;
        uint8_t uint8;

        TEST();

        EXPECT(issigned(typeof(int64)));
        EXPECT(!issigned(typeof(uint64)));
        EXPECT(issigned(typeof(int32)));
        EXPECT(!issigned(typeof(uint32)));
        EXPECT(issigned(typeof(int16)));
        EXPECT(!issigned(typeof(uint16)));
        EXPECT(issigned(typeof(int8)));
        EXPECT(!issigned(typeof(uint8)));

        STATUS();
    }

    {
        int64_t int64;
        uint64_t uint64;
        int32_t int32;
        uint32_t uint32;
        int16_t int16;
        uint16_t uint16;
        int8_t int8;
        uint8_t uint8;

        TEST();

        EXPECT(minimumof(typeof(int64))  == (int64_t)0x8000000000000000LL);
        EXPECT(minimumof(typeof(uint64)) == (uint64_t)0x0000000000000000ULL);
        EXPECT(minimumof(typeof(int32))  == (int32_t)0x80000000L);
        EXPECT(minimumof(typeof(uint32)) == (uint32_t)0x00000000UL);
        EXPECT(minimumof(typeof(int16))  == (int16_t)0x8000);
        EXPECT(minimumof(typeof(uint16)) == (uint16_t)0x0000U);
        EXPECT(minimumof(typeof(int8))   == (int8_t)0x80);
        EXPECT(minimumof(typeof(uint8))  == (uint8_t)0x00);

        STATUS();
    }

    {
        TEST();

        EXPECT(maximumof(typeof(int64_t))  == (int64_t)0x7fffffffffffffffLL);
        EXPECT(maximumof(typeof(uint64_t)) == (uint64_t)0xffffffffffffffffULL);
        EXPECT(maximumof(typeof(int32_t))  == (int32_t)0x7fffffffL);
        EXPECT(maximumof(typeof(uint32_t)) == (uint32_t)0xffffffffUL);
        EXPECT(maximumof(typeof(int16_t))  == (int16_t)0x7fff);
        EXPECT(maximumof(typeof(uint16_t)) == (uint16_t)0xffffU);
        EXPECT(maximumof(typeof(int8_t))   == (int8_t)0x7f);
        EXPECT(maximumof(typeof(uint8_t))  == (uint8_t)0xffU);

        STATUS();
    }

#define VERIFY(_VARIABLE_) { \
    EXPECT(issigned(typeof(_VARIABLE_)) == issigned(diminuto_typeof(_VARIABLE_))); \
    EXPECT(minimumof(typeof(_VARIABLE_)) == minimumof(diminuto_typeof(_VARIABLE_))); \
    EXPECT(maximumof(typeof(_VARIABLE_)) == maximumof(diminuto_typeof(_VARIABLE_))); \
}

    {
        int64_t int64;
        uint64_t uint64;
        int32_t int32;
        uint32_t uint32;
        int16_t int16;
        uint16_t uint16;
        int8_t int8;
        uint8_t uint8;

        /*
         * This part verifies that diminuto_typeof does the same thing as
         * typeof in GNU C or __typeof__ in ISO C, depending your flavor
         * of C compiler.
         */

        TEST();

        VERIFY(int64);
        VERIFY(uint64);
        VERIFY(int32);
        VERIFY(uint32);
        VERIFY(int16);
        VERIFY(uint16);
        VERIFY(int8);
        VERIFY(uint8);

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

#define CHARACTERIZENONINTEGER(_TYPE_) \
    { \
        COMMENT("sizeof(" #_TYPE_ ")=%zu\n", sizeof(_TYPE_)); \
        COMMENT("widthof(" #_TYPE_ ")=%zu\n", widthof(_TYPE_)); \
    }

    {
        TEST();

        CHARACTERIZE(blkcnt_t);
        CHARACTERIZE(blksize_t);
        CHARACTERIZENONINTEGER(bool);
        CHARACTERIZE(char);
        CHARACTERIZE(dev_t);
        CHARACTERIZE(diminuto_port_t);
        CHARACTERIZE(diminuto_signed_t);
        CHARACTERIZE(diminuto_sticks_t);
        CHARACTERIZE(diminuto_ticks_t);
        CHARACTERIZE(diminuto_unsigned_t);
        CHARACTERIZE(gid_t);
        CHARACTERIZE(ino_t);
        CHARACTERIZE(int);
        CHARACTERIZE(int16_t);
        CHARACTERIZE(int32_t);
        CHARACTERIZE(int64_t);
        CHARACTERIZE(int8_t);
        CHARACTERIZE(intptr_t);
        CHARACTERIZE(long);
        CHARACTERIZE(long long);
        CHARACTERIZE(mode_t);
        CHARACTERIZE(nlink_t);
        CHARACTERIZE(off_t);
        CHARACTERIZE(pid_t);
        CHARACTERIZE(pthread_t);
        CHARACTERIZE(ptrdiff_t);
        CHARACTERIZE(short);
        CHARACTERIZE(signed char);
        CHARACTERIZE(signed int);
        CHARACTERIZE(signed long);
        CHARACTERIZE(signed long long);
        CHARACTERIZE(signed short);
        CHARACTERIZE(size_t);
        CHARACTERIZE(ssize_t);
        CHARACTERIZE(time_t);
        CHARACTERIZE(uid_t);
        CHARACTERIZE(uint16_t);
        CHARACTERIZE(uint32_t);
        CHARACTERIZE(uint64_t);
        CHARACTERIZE(uint8_t);
        CHARACTERIZE(uintptr_t);
        CHARACTERIZE(unsigned char);
        CHARACTERIZE(unsigned int);
        CHARACTERIZE(unsigned long);
        CHARACTERIZE(unsigned long long);
        CHARACTERIZE(unsigned short);
        CHARACTERIZE(wchar_t);
        CHARACTERIZE(wint_t);

        STATUS();
    }

    EXIT();
}

