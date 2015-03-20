/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
    int8_t final[0];
};

struct Beta {
    int8_t one;
    int16_t two;
    int32_t four;
    int64_t eight;
    int8_t eightyone[81];
    int8_t final[0];
};

struct Gamma {
    int8_t eightyone[81];
    int8_t one;
    int16_t two;
    int32_t four;
    int64_t eight;
    int8_t final[0];
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
        ASSERT(sizeof(struct Beta) == 104); /* Consider arrays. */
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
    EXIT();
}
