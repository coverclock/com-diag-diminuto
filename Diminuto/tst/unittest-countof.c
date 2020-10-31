/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Count Of feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Count Of feature.
 *
 * I keep forgetting how multidimensional arrays are laid out in C and C++,
 * perhaps because I don't have a need to use them very often, so this is an
 * excellent place to solidify that in my mind (or use it as a reference when
 * I have a senior moment) while testing this feature.
 *
 * I found the syntax in the unit test of a pointer to a multidimensional array
 * to be remarkably counter intuitive, and was somewhat humbling for someone
 * like me who has been writing in C for a few decades. However, it reminds me
 * of the syntax for a pointer to a function, and in that respect it is
 * intimately familiar, at least in retrospect.
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_dump.h"

static char array5[2][3][5] =
{
    { {  0,  1,  2,  3,  4, }, {  5,  6,  7,  8,  9, }, { 10, 11, 12, 13, 14, } },
    { { 15, 16, 17, 18, 19, }, { 20, 21, 22, 23, 24, }, { 25, 26, 27, 28, 29, } }
};

int main(void)
{
    long array1[16];
    short array2[8];
    char array3[4];
    int array4[2][3];

    SETLOGMASK();

    {
        TEST();
        ASSERT(diminuto_countof(array1) == 16);
        ASSERT(diminuto_countof(array2) == 8);
        ASSERT(diminuto_countof(array3) == 4);
        ASSERT(countof(array1) == 16);
        ASSERT(countof(array2) == 8);
        ASSERT(countof(array3) == 4);
        ASSERT(countof(array4) == 2);
        ASSERT(countof(array4[0]) == 3);
        ASSERT(countof(array5) == 2);
        ASSERT(countof(array5[0]) == 3);
        ASSERT(countof(array5[0][0]) == 5);
        STATUS();
    }

    {
        TEST();
        diminuto_dump_bytes(stdout, array5, sizeof(array5));
        /*
         * 00602060: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f |................|
         * 00602070: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d       |..............  |
         */
        STATUS();
    }

    {
        int ii, jj, kk;
        TEST();
        for (ii = 0; ii < countof(array5); ++ii) {
            for (jj = 0; jj < countof(array5[0]); ++jj) {
                for (kk = 0; kk < countof(array5[0][0]); ++kk) {
                    ASSERT(array5[ii][jj][kk] == (ii * countof(array5[0]) * countof(array5[0][0])) + (jj * countof(array5[0][0])) + kk);
                }
            }
        }
        STATUS();
    }

    {
        int ll;
        char * pp;
        TEST();
        pp = (char *)array5;
        for (ll = 0; ll < sizeof(array5); ++ll) {
            ASSERT(pp[ll] == ll);
        }
        STATUS();
    }

    {
        char (*qq)[2][3][5];
        int ii, jj, kk;
        TEST();
        qq = &array5;
        for (ii = 0; ii < countof(array5); ++ii) {
            for (jj = 0; jj < countof(array5[0]); ++jj) {
                for (kk = 0; kk < countof(array5[0][0]); ++kk) {
                    ASSERT((*qq)[ii][jj][kk] == (ii * countof((*qq)[0]) * countof((*qq)[0][0])) + (jj * countof((*qq)[0][0])) + kk);
                }
            }
        }
        STATUS();
    }

    EXIT();
}
