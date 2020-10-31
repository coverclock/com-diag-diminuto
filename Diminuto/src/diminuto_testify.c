/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Testify feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Testify feature.
 */

#include "com/diag/diminuto/diminuto_testify.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int diminuto_testify(const char * testimony, int otherwise) {
    int result = otherwise;
    int32_t value = 0;
    char * endp = (char *)0;

    if (testimony != (const char *)0) {
        switch (testimony[0]) {
        case 'a': case 'A': /* asserted, active, activated */
        case 'c': case 'C': /* closed, confirmed */
        case 'e': case 'E': /* enabled */
        case 'h': case 'H': /* high */
        case 't': case 'T': /* true */
        case 'u': case 'U': /* up */
        case 'y': case 'Y': /* yes */
            result = !0;
            break;
        case 'd': case 'D': /* disabled, deasserted, denied, down, deactivated */
        case 'f': case 'F': /* false */
        case 'i': case 'I': /* inactive */
        case 'n': case 'N': /* no */
        case 'l': case 'L': /* low */
            result = 0;
            break;
        case 'o': case 'O':
            switch (testimony[1]) {
            case 'n': case 'N': /* on */
                result = !0;
                break;
            case 'f': case 'F': /* off */
            case 'p': case 'P': /* open */
                result = 0;
                break;
            default:
                /* Do nothing. */
                break;
            }
            break;
        case '0': /* 0, 01, 0x1, etc. */
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            value = strtol(testimony, &endp, 0);
            result = (*endp == '\0') ? !!value : otherwise;
            break;
        default:
            /* Do nothing. */
            break;
        }
    }

    return result;
}
