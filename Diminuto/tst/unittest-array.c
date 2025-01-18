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
#include <string.h>

int main() {

    SETLOGMASK();
    char buffer[sizeof("[X,X,X]")];

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
        int nn;
        char ** pp;

        TEST();

        for (ii = 0; ii < 3; ++ii) {
            for (jj = 0; jj < 3; ++jj) {
                CHECKPOINT("ARRAY[%d][%d]=\"%s\"\n", ii, jj, ARRAY[ii][jj]);
                snprintf(buffer, sizeof(buffer), "[%d,%d]", ii, jj);
                buffer[sizeof(buffer) - 1] = '\0';
                EXPECT(strcmp(buffer, ARRAY[ii][jj]) == 0);
            }
        }

        pp = (char **)(&ARRAY);
        for (nn = 0; nn < (3 * 3); ++nn) {
            CHECKPOINT("ARRAY[%d]=\"%s\"\n", nn, *(pp++));
        }

        STATUS();
    }

    {
        static const char * ARRAY[3][3][3] = {
            {
                {
                    "[0,0,0]", "[0,0,1]", "[0,0,2]",
                },
                {
                    "[0,1,0]", "[0,1,1]", "[0,1,2]",
                },
                {
                    "[0,2,0]", "[0,2,1]", "[0,2,2]",
                },
            },
            {
                {
                    "[1,0,0]", "[1,0,1]", "[1,0,2]",
                },
                {
                    "[1,1,0]", "[1,1,1]", "[1,1,2]",
                },
                {
                    "[1,2,0]", "[1,2,1]", "[1,2,2]",
                },
            },
            {
                {
                    "[2,0,0]", "[2,0,1]", "[2,0,2]",
                },
                {
                    "[2,1,0]", "[2,1,1]", "[2,1,2]",
                },
                {
                    "[2,2,0]", "[2,2,1]", "[2,2,2]",
                },
            },
        };
        int ii;
        int jj;
        int kk;
        int nn;
        char ** pp;

        TEST();

        for (ii = 0; ii < 3; ++ii) {
            for (jj = 0; jj < 3; ++jj) {
                for (kk = 0; kk < 3; ++kk) {
                    CHECKPOINT("ARRAY[%d][%d][%d]=\"%s\"\n", ii, jj, kk, ARRAY[ii][jj][kk]);
                    snprintf(buffer, sizeof(buffer), "[%d,%d,%d]", ii, jj, kk);
                    buffer[sizeof(buffer) - 1] = '\0';
                    EXPECT(strcmp(buffer, ARRAY[ii][jj][kk]) == 0);
                }
            }
        }

        pp = (char **)(&ARRAY);
        for (nn = 0; nn < (3 * 3 * 3); ++nn) {
            CHECKPOINT("ARRAY[%d]=\"%s\"\n", nn, *(pp++));
        }

        STATUS();
    }

    EXIT();
}
