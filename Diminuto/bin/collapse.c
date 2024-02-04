/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Collapses the escape sequences in an argument and emits it to stdout.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * ABSTRACT
 *
 * Collapses the escape sequences in an argument and emits it to stdout.
 *
 * USAGE
 * 
 * collapse [ ESCAPEDSEQUENCE ... ]
 *
 * EXAMPLE
 *
 * collapse 'AB\103\x44\r\n'
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_escape.h"

int main(int argc, char * argv[])
{
    int xc = 0;
    int index = 0;
    uint8_t * buffer = (uint8_t *)0;
    size_t length = 0;
    ssize_t size = 0;
    ssize_t written = 0;

    for (index = 1; index < argc; index += 1) {
        buffer = (uint8_t *)argv[index];
        if (buffer[0] == '\0') {
            errno = ENOENT;
            diminuto_perror("argv");
            continue;
        }
        length = strlen((const char *)buffer) + 1;
        size = diminuto_escape_collapse((char *)buffer, (char *)buffer, length);
        if ((size < 1) || (buffer[0] == '\0')) {
            errno = ENODATA;
            diminuto_perror("diminuto_escape_collapse");
            continue;
        }
        size -= 1;
        written = fwrite(buffer, 1, size, stdout);
        if (written < 0) {
            diminuto_perror("fwrite");
            xc = 1;
            break;
        } else if (written != size) {
            errno = EIO;
            diminuto_perror("fwrite");
            xc = 1;
fprintf(stderr, "\"%s\" %ld %ld\n", buffer, size, written);
            break;
        } else {
            /* Do nothing. */
        }
    }

    return xc;
}
