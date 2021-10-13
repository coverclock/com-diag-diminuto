/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Endianess feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Endianess feature.
 */

#include "com/diag/diminuto/diminuto_endianess.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"

int main(void)
{
    SETLOGMASK();

    {
        int littleendian;

        TEST();

        littleendian = diminuto_littleendian();

        CHECKPOINT("%s\n", littleendian ? "little-endian" : "big-endian");

        STATUS();
    }

    {
        diminuto_endianess_t endianess;
        int ii;

        TEST();

        for (ii = 0; ii < sizeof(endianess.bytes); ++ii) {
            endianess.bytes[ii] = ii;
            CHECKPOINT("bytes[%d]=0x%2.2x\n", ii, endianess.bytes[ii]);
        }

        CHECKPOINT("word=0x%*.*x\n", (int)(sizeof(endianess.bytes) * 2), (int)(sizeof(endianess.bytes) * 2), endianess.word);

        STATUS();
    }

#if !defined(__BYTE_ORDER__)
#   warning __BYTE_ORDER__ not defined!
#elif !defined(__ORDER_LITTLE_ENDIAN__)
#   warning __ORDER_LITTLE_ENDIAN__ not defined!
#elif !defined(__ORDER_BIG_ENDIAN__)
#   warning __ORDER_BIG_ENDIAN__ not defined!
#else
    {
        int littleendian;
        littleendian = diminuto_littleendian();

        TEST();

        if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
            CHECKPOINT("%s\n", "little-endian");
        }

        if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
            CHECKPOINT("%s\n", "big-endian");
        }

        EXPECT(((littleendian) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)) || ((!littleendian) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)));
        EXPECT(__ORDER_LITTLE_ENDIAN__ != __ORDER_BIG_ENDIAN__);

        STATUS();
    }
#endif

    EXIT();
}
