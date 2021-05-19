/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COMMAND_
#define _H_COM_DIAG_DIMINUTO_COMMAND_

/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides some tools for walking the file system.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Command features provides some tools to look at the command line.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Given an argument count and argument vector, reproduce the command line
 * in the provided buffer of the specified size, NUL terminating the buffer.
 * @param argc is the argument count.
 * @param argv is the argument vector.
 * @param buffer is the buffer to populate.
 * @param size is the size of the buffer in bytes.
 * @return the number of bytes in the buffer including the terminating NUL.
 */
extern size_t diminuto_command_line(int argc, const char * argv[], void * buffer, size_t size);

#endif
