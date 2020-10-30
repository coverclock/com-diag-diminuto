/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2019 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Maybe *this* time I can finally remember how this works.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Maybe *this* time I can finally remember how this works.
 */
#include <stdio.h>
int main(void) {
    int array[3][5];
    int ii, jj, kk;
    int * pp;
    kk = 0;
    for (ii = 0; ii < 3; ++ii) {
        for (jj = 0; jj < 5; ++jj) {
            array[ii][jj] = kk;
            printf("array[%d][%d]=%d\n", ii, jj, array[ii][jj]);
            kk = kk + 1;
        }
    }
    for (pp = &array[0][0], kk = 0; kk < (3 * 5); kk = kk + 1, pp = pp + 1) {
        printf("kk=%d pp=%p *pp=%d\n", kk, (void *)pp, *pp);
    }
}
