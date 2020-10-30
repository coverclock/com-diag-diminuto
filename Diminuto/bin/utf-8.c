/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Display the hex encoding and rendering of all UTF-8 characters.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Display the hex encoding and rendering of all UTF-8 characters.
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
