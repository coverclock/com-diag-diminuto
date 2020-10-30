/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Manipulate a GPIO pin (DEPRECATED).
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * gpiotool PIN [ VALUE ]
 *
 * ABSTRACT
 *
 * Allows simple manipulation of general purpose input/output (GPIO) pins.
 * Should probably only be run as root.
 *
 * DEPRECATED
 *
 * gpiotool is depcrecated in favor of pintool, which has a much more
 * complicated user interface. Porting Diminuto to the NVIDIA Jetson TK1 board,
 * with its 3.10 Linux kernel, revealed that the GPIO driver for that kernel
 * and on that board returns the GPIO pin to its default state (which in this
 * case was floating) when gpiotool unexports the pin. This is not so useful.
 * Prior ports, like that to the Raspberry Pi, didn't do that. Rather than
 * completely rewrite gpiotool, breaking existing uses of it, I decided to
 * deprecate it and replace it with pintool.
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    int rc = 1;
    FILE * fp = (FILE *)0;
    diminuto_unsigned_t pin;
    int state;
    diminuto_unsigned_t value;

    do {

        if (argc < 2) {
            fprintf(stderr, "usage: %s PIN [ VALUE ]\n", argv[0]);
            break;
        }

        if (*diminuto_number_unsigned(argv[1], &pin) != '\0') {
            errno = EINVAL;
            diminuto_perror(argv[1]);
            break;
        }

        if (argc != 2) {
            /* Do nothing. */
        } else if ((fp = diminuto_pin_input(pin)) == (FILE *)0) {
            break;
        } else if ((state = diminuto_pin_get(fp)) < 0) {
            break;
        } else {
            printf("%d\n", !!state);
            rc = 0;
            break;
        }

        if (*diminuto_number_unsigned(argv[2], &value) != '\0') {
            errno = EINVAL;
            diminuto_perror(argv[2]);
            rc = 1;
            break;
        }

        if (argc != 3) {
            /* Do nothing. */
        } else if ((fp = diminuto_pin_output(pin)) == (FILE *)0) {
            break;
        } else if (diminuto_pin_put(fp, !!value) < 0) {
            break;
        } else {
            rc = 0;
            break;
        }

    } while (0);

    diminuto_pin_unused(fp, pin);

    return rc;
}
