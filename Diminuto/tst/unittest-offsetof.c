/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * If the ASSERTions fail, you've broken the feature. But if the ASSERTions
 * work and the EXPECTations fail, you've learned something interesting about
 * how your target aligns words in memory.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_offsetof.h"
#include "com/diag/diminuto/diminuto_types.h"

struct Alpha {
    int64_t sixtyfour;
    int32_t thirtytwo;
    int16_t sixteen;
    int8_t eight;
    int8_t eighty[80];
    int8_t final;
};

struct Beta {
    int8_t eight;
    int16_t sixteen;
    int32_t thirtytwo;
    int64_t sixtyfour;
    int8_t eighty[80];
    int8_t final;
};

int main(void)
{
    ASSERT(diminuto_offsetof(struct Alpha, sixtyfour) == 0);
    ASSERT(diminuto_offsetof(struct Alpha, thirtytwo) == 8);
    ASSERT(diminuto_offsetof(struct Alpha, sixteen) == 12);
    ASSERT(diminuto_offsetof(struct Alpha, eight) == 14);
    ASSERT(diminuto_offsetof(struct Alpha, eighty) == 15);
    ASSERT(diminuto_offsetof(struct Alpha, final) == 95);
    ASSERT(offsetof(struct Alpha, sixtyfour) == 0);
    ASSERT(offsetof(struct Alpha, thirtytwo) == 8);
    ASSERT(offsetof(struct Alpha, sixteen) == 12);
    ASSERT(offsetof(struct Alpha, eight) == 14);
    ASSERT(offsetof(struct Alpha, eighty) == 15);
    ASSERT(offsetof(struct Alpha, final) == 95);
    /**/
    EXPECT(diminuto_offsetof(struct Beta, eight) == 0);
    EXPECT(diminuto_offsetof(struct Beta, sixteen) == 2);
    EXPECT(diminuto_offsetof(struct Beta, thirtytwo) == 4);
    EXPECT(diminuto_offsetof(struct Beta, sixtyfour) == 8);
    EXPECT(diminuto_offsetof(struct Beta, eighty) == 16);
    EXPECT(diminuto_offsetof(struct Beta, final) == 96);
    EXPECT(offsetof(struct Beta, eight) == 0);
    EXPECT(offsetof(struct Beta, sixteen) == 2);
    EXPECT(offsetof(struct Beta, thirtytwo) == 4);
    EXPECT(offsetof(struct Beta, sixtyfour) == 8);
    EXPECT(offsetof(struct Beta, eighty) == 16);
    EXPECT(offsetof(struct Beta, final) == 96);
    /**/
    EXIT();
}
