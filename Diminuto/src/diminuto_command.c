/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Path feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Command feature.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <string.h>

size_t diminuto_command_line(int argc, const char * argv[], void * buffer, size_t size)
{
    size_t total = 0;
    size_t length = 0;
    size_t limit = 0;
    int index = 0;
    char * here = (char *)0;

    here = (char *)buffer;
    limit = size;

    if (limit > 0) {
        limit -= 1; /* For the terminating NUL. */
        for (index = 0; index < argc; ++index) {
            if (argv[index] == (const char *)0) { break; }
            length = strlen(argv[index]);
            if (length >= limit) { break; }
            if (index > 0) {
                *(here++) = ' ';
                --limit;
                ++total;
            }
            strncpy(here, argv[index], length);
            here += length;
            limit -= length;
            total += length;
        }
        *here = '\0';
        ++total;
    }

    return total;
}
