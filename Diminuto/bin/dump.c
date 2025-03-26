/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Dumps files or stdin to stdout, or to stderr while copying input to stdout.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * dump [ -t ] [ [ file | - ] ... ]
 *
 * EXAMPLES
 *
 * dump binaryfile1 binaryfile2 binaryfile3
 * dump binaryfile1 - binaryfile3 < binaryfile2
 * dump < binaryfile
 * cat binaryfile | dump
 * cat binaryfile | dump -t | someothercommand
 *
 * ABSTRACT
 *
 * Dumps files or stdin to stdout, or to stderr while copying input to stdout.
 */

#include "com/diag/diminuto/diminuto_dump.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static const char * program = "dump";

static void usage(FILE * fp)
{
    fprintf(fp, "usage: %s [ -t ]\n", program);
    fprintf(fp, "       -t          Tee input to stdout, output to stderr\n");
    fprintf(fp, "       -?          Print menu\n");
}

static void dump(FILE * in, FILE * out)
{
    uintptr_t offset = 0;
    uint8_t buffer[16];
    size_t length;

    while ((length = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        diminuto_dump_general(out, buffer, length, 0, '.', 1, offset, 0);
        if (out == stderr) { fwrite(buffer, length, 1, stdout); }
        offset += length;
    }
}

int main(int argc, char * argv[])
{
    FILE * in = stdin;
    FILE * out = stdout;
    const char * name = "-";
    int opt;
    extern char * optarg;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "t?")) >= 0) {

        switch (opt) {

        case 't':
            out = stderr;
            break;

        case '?':
        default:
            usage(stderr);
            return 1;
            break;

        }

    }

    if (optind < argc) {
        for (; optind < argc; ++optind) {
            name = argv[optind];
            if (strcmp(name, "-") == 0) {
                in = stdin;
            } else if ((in = fopen(name, "rb")) == (FILE *)0) {
                perror(name);
                return 2;
            } else {
                /* Do nothing. */
            }
            dump(in, out);
            if (ferror(in)) {
                errno = EIO;
                perror(name);
                return 3;
            }
            if (in == stdin) {
                /* Do nothing. */
            } else if (fclose(in) == 0) {
                /* Do nothing. */
            } else {
                perror(name);
            }
        }
    } else {
        dump(in, out);
        if (ferror(in)) {
            errno = EIO;
            perror(name);
            return 3;
        }
    }

    return 0;
}
