/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FILE_
#define _H_COM_DIAG_DIMINUTO_FILE_

/**
 * @file
 * @copyright Copyright 2019-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a horrifyingly brittle mechanism to compute octets buffered in an input stream.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The File feature calculates how many characters are available
 * in a standard I/O FILE object by violently breaking the object's
 * encapsulation. This is probably not a good idea. It is, however,
 * remarkably useful.
 *
 * May fail to compile with other C libraries like UlibC or Bionic.
 * May compile but yield the wrong result if even a minor change is made to
 * GLIBC.
 * Based on a similar hack in the Digital Aggregates Grandote C++ project.
 * Would really be helpful if the C library had a standard way to do this.
 *
 * REFERENCES
 *
 * stdio(3)
 *
 * stdio_ext(3)
 *
 * pipe(7)
 *
 * /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h
 *
 * https://sourceware.org/git/glibc.git:libio/getc.c
 *
 * https://sourceware.org/git/glibc.git:libio/libio.h
 *
 * https://sourceware.org/git/glibc.git:libio/genops.c
 */

#include <stdio.h>
#include <stddef.h>

/**
 * Return the number of octets a FILE object has ready to read in its buffer.
 * I'm guessing that like the write buffer, the read buffer is allocated on
 * its first use. But in this case, returning zero is the right answer
 * anyway. The result after the FILE object has been closed is unpredictable.
 * @param fp points to the FILE object.
 * @return the number of octets ready or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_ready(const FILE * fp)
{
    return (fp->_IO_read_end - fp->_IO_read_ptr);
}

/**
 * Return the number of octets a FILE object has empty for write in its buffer.
 * The standard I/O library apparently only allocates the write buffer on its
 * first use. Hence, before that, this function returns zero, and after that
 * it returns the empty space in the newly allocated write buffer. The result
 * after the FILE object has been closed is unpredictable.
 * @param fp points to the FILE object.
 * @return the number of octets empty or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_empty(const FILE * fp)
{
    return (fp->_IO_write_end - fp->_IO_write_ptr);
}

/**
 * Return the size in octets of the read buffer in an open FILE object.
 * The standard I/O apparently only allocates the read buffer on its
 * first use. 
 * @param fp points to the FILE object.
 * @return the size of the read buffe in octets or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_readsize(const FILE * fp)
{
    return (fp->_IO_read_end - fp->_IO_read_base);
}

/**
 * Return the size in octets of the write buffer in an open FILE object.
 * The standard I/O apparently only allocates the write buffer on its
 * first use. 
 * @param fp points to the FILE object.
 * @return the size of write buffer in octets or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_writesize(const FILE * fp)
{
    return (fp->_IO_write_end - fp->_IO_write_base);
}

/**
 * These are the different I/O methods that the poll function may use.
 */
typedef enum DiminutoFileMethod {
    DIMINUTO_FILE_METHOD_IGNORE = '!',
    DIMINUTO_FILE_METHOD_NONE   = '?',
    DIMINUTO_FILE_METHOD_STDIO  = 'F',
    DIMINUTO_FILE_METHOD_SERIAL = 'S',
    DIMINUTO_FILE_METHOD_SELECT = 'M',
} diminuto_file_method_t;

/**
 * Return the minimum number of bytes likely to be available for the
 * specified FILE object, even if they are not available in the standard
 * I/O buffer. A pending EOF may indicate one byte is available to cause
 * the caller to receive it.
 * @param fp points to the FILE object.
 * @param mp points to where the I/O method is returned.
 * @return 0 if no bytes are available, <0 for an error, or >0;
 */
ssize_t diminuto_file_poll_generic(FILE * fp, diminuto_file_method_t * mp);

/**
 * Return the minimum number of bytes likely to be available for the
 * specified FILE object, even if they are not available in the standard
 * I/O buffer. A pending EOF may indicate one byte is available to cause
 * the caller to receive it.
 * @param fp points to the FILE object.
 * @return 0 if no bytes are available, <0 for an error, or >0;
 */
static inline ssize_t diminuto_file_poll(FILE * fp) {
    diminuto_file_method_t ignore = DIMINUTO_FILE_METHOD_IGNORE;
    return diminuto_file_poll_generic(fp, &ignore);
}

#endif
