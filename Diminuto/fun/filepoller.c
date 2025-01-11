/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the File functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Copies stdin to stdout while monitoring the stream using File poll.
 *
 * E.g.
 *
 * filepoller
 *
 * filepoller < lesser.txt > temp.txt;
 * diff lesser.txt temp.txt;
 * rm temp.txt
 *
 * cat lesser.txt | filepoller > temp.txt;
 * diff lesser.txt temp.txt;
 * rm temp.txt
 *
 * mkfifo FIFO;
 * filepoller < FIFO > temp.txt &
 * cat lesser.txt > FIFO;
 * diff lesser.txt temp.txt;
 * rm temp.txt;
 * rm FIFO;
 */

#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    int xc = 0;
    size_t total = 0;
    ssize_t bytes = 0;
    diminuto_file_method_t method = DIMINUTO_FILE_METHOD_NONE;
    int ch = '\0';
    bool sleepy = false;

    do {
        bytes = diminuto_file_poll_generic(stdin, &method);
        if (bytes < 0) {
            xc = 1;
            break;
        } else if (bytes == 0) {
            if (!sleepy) {
                fprintf(stderr, "%s [%lu] [%ld] '%c'\n", argv[0], total, bytes, method);
            }
            usleep(1000000);
            sleepy = true;
        } else {
            total += bytes;
            fprintf(stderr, "%s [%lu] [%ld] '%c'\n", argv[0], total, bytes, method);
            while (bytes > 0) {
                if ((ch = fgetc(stdin)) == EOF) {
                    total -= 1;
                    break;
                }
                (void)fputc(ch, stdout);
                bytes -= 1;
            }
            sleepy = false;
        }
    } while (ch != EOF);

    fprintf(stderr, "%s [%lu] [%ld] (%d)\n", argv[0], total, bytes, xc);

    return xc;
}
