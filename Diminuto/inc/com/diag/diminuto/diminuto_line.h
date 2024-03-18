/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LINE_
#define _H_COM_DIAG_DIMINUTO_LINE_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides an interface to handle GPIO pins via the ioctl ABI.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * WORK IN PROGRESS
 *
 * This implementation is based on the Linux kernel's GPIO character device
 * drivers and their ABI.
 *
 * Why not use libgpiod? Maybe I should have. But that library needs to be
 * all things to all people, while what I need is just the stuff I need. Also,
 * after canvassing all of my various systems on which older versions of the
 * Diminuto Library runs, which include Raspberry Pi 2s, 3s, 4s, a Raspberry
 * Pi 5, an Orange Pi, two RISC-V SBCs, and a couple of Intel systems, I
 * found that all but the oldest ones have /usr/include/linux/gpio.h. while
 * the availability of libgpiod may be problematic.
 *
 * REFERENCES
 *
 * <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/driver-api/gpio>
 *
 * <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/gpio>
 */

#include <linux/gpio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef enum DiminutoLineFlag {
	DIMINUTO_LINE_FLAG_ACTIVE_LOW =         GPIO_V2_LINE_FLAG_ACTIVE_LOW,
	DIMINUTO_LINE_FLAG_INPUT =              GPIO_V2_LINE_FLAG_INPUT,
	DIMINUTO_LINE_FLAG_OUTPUT =             GPIO_V2_LINE_FLAG_OUTPUT,
	DIMINUTO_LINE_FLAG_EDGE_RISING =        GPIO_V2_LINE_FLAG_EDGE_RISING,
	DIMINUTO_LINE_FLAG_EDGE_FALLING =       GPIO_V2_LINE_FLAG_EDGE_FALLING,
	DIMINUTO_LINE_FLAG_OPEN_DRAIN =         GPIO_V2_LINE_FLAG_OPEN_DRAIN,
	DIMINUTO_LINE_FLAG_OPEN_SOURCE =        GPIO_V2_LINE_FLAG_OPEN_SOURCE,
	DIMINUTO_LINE_FLAG_BIAS_PULL_UP =       GPIO_V2_LINE_FLAG_BIAS_PULL_UP,
	DIMINUTO_LINE_FLAG_BIAS_PULL_DOWN =     GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN,
} diminuto_line_flag_t;

typedef unsigned int diminuto_line_offset_t;

typedef uint64_t diminuto_line_bits_t;

typedef uint32_t diminuto_line_duration_t;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

static const char DIMINUTO_LINE_ROOT[] = "/dev";

static const char DIMINUTO_LINE_PREFIX[] = "gpiochip";

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

extern const char * diminuto_line_consumer(const char * next);

/*******************************************************************************
 * FINDING
 ******************************************************************************/

extern const char * diminuto_line_find_generic(const char * name, const char * root, const char * prefix, char * buffer, size_t size, diminuto_line_offset_t * linep);

static inline const char * diminuto_line_find(const char * name, char * buffer, size_t length, diminuto_line_offset_t * linep) {
    return diminuto_line_find_generic(name, DIMINUTO_LINE_ROOT, DIMINUTO_LINE_PREFIX, buffer, length, linep);
}

/*******************************************************************************
 * PARSING
 ******************************************************************************/

/*
 * e.g. "/dev/gpiochip4:-15" == /dev/gpiochip4 line 15 inverted.
 */

extern const char * diminuto_line_parse(const char * parameter, char * buffer, size_t size, diminuto_line_offset_t * linep, int * invertedp);

/*******************************************************************************
 * GENERICS
 ******************************************************************************/

extern int diminuto_line_open_generic(const char * path, const diminuto_line_offset_t line[], size_t lines, diminuto_line_bits_t flags, diminuto_line_duration_t useconds);

extern int diminuto_line_get_generic(int fd, diminuto_line_bits_t mask, diminuto_line_bits_t * bitsp);

extern int diminuto_line_put_generic(int fd, diminuto_line_bits_t mask, diminuto_line_bits_t bits);

extern int diminuto_line_close(int fd);

/*******************************************************************************
 * MUTIPLEXING
 ******************************************************************************/

static inline int diminuto_line_open_read(const char * path, diminuto_line_offset_t line, diminuto_line_bits_t flags, diminuto_line_duration_t useconds) {
    return diminuto_line_open_generic(path, &line, 1, flags, useconds);
}

extern int diminuto_line_read(int fd);

/*******************************************************************************
 * POLLING
 ******************************************************************************/

static inline int diminuto_line_open(const char * path, diminuto_line_offset_t line, diminuto_line_bits_t flags) {
    return diminuto_line_open_read(path, line, flags, 0);
}

extern int diminuto_line_get(int fd);

extern int diminuto_line_put(int fd, int bit);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline int diminuto_line_set(int fd)
{
    return diminuto_line_put(fd, !0);
}

static inline int diminuto_line_clear(int fd)
{
    return diminuto_line_put(fd, 0);
}

/*******************************************************************************
 * EXAMPLES
 ******************************************************************************/

static inline int diminuto_line_open_input(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_INPUT);
}

static inline int diminuto_line_open_input_inverted(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_INPUT | DIMINUTO_LINE_FLAG_ACTIVE_LOW);
}

static inline int diminuto_line_open_output(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_OUTPUT);
}

static inline int diminuto_line_open_output_inverted(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_OUTPUT | DIMINUTO_LINE_FLAG_ACTIVE_LOW);
}

#endif
