/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
