/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
    uintptr_t mask;

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

    mask = (bpw * wpl) - 1;
    begin = address & ~mask;
    end = (last + mask) & ~mask;

    here = begin;

    while (here < end) {

        for (ii = 0; ii < indent; ++ii) {
            fputc(' ', fp);
        }

        fprintf(fp, eight, here);
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
                    fprintf(fp, two, *pp);
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
