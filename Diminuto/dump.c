/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * dump [ -t ]
 *
 * EXAMPLES
 *
 * dump < binaryfile
 * cat binaryfile | dump
 * cat binaryfile | dump -t | someothercommand
 *
 * ABSTRACT
 *
 * Dumps stdin to stdout or to stderr while copying stdin to stdout.
 */

#include "com/diag/diminuto/diminuto_dump.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static const char * program = "dump";

static void usage(FILE * fp)
{
    fprintf(fp, "usage: %s [ -t ]\n", program);
    fprintf(fp, "       -t          Tee input to stdout, output to stderr\n");
    fprintf(fp, "       -?          Print menu\n");
}

int main(int argc, char * argv[])
{
    FILE * out = stdout;
    uintptr_t offset = 0;
    int opt;
    extern char * optarg;
    uint8_t buffer[16];
    size_t length;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "t?")) >= 0) {

        switch (opt) {

        case 't':
            out = stderr;
            break;

        case '?':
            usage(stderr);
            return 0;
            break;

        default:
            usage(stderr);
            return 1;
            break;

        }

    }

    while ((length = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
        diminuto_dump_generic(out, buffer, length, 0, '.', 1, offset, 0);
        if (out == stderr) { fwrite(buffer, length, 1, stdout); }
        offset += length;
    }

    return 0;
}
