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

#include "diminuto_types.h"
#include <stdio.h>

/**
 * Dumps a block of memory in hexadecimal along with printable characters.
 * The hexadecimal and character portions of the dump are both done in network
 * byte order (big-endian), with addresses increasing from left to right. This
 * makes multi-byte fields on little-endian systems look a little weird as
 * bytes are swapped. This function is ridiculously customizable; much easier
 * to use one of the macros like diminuto_dump(), diminuto_dump_virtual(),
 * or diminuto_dump_bytes().
 * @param fp points to the file stream to which to dump.
 * @param data points to the data to dump.
 * @param length is the size of data to dump in bytes.
 * @param upper if true dumps in uppercase hex else lowercase hex.
 * @param dot is the character to substitute for unprintable characters.
 * @param virtualize if true cause the next parameter to be printed as the
 * address instead of the actual address.
 * @param address is used as the replacement address to be printed.
 * @param indent causes each line to be indented by this many spaces.
 * @param bpw is the number of bytes per word.
 * @param wpl is the number of words per line.
 * @param addrsep points to a string that seperates address and first word.
 * @param wordsep points to a string that seperates words.
 * @param charsep points to a string that seperates last word from characters.
 * @param byteskip is a character printed when there are no bytes.
 * @param charskip is a character printed when there are no characters.
 * @param lineend points to to a string that ends each line.
 */
extern void diminuto_dump_custom(
    FILE * fp,
    const void * data,
    size_t length,
    int upper,
    char dot,
    int virtualize,
    uintptr_t address,
    size_t indent,
    size_t bpw,
    size_t wpl,
    const char * addrsep,
    const char * wordsep,
    const char * charsep,
    const char byteskip,
    const char charskip,
    const char * lineend
);

#define diminuto_dump_generic(_FP_, _DATA_, _LENGTH_, _UPPER_, _DOT_, _VIRTUALIZE_, _ADDRESS_, _INDENT_) \
    diminuto_dump_custom(_FP_, _DATA_, _LENGTH_, _UPPER_, _DOT_, _VIRTUALIZE_, _ADDRESS_, _INDENT_, 4, 4, ": ", " ", "|", ' ', ' ', "|\n")

#define diminuto_dump_bytes(_FP_, _DATA_, _LENGTH_) \
    diminuto_dump_custom(_FP_, _DATA_, _LENGTH_, 0, '.', 0, 0, 0, 1, 16, ": ", " ", "|", ' ', ' ', "|\n")

#define diminuto_dump_virtual(_FP_, _DATA_, _LENGTH_, _ADDRESS_) \
    diminuto_dump_generic(_FP_, _DATA_, _LENGTH_, 0, '.', !0, _ADDRESS_, 0)

#define diminuto_dump(_FP_, _DATA_, _LENGTH_) \
    diminuto_dump_generic(_FP_, _DATA_, _LENGTH_, 0, '.', 0, 0, 0)

#endif
