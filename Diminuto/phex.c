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

#include "diminuto_serial.h"
#include "diminuto_number.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <termio.h>

static const int LENGTH = 80;

static const char * program = "phex";
static int debug = 0;

static void limit(FILE * fp, const size_t length, ssize_t increment)
{
    static size_t current = 0;
    static int end = 0;

    if (length == 0) {
        /* Do nothing. */
    } else if ((increment < 0) && (!end)) {
        fputc('\n', fp);
        end = !0;
    } else if ((increment == 0) && (current > 0) && (!end)) {
        fputc('\n', fp);
        end = !0;
    } else if ((current += increment) > length) {
        fputc('\n', fp);
        end = !0;
        current = increment;
    } else {
        end = 0;
    }
}

static void phex(FILE * fp, unsigned char ch, size_t length, int nl, int esc, int hex)
{
    /*                                 BEL  BS   TAB  LF   VT   FF   CR  */
    static unsigned char special[] = { 'a', 'b', 't', 'n', 'v', 'f', 'r' };

    if ((ch == '\0') && hex) {
        limit(fp, length, 4);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (ch == '\0') {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc('0', fp);
    } else if ((ch == '\n') && nl) {
        limit(fp, length, -1);
    } else if ((('\a' <= ch) && (ch <= '\r')) && hex) {
        limit(fp, length, 4);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (('\a' <= ch) && (ch <= '\r')) {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc(special[ch - '\a'], fp);
    } else if ((ch == '\\') && hex) {
        limit(fp, length, 4);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (ch == '\\') {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc(ch, fp);
    } else if (((ch == '"') || (ch == '\'') || (ch == '?')) && esc && hex) {
        limit(fp, length, 4);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (((ch == '"') || (ch == '\'') || (ch == '?')) && esc) {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc(ch, fp);
    } else if ((' ' <= ch) && (ch <= '~')) {
        limit(fp, length, 1);
        fputc(ch, fp);
    } else {
        limit(fp, length, 4);
        fprintf(fp, "\\x%2.2x", ch);
    }

    (void)fflush(fp);
}

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
    int esc = 0;
    size_t length = LENGTH;
    int nl = 0;
    int hex = 0;
    FILE * out = stdout;
    int opt;
    extern char * optarg;
    uint64_t value;
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
        phex(out, ch, length, nl, esc, hex);
        if (out == stderr) { fputc(ch, stdout); }
    }

    limit(out, length, 0);

    return 0;
}
