/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Runs the specified executable with full core dumps enabled.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * coreable executablebinary
 *
 * EXAMPLES
 *
 * coreable executablebinary
 *
 * ABSTRACT
 *
 * Runs the specified executable binary and enables a full core dump to be
 * generated in the event that it fails.
 */

#include "com/diag/diminuto/diminuto_core.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char **argv) {

    do {

        if (argc < 2) {
            errno = EINVAL;
            break;
        }

        if (diminuto_core_enable() < 0) {
            break;
        }

        execvp(argv[1], &(argv[1]));
        perror("execvp");

    } while (0);

    return 1;
}
