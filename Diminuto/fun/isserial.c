/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the File functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Displays whether stderr is a serial device or not.
 */

#include "com/diag/diminuto/diminuto_serial.h"
#include <stdio.h>

int main(int argc, char * argv[])
{
    if (diminuto_serial_valid(fileno(stderr))) {
        fprintf(stderr, "isserial: true\n");
        return 0;
    } else {
        fprintf(stderr, "isserial: false\n");
        return 1;
    }
}
