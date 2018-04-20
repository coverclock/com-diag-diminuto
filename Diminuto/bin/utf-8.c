/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int ch;

    for (ch = 0; ch < 0x100; ++ch) {
        printf("0x%02x '%c'\n", ch, ch);
    }
}
