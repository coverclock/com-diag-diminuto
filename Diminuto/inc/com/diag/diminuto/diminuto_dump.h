/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DUMP_
#define _H_COM_DIAG_DIMINUTO_DUMP_

/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides functions to do formatted memory dumps to an output stream.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Dump feature implements a variety of memory dump functions to a
 * FILE stream. This facility was inspired by countless dump programs the
 * author has  encountered over the years on platforms like the IBM 360 and
 * the PDP-11. Depending on what you're doing, you might want to try
 * diminuto_phex_emit() instead, particularly if you are dumping data byte
 * by byte from a real-time source like a serial port. On the other hand,
 * if your data is in a buffer or is structured, this approach is probably
 * better.
 */

#include "com/diag/diminuto/diminuto_types.h"
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
 * @param addrsep points to a string that separates address and first word.
 * @param wordsep points to a string that separates words.
 * @param charsep points to a string that separates last word from characters.
 * @param byteskip is a character printed when there are no bytes.
 * @param charskip is a character printed when there are no characters.
 * @param lineend points to to a string that ends each line.
 */
extern void diminuto_dump_generic(
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

/**
 * Specialize diminuto_dump_generic() using for four bytes per word and four
 * words per line.
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
static inline void diminuto_dump_general(
    FILE * fp,
    const void * data,
    size_t length,
    int upper,
    char dot,
    int virtualize,
    uintptr_t address,
    size_t indent
) {
    diminuto_dump_generic(fp, data, length, upper, dot, virtualize, address, indent, sizeof(int32_t), 16 / sizeof(int32_t), ": ", " ", "|", ' ', ' ', "|\n");
}

/**
 * Specialize diminuto_dump_generic() for a byte by byte dump.
 * @param fp points to the file stream to which to dump.
 * @param data points to the data to dump.
 * @param length is the size of data to dump in bytes.
 */
static inline void diminuto_dump_bytes(
    FILE * fp,
    const void * data,
    size_t length
) {
    diminuto_dump_generic(fp, data, length, 0, '.', 0, 0, 0, 1, 2 * sizeof(int64_t), ": ", " ", "|", ' ', ' ', "|\n");
}

/**
 * Specialize diminuto_dump_general() for a dump that displays offsets relative
 * to a specified address instead of actual (virtual) memory addresses.
 * @param fp points to the file stream to which to dump.
 * @param data points to the data to dump.
 * @param length is the size of data to dump in bytes.
 * @param address is used as the replacement address to be printed.
 */
static inline void diminuto_dump_virtual(
    FILE * fp,
    const void * data,
    size_t length,
    uintptr_t address
) {
    diminuto_dump_general(fp, data, length, 0, '.', !0, address, 0);
}

/**
 * Specialize diminuto_dump_general() for a good old fashioned general purpose
 * dump probably suitable for whatever you may have in mind.
 * @param fp points to the file stream to which to dump.
 * @param data points to the data to dump.
 * @param length is the size of data to dump in bytes.
 */
static inline void diminuto_dump(
    FILE * fp,
    const void * data,
    size_t length
) {
    diminuto_dump_general(fp, data, length, 0, '.', 0, 0, 0);
}

#endif
