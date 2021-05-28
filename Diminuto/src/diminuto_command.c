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

#include <string.h>

size_t diminuto_command_length(int argc, const char * argv[])
{
    size_t total = 0;
    int index = 0;

    for (index = 0; index < argc; ++index) {
        if (argv[index] == (const char *)0) {
            break;
        }
        if (index > 0) {
            total += 1; /* For the separating SPACE. */
        }
        total += strlen(argv[index]); /* For the argv[index]. */
    }
    total += 1; /* For the terminating NUL. */

    return total;
}

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
            if (argv[index] == (const char *)0) {
                break;
            }
            if (index <= 0) {
                /* Do nothing. */
            } else if (1 > limit) {
                break;
            } else {
                *(here++) = ' ';
                limit -= 1; /* For the separating SPACE. */
                total += 1;
            }
            length = strlen(argv[index]);
            if (length > limit) {
                break;
            }
            strncpy(here, argv[index], length);
            here += length;
            limit -= length;
            total += length;
        }
        *here = '\0';
        total += 1;
    }

    return total;
}
