/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * phex [ -l BYTES ] [ -n ]
 *
 * EXAMPLES
 *
 * phex < binaryfile
 * phex -l 0 -n < /dev/serial
 * cat binaryfile | phex
 *
 * ABSTRACT
 *
 * Prints characters from standard input to standard output, encoding
 * non-printable characters as if they were written for a C program.
 * Automatically inserts newlines every BYTES printed characters, default
 * eighty, unless BYTES is zero. Encodes newlines like other non-printable
 * characters unless -n is specified. Places standard input in "raw mode" if
 * it is a serial port. Pronounced "fex" but intended to mean "print hex".
 */

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

static void raw(int fd)
{
    struct termios tios = { 0 };

    do {

        if (tcgetattr(fd, &tios) < 0) {
            perror("tcgetattr");
            break;
        }

        /*
         * Taken right from termios(3) "Raw mode".
         */
        tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON );
        tios.c_oflag &= ~OPOST;
        tios.c_cflag &= ~(CSIZE | PARENB);
        tios.c_cflag |= (CS8 | CREAD | CLOCAL);
        tios.c_cc[VTIME] = 0;
        tios.c_cc[VMIN] = 1;

        if (tcflush(fd, TCIFLUSH) < 0) {
            perror("tcflush");
            /* Proceed anyway. */
        }

        if (tcsetattr(fd, TCSANOW, &tios) < 0) {
            perror("tcsetattr");
            break;
        }

    } while (0);
}

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

static void phex(FILE * fp, unsigned char ch, size_t length, int newlines)
{
    /*                                 BEL  BS   TAB  LF   VT   FF   CR  */
    static unsigned char special[] = { 'a', 'b', 't', 'n', 'v', 'f', 'r' };

    if (ch == '\0') {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc('0', fp);
    } else if ((ch == '\n') && newlines) {
        limit(fp, length, -1);
    } else if (('\a' <= ch) && (ch <= '\r')) {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc(special[ch - '\a'], fp);
    } else if (ch == '\\') {
        limit(fp, length, 2);
        fputc('\\', fp);
        fputc('\\', fp);
    } else if ((' ' <= ch) && (ch <= '~')) {
        limit(fp, length, 1);
        fputc(ch, fp);
    } else {
        limit(fp, length, 4);
        fprintf(fp, "\\x%2.2x", ch);
    }
}

static void usage(FILE * fp)
{
    fprintf(fp, "usage: %s [ -l BYTES ] [ -n ]\n", program);
    fprintf(fp, "       -l BYTES    Use BYTES for length instead of %d\n", LENGTH);
    fprintf(fp, "       -n          Do not escape newlines\n");
    fprintf(fp, "       -?          Print menu\n");
}

int main(int argc, char * argv[])
{
    size_t length = LENGTH;
    int newlines = 0;
    int opt;
    extern char * optarg;
    uint64_t value;
    int ch;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "l:n?")) >= 0) {

        switch (opt) {

        case 'l':
            if (*diminuto_unsigned(optarg, &value) != '\0') {
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
            newlines = !0;
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
        raw(fileno(stdin));
    }

    while ((ch = fgetc(stdin)) != EOF) {
        phex(stdout, ch, length, newlines);
    }

    limit(stdout, length, 0);

    return 0;
}
