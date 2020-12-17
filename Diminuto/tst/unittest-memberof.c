/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Offset Of feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Member Of feature.
 */

#include <stdint.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_memberof.h"

struct Alpha {
    int64_t eight;
    int32_t four;
    int16_t two;
    int8_t one;
    int64_t * eightp;
    int32_t * fourp;
    int16_t * twop;
    int8_t * onep;
    int8_t eightyone[81];
    int8_t final[0]; /* Will produce -pedantic warnings. */
};

int main(void)
{
    SETLOGMASK();
    {
        TEST();
        ASSERT(sizeof(memberof(struct Alpha, eight)) == 8);
        ASSERT(sizeof(memberof(struct Alpha, four)) == 4);
        ASSERT(sizeof(memberof(struct Alpha, two)) == 2);
        ASSERT(sizeof(memberof(struct Alpha, one)) == 1);
        ASSERT(sizeof(*memberof(struct Alpha, eightp)) == 8);
        ASSERT(sizeof(*memberof(struct Alpha, fourp)) == 4);
        ASSERT(sizeof(*memberof(struct Alpha, twop)) == 2);
        ASSERT(sizeof(*memberof(struct Alpha, onep)) == 1);
        ASSERT(sizeof(memberof(struct Alpha, eightyone)) == 81);
        ASSERT(sizeof(memberof(struct Alpha, final)) == 0);
        STATUS();
    }
    EXIT();
}
