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
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_typeof.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

/**
 * Debugging flag which is not part of the public API.
 */
int diminuto_line_debug = 0;

/*******************************************************************************
 * STATICS
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
 * FINDING
 ******************************************************************************/

typedef struct State {
    const char * name;
    const char * prefix;
    size_t length;
    char * device;
    size_t size;
    diminuto_line_offset_t line;
} state_t;

static int walker(void * statep, const char * name, const char * path, size_t depth, const struct stat * statp)
{
    int result = 0;
    state_t * sp = (state_t *)0;
    int fd = -1;
    int rc = -1;
    struct gpiochip_info chip = { 0, };
    int ii = 0;

    do {

        sp = (state_t *)statep;

        if (diminuto_fs_mode2type(statp->st_mode) != DIMINUTO_FS_TYPE_CHARACTERDEV) {
            break;
        }

        if (strncmp(name, sp->prefix, sp->length) != 0) {
            break;
        }

        fd = open(path, 0);
        if (fd < 0) {
            diminuto_perror(path);
            break;
        }

        rc = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip);
        if (rc < 0) {
            diminuto_perror(path);
            break;
        }

        for (ii = 0; ii < chip.lines; ++ii) {
            struct gpio_v2_line_info line = { 0, };

            line.offset = ii;

            rc = ioctl(fd, GPIO_V2_GET_LINEINFO_IOCTL, &line);
            if (rc < 0) {
                diminuto_perror(path);
                break;
            }

            if (line.name[0] == '\0') {
                continue;
            }

            if (strcmp(line.name, sp->name) != 0) {
                continue;
            }

            strncpy(sp->device, path, sp->size);
            sp->device[sp->size - 1] = '\0';
            sp->line = ii;
            result = 1;
            break;
        }

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

const char * diminuto_line_find_generic(const char * name, const char * root, const char * prefix, char * buffer, size_t length, diminuto_line_offset_t * linep)
{
    const char * result = (const char *)0;
    state_t state = { 0, };
    char * path = (char *)0;
    int rc = -1;

    do {

        path = (char *)alloca(sizeof(diminuto_path_t));
        if (path == (char *)0) {
            diminuto_perror("diminuto_line_find_generic");
            break;
        }
        /* No free(3) necessary. */

        state.name = name;
        state.prefix = prefix;
        state.length = strlen(prefix);
        state.device = path;
        state.size = sizeof(diminuto_path_t);
        state.line = maximumof(diminuto_line_offset_t);

        rc = diminuto_fs_walk(root, &walker, &state);
        if (rc > 0) {
            path[sizeof(diminuto_path_t) - 1] = '\0';
            (void)strncpy(buffer, path, length);
            buffer[length - 1] = '\0';
            *linep = state.line;
            result = buffer;
        }

    } while (0);

    return result;
}

/*******************************************************************************
 * PARSING
 ******************************************************************************/

/*
 * e.g. "/dev/gpiochip4:-2" means device "/dev/gpiochip4, line 2, active low.
 */

const char * diminuto_line_parse(char * parameter, diminuto_line_offset_t * linep, int * invertedp)
{
    const char * result = (char *)0;
    char * here = (char *)0;
    char * end = (char *)0;
    unsigned long value = 0;
    int inverted = 0;

    do {

        if (parameter == (char *)0) {
            errno = ENODEV;
            diminuto_perror("diminuto_line_parse");
            break;
        }

        here = strrchr(parameter, ':');
        if (here == (char *)0) {
            errno = ENODATA;
            diminuto_perror(parameter);
            break;
        }

        *(here++) = '\0';

        /*
         * Because "-0" is a valid line specification.
         */

        if (*here == '\0') {
            errno = ENOENT;
            diminuto_perror(parameter);
            break;
        }

        if (*here == '-') {
            inverted = !0;
            here += 1;
        } else if (*here == '+') {
            here += 1;
        } else {
            /* Do nothing. */
        }

        value = strtoul(here, &end, 0);
        if ((end == (char *)0) || (*end != '\0')) {
            errno = EINVAL;
            diminuto_perror(parameter);
            break;
        }

        if (value > maximumof(typeof(*linep))) {
            errno = ERANGE;
            diminuto_perror(parameter);
            break;
        }

        result = parameter;
        if (linep != (diminuto_line_offset_t *)0) { *linep = value; }
        if (invertedp != (int *)0) { *invertedp = inverted; }

    } while (0);

    return result;
}

/*******************************************************************************
 * OPENING, CLOSING
 ******************************************************************************/

int diminuto_line_open_generic(const char * path, const unsigned int line[], size_t lines, uint64_t flags, uint32_t useconds)
{
    int result = -1;
    int rc = -1;
    int fd = -1;
    int ii = 0;
    struct gpio_v2_line_request request = { 0, };

    do {

        if (path == (const char *)0) {
            errno = ENODEV;
            diminuto_perror(path);
            break;
        }

        if (!((0 < lines) && (lines < countof(request.offsets)))) {
            errno = E2BIG;
            diminuto_perror(path);
            break;
        }

        for (ii = 0; ii < lines; ++ii) {
            if (line[ii] >= widthof(uint64_t)) {
                break;
            }
            request.offsets[ii] = line[ii];
        }
        if (ii != lines) {
            errno = ERANGE;
            diminuto_perror(path);
            break;
        }

        request.num_lines = lines;

        if (useconds > 0) {
            request.config.num_attrs = 1;
            for (ii = 0; ii < lines; ++ii) {
                request.config.attrs[0].mask |= 1ULL << ii;
            }
            request.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
            request.config.attrs[0].attr.debounce_period_us = useconds;
        }

        request.config.flags = flags;

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            strncpy(request.consumer, consumer, sizeof(request.consumer));
        DIMINUTO_CRITICAL_SECTION_END;
        request.consumer[sizeof(request.consumer) - 1] = '\0';

        fd = open(path, 0x0);
        if (fd < 0) {
            diminuto_perror(path);
            break;
        }

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

        if (mask == 0x0) {
            errno = ERANGE;
            diminuto_perror("diminuto_line_get_generic");
            break;
        }

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

int diminuto_line_get(int fd)
{
    int result = -1;
    int rc = -1;
    uint64_t bits = 0;

    do {

        rc = diminuto_line_get_generic(fd, 0x1, &bits);
        if (rc < 0) {
            break;
        }

        result = !!bits;

    } while (0);

    return result;
}

/*******************************************************************************
 * MODIFYING
 ******************************************************************************/

int diminuto_line_put_generic(int fd, uint64_t mask, uint64_t bits)
{
    int rc = -1;
    struct gpio_v2_line_values values = { 0, };

    do {

        if (mask == 0x0) {
            errno = ERANGE;
            diminuto_perror("diminuto_line_put_generic");
            break;
        }

        values.mask = mask;
        values.bits = bits;

        rc = ioctl(fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
        if (rc < 0) {
            diminuto_perror("diminuto_line_put_generic");
            break;
        }

    } while (0);

    return rc;
}

int diminuto_line_put(int fd, int bit)
{
    int result = -1;
    int rc = -1;
    uint64_t bits = 0;

    do {

        bits = !!bit;

        rc = diminuto_line_put_generic(fd, 0x1, bits);
        if (rc < 0) {
            break;
        }

        result = bit;

    } while (0);

    return result;
}

/*******************************************************************************
 * WATCHING
 ******************************************************************************/

int diminuto_line_read(int fd)
{
    int result = -1;
    ssize_t rc = -1;
    struct gpio_v2_line_event event = { 0, };

    do {

        rc = read(fd, &event, sizeof(event));
        if (rc < 0) {
            diminuto_perror("diminuto_line_read");
            break;
        } else if (rc == 0) {
            errno = ENODEV;
            diminuto_perror("diminuto_line_read");
            break;
        } else if (rc != sizeof(event)) {
            errno = E2BIG;
            diminuto_perror("diminuto_line_read");
            break;
        } else {
            switch (event.id) {
            case GPIO_V2_LINE_EVENT_RISING_EDGE:
                result = 1;
                break;
            case GPIO_V2_LINE_EVENT_FALLING_EDGE:
                result = 0;
                break;
            default:
                errno = ERANGE;
                diminuto_perror("diminuto_line_read");
                break;
            }
        }

    } while (0);

    return result;
}
