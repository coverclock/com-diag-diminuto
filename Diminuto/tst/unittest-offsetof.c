/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Offset Of feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Offset Of feature.
 *
 * If the ASSERTions fail, you've broken the feature. But if the ASSERTions
 * work and the EXPECTations fail, you've learned something interesting about
 * how your target aligns words in memory.
 */

#include <stdint.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_offsetof.h"

struct Alpha {
    int64_t eight;
    int32_t four;
    int16_t two;
    int8_t one;
    int8_t eightyone[81];
    int8_t final[0]; /* Will produce -pedantic warnings. */
};

struct Beta {
    int8_t one;
    int16_t two;
    int32_t four;
    int64_t eight;
    int8_t eightyone[81];
    int8_t final[0]; /* Will produce -pedantic warnings. */
};

struct Gamma {
    int8_t eightyone[81];
    int8_t one;
    int16_t two;
    int32_t four;
    int64_t eight;
    int8_t final[0]; /* Will produce -pedantic warnings. */
};

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(diminuto_offsetof(struct Alpha, eight) == 0);
        ASSERT(diminuto_offsetof(struct Alpha, four) == 8);
        ASSERT(diminuto_offsetof(struct Alpha, two) == 12);
        ASSERT(diminuto_offsetof(struct Alpha, one) == 14);
        ASSERT(diminuto_offsetof(struct Alpha, eightyone) == 15);
        ASSERT(diminuto_offsetof(struct Alpha, final) == 96);
        ASSERT(sizeof(struct Alpha) == 96); /* Simplest strategy. */

        STATUS();
    }

    {
        TEST();

        EXPECT(diminuto_offsetof(struct Beta, one) == 0);
        EXPECT(diminuto_offsetof(struct Beta, two) == 2);
        EXPECT(diminuto_offsetof(struct Beta, four) == 4);
        EXPECT(diminuto_offsetof(struct Beta, eight) == 8);
        EXPECT(diminuto_offsetof(struct Beta, eightyone) == 16);
        EXPECT(diminuto_offsetof(struct Beta, final) == 97);
        ASSERT((sizeof(struct Beta) == 100) || (sizeof(struct Beta) == 104)); /* Consider arrays. */

        STATUS();
    }

    {
        TEST();

        EXPECT(diminuto_offsetof(struct Gamma, eightyone) == 0);
        EXPECT(diminuto_offsetof(struct Gamma, one) == 81);
        EXPECT(diminuto_offsetof(struct Gamma, two) == 82);
        EXPECT(diminuto_offsetof(struct Gamma, four) == 84);
        EXPECT(diminuto_offsetof(struct Gamma, eight) == 88);
        EXPECT(diminuto_offsetof(struct Gamma, final) == 96);
        ASSERT(sizeof(struct Gamma) == 96); /* Counter-intuitive? */

        STATUS();
    }

    {
        TEST();

        /*
         * REFERENCE
         *
         * Fe'lix Cloutier (fay59), "Quirks of C",
         * https://gist.github.com/fay59/5ccbe684e6e56a7df8815c3486568f01
         */
        
        struct foo {
            char a;
            long b: 16;
            char c;
        } bar;

        NOTIFY("_Alignof(struct foo)=%zu\n", _Alignof(struct foo));
        NOTIFY("__builtin_offsetof(struct foo, c)=%zu\n", __builtin_offsetof(struct foo, c));

        NOTIFY("offsetof(struct foo, a)=%zu\n", offsetof(struct foo, a));
        /* Cannot take sizeof() bitfield. */
        NOTIFY("offsetof(struct foo, c)=%zu\n", offsetof(struct foo, c));

        NOTIFY("sizeof(bar)=%zu\n", sizeof(bar));

        /*
         * This should worry you.
         */

        ADVISE(offsetof(struct foo, c) > 3);

        STATUS();
    }

    EXIT();
}
