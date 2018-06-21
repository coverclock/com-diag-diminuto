/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <assert.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
    int duty;
    int on;
    int off;

    for (duty = 0; duty <= 100; ++duty) {
        on = duty;
        off = 100 - duty;
        while (((on / 7) > 0) && ((on % 7) == 0) && ((off / 7) > 0) && ((off % 7) == 0)) {
            on /= 7;
            off /= 7;
        }
        while (((on / 5) > 0) && ((on % 5) == 0) && ((off / 5) > 0) && ((off % 5) == 0)) {
            on /= 5;
            off /= 5;
        }
        while (((on / 3) > 0) && ((on % 3) == 0) && ((off / 3) > 0) && ((off % 3) == 0)) {
            on /= 3;
            off /= 3;
        }
        while (((on / 2) > 0) && ((on % 2) == 0) && ((off / 2) > 0) && ((off % 2) == 0)) {
            on /= 2;
            off /= 2;
        }
        printf("duty=%d=(%d,%d) on=%d off=%d\n", duty, duty, 100 - duty, on, off);
    }

    return 0;
}
