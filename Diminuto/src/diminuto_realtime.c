/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief 
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include "com/diag/diminuto/diminuto_realtime.h"
#include "com/diag/diminuto/diminuto_error.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

int diminuto_realtime_supported_path(const char * path)
{
    int result = 0;
    FILE * fp = (FILE *)0;
    int rc = EOF;
    int value = 0;

    do {

        if (path == (const char *)0) {
            errno = EINVAL;
            result = -1;
            diminuto_perror("NULL");
            break;
        }

        fp = fopen(path, "r");
        if (fp != (FILE *)0) {
            /* Do nothing. */
        } else if (errno == ENOENT) {
            break;
        } else {
            result = -1;
            diminuto_perror(path);
            break;
        }

        rc = fscanf(fp, "%d", &value);
        if (rc >= 1) {
            /* Do nothing. */
        } else if (rc == 0) {
            break;
        } else if (errno == 0) {
            break;
        } else {
            result = -1;
            diminuto_perror(path);
            break;
        }

        if (value <= 0) {
            break;
        }

        result = !0;

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if ((rc = fclose(fp)) >= 0) {
        /* Do nothing. */
    } else if (result < 0) {
        /* Do nothing. */
    } else {
        result = -1;
        diminuto_perror(path);
    }

    return result;
}
