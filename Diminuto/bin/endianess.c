/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Display the endianess of the host processor.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coveclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * endianess
 *
 * EXAMPLES
 *
 * endianess
 *
 * ABSTRACT
 *
 * Display the endianess of the host processor. Either "little" or "big" will
 * be emitted to standard out.
 */

#include "com/diag/diminuto/diminuto_endianess.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int xc = 0;
    int littleendian = 0;
    static const int LITTLEENDIAN = 1;

    littleendian = diminuto_littleendian();

    if (puts(littleendian ? "little" : "big") != EOF) {
        /* Do nothing. */
    } else if (ferror(stdout)) {
        errno = EIO;
        diminuto_perror("endian: puts stdout error");
        xc = 1;
    } else if (feof(stdout)) {
        errno = EIO;
        diminuto_perror("endian: puts stdout EOF");
        xc = 2;
    } else {
        errno = EIO;
        diminuto_perror("endian: puts stdout failed");
        xc = 3;
    }

    if (fflush(stdout) == EOF) {
        diminuto_perror("endian: fflush");
        xc = 4;
    }

    /*
     * Curious.
     */

    diminuto_expect(littleendian == *((char *)(&LITTLEENDIAN)));

    return xc;
}
