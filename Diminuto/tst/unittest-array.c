/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This test reminds me how arrays are ordered in C.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This test only exists to remind me how rows and columns are ordered
 * in C. I use multi-dimensional arrays so seldom I always have to figure
 * it out again.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>

int main() {

    SETLOGMASK();

    {
        static const char * ARRAY[3][3] = {
            {
                "[0,0]", "[0,1]", "[0,2]",
            },
            {
                "[1,0]", "[1,1]", "[1,2]",
            },
            {
                "[2,0]", "[2,1]", "[2,2]",
            },
        };
        int ii;
        int jj;
        int kk;
        char ** pp;

        TEST();

        for (ii = 0; ii < 3; ++ii) {
            for (jj = 0; jj < 3; ++jj) {
                printf("ARRAY[%d][%d]=\"%s\"\n", ii, jj, ARRAY[ii][jj]);
            }
        }

        for (kk = 0, pp = (char **)(&ARRAY); kk < 9; ++kk, ++pp) {
            printf("ARRAY[%d]=\"%s\"\n", kk, *pp);
        }

        STATUS();
    }

    EXIT();
}
