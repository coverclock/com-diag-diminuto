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
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_typeof.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*******************************************************************************
 * PRIVATE GLOBALS
 ******************************************************************************/

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static const char * consumer = __FILE__;

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

const char * diminuto_line_consumer(const char * next)
{
    const char * prior = (const char *)0;

    if (next != (const char *)0) {
        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            prior = consumer;
            consumer = next;
        DIMINUTO_CRITICAL_SECTION_END;
    }

    return prior;
}

/*******************************************************************************
 * PARSING
 ******************************************************************************/

/*
 * e.g. "/dev/gpiochip4:-2" means device "/dev/gpiochip4, line 2, active low.
 */

char * diminuto_line_parse(char * parameter, int * linep)
{
    char * result = (char *)0;
    char * here = (char *)0;
    char * end = (char *)0;
    long line = 0;

    do {

        here = strrchr(parameter, ':');
        if (here == (char *)0) {
            errno = EINVAL;
            diminuto_perror(parameter);
            break;
        }

        *(here++) = '\0';

        line = strtol(here, &end, 10);
        if (*end != '\0') {
            errno = EINVAL;
            diminuto_perror(parameter);
            break;
        }
        if (!((minimumof(typeof(*linep)) <= line) && (line <= maximumof(typeof(*linep))))) {
            errno = ERANGE;
            diminuto_perror(parameter);
            break;
        }

        result = parameter;
        *linep = line;

    } while (0);

    return result;
}

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

        fd = open(path, O_RDWR | O_CLOEXEC);
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

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            strncpy(request.consumer, consumer, sizeof(request.consumer));
        DIMINUTO_CRITICAL_SECTION_END;
        request.consumer[sizeof(request.consumer) - 1] = '\0';

        request.config.flags = flags;

        rc = ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &request);
        if (rc < 0) {
            diminuto_perror(path);
            break;
        }

        result = request.fd;

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

int diminuto_line_open(const char * path, unsigned int line, bool input, bool output, bool low, bool rising, bool falling, bool drain, bool source, bool up, bool down)
{
    uint64_t flags = 0;

    if (input)      { flags |= DIMINUTO_LINE_FLAG_INPUT; }
    if (output)     { flags |= DIMINUTO_LINE_FLAG_OUTPUT; }
    if (low)        { flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW; }
    if (rising)     { flags |= DIMINUTO_LINE_FLAG_EDGE_RISING; }
    if (falling)    { flags |= DIMINUTO_LINE_FLAG_EDGE_FALLING; }
    if (drain)      { flags |= DIMINUTO_LINE_FLAG_OPEN_DRAIN; }
    if (source)     { flags |= DIMINUTO_LINE_FLAG_OPEN_SOURCE; }
    if (up)         { flags |= DIMINUTO_LINE_FLAG_BIAS_PULL_UP; }
    if (down)       { flags |= DIMINUTO_LINE_FLAG_BIAS_PULL_DOWN; }

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
 * ACCESSING
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

/*******************************************************************************
 * MODIFYING
 ******************************************************************************/

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

int diminuto_line_put(int fd, unsigned int line, int bit)
{
    int result = -1;
    int rc = -1;
    uint64_t mask = 0;
    uint64_t bits = 0;

    mask = 1ULL << line;
    bits = !!bit;
    bits <<= line;

    rc = diminuto_line_put_generic(fd, mask, bits);
    if (rc >= 0) {
        result = bit;
    }

    return result;
}

/*******************************************************************************
 * WATCHING
 ******************************************************************************/

int diminuto_line_query(int fd)
{
    int result = 0;
    ssize_t rc = -1;
    struct gpio_v2_line_event event = { 0, };

    do {

        rc = read(fd, &event, sizeof(event));
        if (rc < 0) {
            diminuto_perror("diminuto_line_query");
            break;
        }
        if (rc != sizeof(event)) {
            errno = EIO;
            diminuto_perror("diminuto_line_query");
            break;
        }

        switch (event.id) {
        case GPIO_V2_LINE_EVENT_RISING_EDGE:
            result = 1;
            break;
        case GPIO_V2_LINE_EVENT_FALLING_EDGE:
            result = -1;
            break;
        default:
            /* Do nothing. */
            break;
        }

    } while (0);

    return result;
}
