/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Line feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Line feature.
 * WORK IN PROGRESS
 */

#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

/*******************************************************************************
 * OPENING, CLOSING
 ******************************************************************************/

int diminuto_line_open_generic(const char * path, const unsigned int line[], size_t lines, uint64_t flags)
{
    int result = -1;
    int rc = -1;
    int fd = -1;
    int ii = 0;
    struct gpio_v2_line_request request = { 0, };

    do {

        fd = open(path, 0);
        if (fd < 0) {
            diminuto_perror(path);
            break;
        }

        if (lines > countof(request.offsets)) {
            lines = countof(request.offsets);
        }
        for (ii = 0; ii < lines; ++ii) {
            request.offsets[ii] = line[ii];
        }
        request.num_lines = lines;

        strncpy(request.consumer, __FILE__, sizeof(request.consumer));
        request.consumer[sizeof(request.consumer) - 1] = '\0';

        request.config.flags = flags;

        rc = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &request);
        if (rc < 0) {
            diminuto_perror(path);
            break;
        }

        result = rc;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        diminuto_perror(path);
    } else {
        /* Do nothing. */
    }

    return result;
}

int diminuto_line_open(const char * path, unsigned int line, uint64_t flags)
{
    return diminuto_line_open_generic(path, &line, 1, flags);
}

int diminuto_line_close(int fd)
{
    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        diminuto_perror("diminuto_line_close");
    } else {
        fd = -1;
    }

    return fd;
}

/*******************************************************************************
 * MODIFYING
 ******************************************************************************/

int diminuto_line_get_generic(int fd, uint64_t mask, uint64_t * bitsp)
{
    int rc = -1;
    struct gpio_v2_line_values values = { 0, };

    do {

        values.mask = mask;

        rc = ioctl(fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values);
        if (rc < 0) {
            diminuto_perror("diminuto_line_get_generic");
            break;
        }

        *bitsp = values.bits;

    } while (0);

    return rc;
}

int diminuto_line_put_generic(int fd, uint64_t mask, uint64_t bits)
{
    int rc = 0;
    struct gpio_v2_line_values values = { 0, };

    values.mask = mask;
    values.bits = bits;

    rc = ioctl(fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
    if (rc < 0) {
        diminuto_perror("diminuto_line_get_generic");
    }

    return rc;
}

int diminuto_line_get(int fd, unsigned int line)
{
    int result = -1;
    int rc = -1;
    uint64_t mask = 0;
    uint64_t bits = 0;

    mask = 1ULL << line;
    rc = diminuto_line_get_generic(fd, mask, &bits);
    if (rc >= 0) {
        result = !!(bits & mask);
    }

    return result;
}

int diminuto_line_put(int fd, unsigned int line, int bit)
{
    int result = -1;
    int rc = -1;
    uint64_t mask = 0;
    uint64_t bits = 0;

    mask = 1ULL << line;
    bit = !!bit;
    bits = bit << line;
    rc = diminuto_line_put_generic(fd, mask, bits);
    if (rc >= 0) {
        result = bit;
    }

    return result;
}
