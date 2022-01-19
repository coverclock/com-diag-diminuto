/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Path feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Path feature.
 */

#include "com/diag/diminuto/diminuto_path.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>

char * diminuto_path_scan(const char * string, const char * file)
{
    size_t length = 0;
    char * path = (char *)0;
    char * list = (char *)0;
    char * prefix;
    char * here;
    char * context;
    int size = 0;
    struct stat status;

    do {

        if (string == (const char *)0) {
            break;
        }

        if (file == (const char *)0) {
            break;
        }

        length = strlen(string) + 1; /* Include trailing NUL. */
        list = (char *)alloca(length); /* No error return. */
        strncpy(list, string, length);

        here = list;
        while ((prefix = strtok_r(here, ":", &context)) != (char *)0) {

            diminuto_path_t candidate;
            size = snprintf(candidate, sizeof(candidate), "%s/%s", prefix, file);
            if (size >= sizeof(candidate)) {
                break;
            }

            if (stat(candidate, &status) == 0) {
                path = strdup(candidate);
                break;
            }

            here = (char *)0;
        }

    } while (0);

    return path;
}

char * diminuto_path_find(const char * keyword, const char * file)
{
    char * path = (char *)0;
    const char * value;

    do {

        if (keyword == (const char *)0) {
            break;
        }

        value = getenv(keyword);

        path = diminuto_path_scan(value, file);

    } while (0);

    return path;
}
