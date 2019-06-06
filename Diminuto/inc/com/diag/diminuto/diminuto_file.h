/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FILE_
#define _H_COM_DIAG_DIMINUTO_FILE_

/**
 * @file
 *
 * Copyright 2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_types.h"

/*
 * May fail to compile with other C libraries like UlibC or Bionic.
 *
 * May compiles but yield the wrong result if even a minor change is made to GLIBC.
 *
 * Based on a similar hack in the com-diag-grandote C++ project.
 *
 * There is no unit test for this, but there is a functional test.
 *
 * Would really be helpful if the C library had a standard way to do this.
 */

/**
 * Return the number of bytes a FILE object has ready to read in its buffer.
 * @param fp points to the FILE object.
 * @return the number of bytes available or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_ready(const FILE * fp)
{
    return (fp->_IO_read_end - fp->_IO_read_ptr);
}

#endif
