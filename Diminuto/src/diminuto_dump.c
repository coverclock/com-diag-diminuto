/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Dump feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Dump feature.
 */

#include "com/diag/diminuto/diminuto_dump.h"

void diminuto_dump_generic(
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
)
{
    const char * addressformat;
    const char * datumformat;
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
    uintptr_t mask;

    if (upper) {
        addressformat = "%16.16X";
        datumformat = "%2.2X";
    } else {
        addressformat = "%16.16x";
        datumformat = "%2.2x";
    }

    pointer = (const unsigned char *)data;

    if (!virtualize) {
        address = (uintptr_t)pointer;
    }

    first = address;
    last = address + length;

    mask = (bpw * wpl) - 1;
    begin = address & ~mask;
    end = (last + mask) & ~mask;

    here = begin;

    while (here < end) {

        for (ii = 0; ii < indent; ++ii) {
            fputc(' ', fp);
        }

        fprintf(fp, addressformat, here);
        fputs(addrsep, fp);

        pp = pointer;
        hh = here;

        line = here + (bpw * wpl);

        while (hh < line) {

            word = hh + bpw;

            while (hh < word) {
                if (!((first <= hh) && (hh < last))) {
                    fputc(byteskip, fp);
                    fputc(byteskip, fp);
                } else {
                    fprintf(fp, datumformat, *pp);
                    ++pp;
                }
                ++hh;
            }

            fputs(wordsep, fp);

        }

        pp = pointer;
        hh = here;

        fputs(charsep, fp);

        while (hh < line) {

            if (!((first <= hh) && (hh < last))) {
                fputc(charskip, fp);
            } else if ((' ' <= *pp) && (*pp <= '~')) {
                fputc(*pp, fp);
                ++pp;
            } else {
                fputc(dot, fp);
                ++pp;
            }
            ++hh;
        }

        fputs(lineend, fp);

        pointer = pp;
        here = hh;
    }
}
