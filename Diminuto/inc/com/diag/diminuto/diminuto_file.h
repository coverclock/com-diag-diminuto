/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FILE_
#define _H_COM_DIAG_DIMINUTO_FILE_

/**
 * @file
 * @copyright Copyright 2019 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a horrifyingly brittle mechanism to compute bytes buffered in an input stream.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The File feature calculates how many characters are available
 * in a standard I/O FILE object by violently breaking the object's
 * encapsulation. This is probably not a good idea. It is, however,
 * remarkably useful.
 *
 * May fail to compile with other C libraries like UlibC or Bionic.
 *
 * May compile but yield the wrong result if even a minor change is made to
 * GLIBC.
 *
 * Based on a similar hack in the Digital Aggregates Grandote C++ project.
 *
 * There is no unit test for this, but there is a functional test.
 *
 * Would really be helpful if the C library had a standard way to do this.
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the number of bytes a FILE object has ready to read in its buffer.
 * @param fp points to the FILE object.
 * @return the number of bytes ready or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_ready(const FILE * fp)
{
    return (fp->_IO_read_end - fp->_IO_read_ptr);
}

/**
 * Return the number of bytes a FILE object has available to write in its buffer.
 * @param fp points to the FILE object.
 * @return the number of bytes available or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_available(const FILE * fp)
{
    return (fp->_IO_write_end - fp->_IO_write_ptr);
}

#endif
