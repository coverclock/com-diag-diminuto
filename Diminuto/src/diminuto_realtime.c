/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This implements the API to the RT_PREEMPT Linux kernel feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This implements the API to the RT_PREEMPT Linux kernel feature.
 */

#include "com/diag/diminuto/diminuto_realtime.h"
#include "com/diag/diminuto/diminuto_error.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include "diminuto_realtime.h"

int diminuto_realtime_supported_fp(FILE * fp, const char * path)
{
    int result = 0;
    int rc = -1;
    int value = 0;

    do {

        if (fp == (FILE *)0) {
            result = -1;
            errno = EBADF;
            diminuto_perror(path);
            break;
        }

        rewind(fp);

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

    return result;
}

int diminuto_realtime_supported_path(const char * path)
{
    int result = 0;
    FILE * fp = (FILE *)0;
    int rc = -1;

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

        result = diminuto_realtime_supported_fp(fp, path);

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if ((rc = fclose(fp)) >= 0) {
        /* Do nothing. */
    } else {
        result = -1;
        diminuto_perror(path);
    }

    return result;
}

int diminuto_realtime_supported(void) {
    return diminuto_realtime_supported_path(DIMINUTO_REALTIME_PATH);
}
