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
 * phex [ -l BYTES ] [ -n ] [ -t ]
 *
 * EXAMPLES
 *
 * phex < binaryfile
 * cat binaryfile | phex
 * phex -l 0 -n < /dev/serial
 * phex -e < binaryfile > almostc
 * cat binaryfile | phex -t | someothercommand
 *
 * ABSTRACT
 *
 * Prints characters from standard input to standard output, encoding
 * non-printable characters as if they were written for a C program.
 * Automatically inserts newlines every BYTES printed characters, default
 * eighty, unless BYTES is zero. Encodes newlines like other non-printable
 * characters unless -n is specified. When printed in hexadecimal, two
 * hexadecimal digits are always printed, eliminating any ambiguity. Places
 * standard input in "raw mode" if it is a serial port. Pronounced "fex"
 * but intended to mean "print hex".
 */

#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_phex.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <termio.h>

static const int LENGTH = 80;
static const char * program = "phex";

static void usage(FILE * fp)
{
    fprintf(fp, "usage: %s [ -e ] [ -l BYTES ] [ -n ] [ -t ]\n", program);
    fprintf(fp, "       -e          Also escape the characters '\"', '\\'', and '?'\n");
    fprintf(fp, "       -l BYTES    Limit line length to BYTES instead of %d\n", LENGTH);
    fprintf(fp, "       -n          Do not escape newlines\n");
    fprintf(fp, "       -t          Tee input to stdout, output to stderr\n");
    fprintf(fp, "       -x          Print normally escaped characters in hex\n");
    fprintf(fp, "       -?          Print menu\n");
}

int main(int argc, char * argv[])
{
    size_t current = 0;
    int end = 0;
    int esc = 0;
    size_t length = LENGTH;
    int nl = 0;
    int hex = 0;
    FILE * out = stdout;
    int opt;
    extern char * optarg;
    diminuto_unsigned_t value;
    int ch;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "el:ntx?")) >= 0) {

        switch (opt) {

        case 'e':
            esc = !0;
            break;

        case 'l':
            if (*diminuto_number_unsigned(optarg, &value) != '\0') {
                perror(optarg);
                return 1;
            } else if (value > ~0UL) {
                errno = EINVAL;
                perror(optarg);
                return 1;
            } else {
                length = value;
            }
            break;

        case 'n':
            nl = !0;
            break;

        case 't':
            out = stderr;
            break;

        case 'x':
            hex = !0;
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

    if (isatty(fileno(stdin))) {
        diminuto_serial_raw(fileno(stdin));
    }

    if (isatty(fileno(out))) {
        diminuto_serial_unbuffered(out);
    }

    while ((ch = fgetc(stdin)) != EOF) {
        diminuto_phex_emit(out, ch, length, nl, esc, hex, &current, &end, !0);
        if (out == stderr) { fputc(ch, stdout); }
    }

    diminuto_phex_limit(out, length, 0, &current, &end);

    return 0;
}
