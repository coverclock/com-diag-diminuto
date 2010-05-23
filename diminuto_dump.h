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
 * Dumps a block of memory in hexadecimal along with printable characters.
 * The hexadecimal and character portions of the dump are both done in network
 * byte order (big-endian), with addresses increasing from left to right. This
 * makes multi-byte fields on little-endian systems look a little weird as
 * bytes are swapped.
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
void diminuto_dump_generic(FILE * fp, const void * data, size_t length, int upper, char dot, int virtualize, uintptr_t address, size_t indent);

#define diminuto_dump(_FP_, _DATA_, _LENGTH_) \
    diminuto_dump_generic(_FP_, _DATA_, _LENGTH_, 0, '.', 0, 0, 0)

#endif
