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
 *
 * WORK IN PROGRESS
 *
 * REFERENCES
 *
 * <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/driver-api/gpio>
 *
 * <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/gpio>
 */

#include <linux/gpio.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * BASE
 ******************************************************************************/

extern int diminuto_line_open_generic(const char * path, const unsigned int line[], size_t lines, uint64_t flags);

extern int diminuto_line_get_generic(int fd, uint64_t mask, uint64_t * bitsp);

extern int diminuto_line_put_generic(int fd, uint64_t mask, uint64_t bits);

extern int diminuto_line_close(int fd);

/*******************************************************************************
 * DERIVED
 ******************************************************************************/

extern int diminuto_line_open(const char * path, unsigned int line, uint64_t flags);

extern int diminuto_line_get(int fd, unsigned int line);

extern int diminuto_line_put(int fd, unsigned int line, int bit);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline int diminuto_line_set(int fd, unsigned int line)
{
    return diminuto_line_put(fd, line, !0);
}

static inline int diminuto_line_clear(int fd, unsigned int line)
{
    return diminuto_line_put(fd, line, 0);
}

#endif
