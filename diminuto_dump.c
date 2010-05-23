/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_dump.h"

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
void diminuto_dump_generic(FILE * fp, const void * data, size_t length, int upper, char dot, int virtualize, uintptr_t address, size_t indent)
{
    const char * eight;
    const char * two;
    uintptr_t first;
    uintptr_t last;
    uintptr_t begin;
    uintptr_t end;
    uintptr_t line;
    uintptr_t word;
    uintptr_t here;
    uintptr_t hh;
    const unsigned char * pointer;
    const unsigned char * pp;
    size_t ii;

    if (upper) {
        eight = "%8.8X";
        two = "%2.2X";
    } else {
        eight = "%8.8x";
        two = "%2.2x";
    }

    pointer = (const unsigned char *)data;

    if (!virtualize) {
        address = (uintptr_t)pointer;
    }

    first = address;
    last = address + length;

    begin = address & ~(uintptr_t)0xf;
    end = (last + 0xf) & ~(uintptr_t)0xf;

    here = begin;

    while (here < end) {

        for (ii = 0; ii < indent; ++ii) {
            fputc(' ', fp);
        }

        fprintf(fp, eight, here);
        fputs(": ", fp);

        pp = pointer;
        hh = here;

        line = here + 0x10;

        while (hh < line) {

            word = hh + 0x4;

            while (hh < word) {
                if (!((first <= hh) && (hh < last))) {
                    fputs("  ", fp);
                } else {
                    fprintf(fp, two, *pp);
                    ++pp;
                }
                ++hh;
            }

            fputc(' ', fp);

        }

        pp = pointer;
        hh = here;

        fputc('|', fp);

        while (hh < line) {

            if (!((first <= hh) && (hh < last))) {
                fputc(' ', fp);
            } else if ((' ' <= *pp) && (*pp <= '~')) {
                fputc(*pp, fp);
                ++pp;
            } else {
                fputc(dot, fp);
                ++pp;
            }
            ++hh;
        }

        fputs("|\n", fp);

        pointer = pp;
        here = hh;
    }
}
