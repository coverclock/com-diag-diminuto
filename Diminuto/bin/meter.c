/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Measure peak and sustained data rates from stdin to stdout.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * meter [ -d ] [ -v ] [ -b BYTES ] [ -c | -l ]
 *
 * EXAMPLES
 *
 * source | shaper -p 512 -s 256 | meter | sink
 *
 * ABSTRACT
 *
 * This filter measures the peak and sustainable rates present on a data
 * stream between standard input and standard output (for example, in a
 * pipeline), and reports the current metrics when it receives a SIGHUP
 * signal.
 */

#include "com/diag/diminuto/diminuto_meter.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static const char * program = (const char *)0;

static void report(const diminuto_meter_t * mp)
{
    diminuto_log_emit("%s: count=%zuio\n", program, diminuto_meter_count(mp));
    diminuto_log_emit("%s: elapsed=%lfs\n", program, (double)diminuto_meter_interval(mp) / (double)diminuto_frequency());
    diminuto_log_emit("%s: total=%zuB\n", program, diminuto_meter_total(mp));
    diminuto_log_emit("%s: mean=%lfB/io\n", program, diminuto_meter_average(mp));
    diminuto_log_emit("%s: peak=%lfB/s\n", program, diminuto_meter_peak(mp));
    diminuto_log_emit("%s: sustained=%lfB/s\n", program, diminuto_meter_sustained(mp));
    diminuto_log_emit("%s: burst=%zuB\n", program, diminuto_meter_burst(mp));
}

int main(int argc, char * argv[])
{
    diminuto_meter_t meter = DIMINUTO_METER_INITIALIZER;
    diminuto_ticks_t now = 0;
    int debug = 0;
    int verbose = 0;
    ssize_t rc = -1;
    size_t size = 0;
    size_t blocksize = 512;
    uint8_t * buffer = (uint8_t *)0;
    int opt;
    extern char * optarg;
    char * end;

    /* INITIALIZATION */

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    /* PARAMETERS */

    while ((opt = getopt(argc, argv, "b:dv")) >= 0) {

        switch (opt) {

        case 'b':
            blocksize = strtoull(optarg, &end, 0);
            if ((blocksize == 0) || (*end != '\0')) {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 'd':
            debug = !0;
            break;

        case 'v':
            verbose = !0;
            break;

        default:
            fprintf(stderr, "usage: %s [ -d ] [ -v ] [ -l | -c ] [ -b BYTES ]\n", program);
            fprintf(stderr, "       -b BYTES             Use an I/O buffer of size BYTES instead of %zu bytes.\n", blocksize);
            fprintf(stderr, "       -d                   Enable debugging output.\n");
            fprintf(stderr, "       -v                   Enable verbose output.\n");
            return 1;
            break;

        }

    }

    /* SETUP */

    buffer = malloc(blocksize);
    if (buffer == (uint8_t *)0) {
        perror("malloc");
        return 2;
    }

    if (diminuto_hangup_install(0) < 0) {
        return 2;
    }

    if (diminuto_interrupter_install(0) < 0) {
        return 2;
    }

    if (diminuto_terminator_install(0) < 0) {
        return 2;
    }

    now = diminuto_meter_now();
    diminuto_meter_init(&meter, now);

    /* WORKLOOP */

    while (!0) {

        /* INPUT */

        if ((rc = diminuto_fd_read_generic(STDIN_FILENO, buffer, 1, blocksize)) > 0) {
            size = rc;
        } else if (rc == 0) {
            break;
        } else if (errno == EINTR) {
            size = 0;
        } else {
            diminuto_perror("read");
            break;
        }

        /* MEASURE */

        if (size > 0) {
            now = diminuto_meter_now();
            diminuto_meter_events(&meter, now, size);
        }

        /* OUTPUT */

        if (size <= 0) {
            /* Do nothing. */
        } else if ((rc = diminuto_fd_write(STDOUT_FILENO, buffer, size)) > 0) {
            /* Do nothing. */
        } else if (rc == 0) {
            break;
        } else {
            diminuto_perror("write");
            break;
        }

        /* REPORT */

        if (diminuto_interrupter_check() > 0) {
            break;
        }

        if (diminuto_terminator_check() > 0) {
            break;
        }

        if ((diminuto_hangup_check() > 0) || verbose) {
            report(&meter);
        }

    }

    /* FINI */

    report(&meter);

    diminuto_meter_fini(&meter);

    return 0;
}
