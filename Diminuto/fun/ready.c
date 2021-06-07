/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2019 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the File functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the File functional test.
 * This may be useful to run with strace(1) to watch the actual I/Os.
 */

#include "com/diag/diminuto/diminuto_file.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

int main(int argc, char * argv[])
{
    const char * name;
    FILE * fp;
    ssize_t before;
    ssize_t after;
    ssize_t maximumin;
    ssize_t maximumout;
    int ch;
    uint64_t index;

    name = ((name = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : name + 1;

    if (argc < 2) {
        fp = stdin;
    } else if (strcmp(argv[1], "-") == 0) {
        fp = stdin;
    } else if ((fp = fopen(argv[1], "r")) == (FILE *)0) {
        perror(argv[1]);
        return 1;
    } else {
        /* Do nothing. */
    }

    index = 0;
    maximumin = 0;
    maximumout = 0;

    fprintf(stderr, "%s: DIRECT %2s %10s %4s %4s %4s %4s\n", name, "FD", "INDEX", "MAX", "BFOR", "AFTR", "CHAR");

    while (!0) {
        before = diminuto_file_ready(fp);
        if (before > maximumin) { maximumin = before; }
        ch = getc(fp);
        after = diminuto_file_ready(fp);
        if (after > maximumin) { maximumin = after; }
        if (ch == EOF) {
            break;
        } else if (isprint(ch)) {
            fprintf(stderr, "%s: input  %2d %10lld %4lld %4lld %4lld 0x%02x %c\n", name, fileno(fp), (unsigned long long)index, (long long)maximumin, (long long)before, (long long)after, ch, (after >= before) ? '*' : ' ');
        } else {
            fprintf(stderr, "%s: input  %2d %10lld %4lld %4lld %4lld 0x%02x %c\n", name, fileno(fp), (unsigned long long)index, (long long)maximumin, (long long)before, (long long)after, ch, (after >= before) ? '*' : ' ');
        }
        before = diminuto_file_available(stdout);
        if (before > maximumout) { maximumout = before; }
        fputc(ch, stdout);
        after = diminuto_file_available(stdout);
        if (after > maximumout) { maximumout = after; }
        fprintf(stderr, "%s: output %2d %10lld %4lld %4lld %4lld 0x%02x %c\n", name, fileno(stdout), (unsigned long long)index, (long long)maximumout, (long long)before, (long long)after, ch, (after >= before) ? '*' : ' ');
        index += 1;
    }

    before = diminuto_file_available(stdout);
    if (before > maximumout) { maximumout = before; }
    fflush(stdout);
    after = diminuto_file_available(stdout);
    if (after > maximumout) { maximumout = after; }
    fprintf(stderr, "%s: output %2d %10lld %4lld %4lld %4lld      %c\n", name, fileno(stdout), (unsigned long long)index, (long long)maximumout, (long long)before, (long long)after, (after >= before) ? '*' : ' ');

    fclose(fp);
    fclose(stdout);

    return 0;
}
