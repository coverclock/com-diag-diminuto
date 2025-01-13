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
 * filepoller # Serial.
 *
 * filepoller 5000000000 # Serial with a 5s timeout in ticks.
 *
 * filepoller -1 # Serial with a wait.
 *
 * filepoller 0 # Serial with a busy wait.
 *
 * (sleep 5;filepoller) # Serial and gives time to queue chars up on UART.
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
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    int xc = 0;
    size_t total = 0;
    ssize_t bytes = 0;
    diminuto_file_method_t method = DIMINUTO_FILE_METHOD_NONE;
    int ch = '\0';
    bool timedout = false;
    static const diminuto_sticks_t TIMEOUT = COM_DIAG_DIMINUTO_FREQUENCY;
    diminuto_sticks_t timeout = TIMEOUT;
    long long argument = 0;
    char * endptr = (char *)0;

    (void)diminuto_alarm_install(0);

    if (argc > 1) {
        argument = strtoll(argv[1], &endptr, 0 /* Any base. */);
        if (endptr == (char *)0) {
            /* Do nothing. */
        } else if (*endptr != '\0') {
            /* Do nothng. */
        } else {
            /*
             * <0: wait
             * 0:  poll
             * >0: timeout
             */
            timeout = argument;
        }
    }

    do {
        bytes = diminuto_file_poll_base(stdin, timeout, &method);
        if (bytes == 0) {
            if (!timedout) {
                fprintf(stderr, "%s [%lu] [%ld] '%c' %lldticks\n", argv[0], total, bytes, method, (long long)timeout);
                timedout = true;
            }
        } else if (bytes > 0) {
            total += bytes;
            fprintf(stderr, "%s [%lu] [%ld] '%c'\n", argv[0], total, bytes, method);
            while (bytes > 0) {
                if ((ch = fgetc(stdin)) == EOF) {
                    break;
                }
                (void)fputc(ch, stdout);
                bytes -= 1;
            }
            timedout = false;
        } else if (errno == EINTR) {
            fprintf(stderr, "%s [%lu] [%ld] <%d>\n", argv[0], total, bytes, SIGALRM);
            timedout = false;
            continue;
        } else {
            xc = 1;
            break;
        }
    } while (ch != EOF);

    fprintf(stderr, "%s [%lu] [%ld] (%d)\n", argv[0], total, bytes, xc);

    return xc;
}
