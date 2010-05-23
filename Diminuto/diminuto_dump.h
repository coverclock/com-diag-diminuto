/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DUMP_
#define _H_COM_DIAG_DIMINUTO_DUMP_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * Dumps a block of memory in hexadecimal in a format reminiscent (to me
 * anyway) of the old IBM mainframe dumps.
 * @param fp points to the file stream to which to dump.
 * @param data points to the data to dump.
 * @param length is the size of data to dump in bytes.
 * @param upper if true dumps in uppercase hex else lowercase hex.
 * @param dot is the character to substitute for unprintable characters.
 * @param virtualize if true cause the next parameter to be printed as the
 * address instead of the actual address.
 * @param address is used as the replacement address to be printed.
 * @param indent causes each line to be indented by this many spaces.
 */
void diminuto_dump_generic(FILE * fp, const void * data, size_t length, int upper, char dot, int virtualize, uintptr_t first, size_t indent);

/**
 * Dumps a block of memory in hexadecimal in a format reminiscent (to me
 * anyway) of the old IBM mainframe dumps.
 * @param fp points to the file stream to which to dump.
 * @param data points to the data to dump.
 * @param length is the size of data to dump in bytes.
 */
void diminuto_dump(FILE * fp, const void * data, size_t length);

#endif
