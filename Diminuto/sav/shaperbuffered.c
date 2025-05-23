/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Traffic shape and measure throughput from stdin to stdout.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * shaperbuffered [ -d ] [ -v ] [ -b BYTES ] [ -p BYTESPERSECOND ] [ -j TICKS ] [ -s BYTESPERSECOND ] [ -m BYTES ]
 *
 * Consider letting the jitter tolerance ("-j") default to zero ticks and
 * setting the maximum burst size ("-m") to the same value in bytes as the
 * I/O block size ("-b").
 *
 * EXAMPLES
 *
 * source | shaperbuffered | sink<BR>
 * source | shaperbuffered -b 4096 -p 2048 -s 1024 -m 4096 | sink<BR>
 * dd if=/dev/urandom bs=512 count=100 | shaperbuffered -b 512 | shaperbuffered -b 512 -p 2048 -s 1024 -m 512 | shaperbuffered -b 512 > /dev/null<BR>
 *
 * ABSTRACT
 *
 * Implements both a traffic shaper and a traffic throughput measurer.
 * Throughput is always measured and reported at EOF of the incoming stream
 * or upon receipt of SIGHUP, SIGINT, SIGTERM, or SIGQUIT. If traffic shaping
 * parameters are provided on the command line, traffic is shaped to the
 * specified contract.
 *
 * For traffic sources that generate data with an interarrival time smaller
 * than the resolution of the system clock (which may be larger than that of
 * either the Diminuto tick or the POSIX time system call), the peak rate
 * cannot be calculated. This is because the interarrival time between
 * successive data events is effectively zero (because it is too short to
 * measure), so the peak rate is, arithmetically, infinite, even though it is
 * not really the case. The sustained rate, however, can and is measured.
 *
 * For contracts that have maximum burst sizes greater than zero and for which
 * the peak rate is greater than the sustained rate, the measured sustained rate
 * may take a long time to reduce to the contracted rate. This is due to the big
 * bulge at the beginning in which the maximum burst size is transmitted at the
 * peak rate, skewing the measured sustained rate. Particularly for fast traffic
 * sources and large I/O block sizes, the contracted sustained rate is a very
 * long term average.
 *
 * This filter uses the Diminuto shaper in its multiple event mode: the contract
 * specifies the delay for the first byte of the next I/O block to be in
 * conformance with the contract. But the entire I/O block is written at once
 * and the shaper updated appropriately so that the first byte of the next I/O
 * block is in conformance. The resulting traffic is extremely bursty, even
 * though the long term peak and sustainable rates are correct. The alternative
 * is to use a I/O block size of one byte, so that every single byte is in
 * conformance. This causes as much I/O and system call overhead as you would
 * expect, and isn't really practical.
 */

#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

typedef enum IO {
    BLOCK,
    CHARACTER,
    LINE,
} io_t;

static const size_t BLOCKSIZE = 512;
static const diminuto_ticks_t JITTERTOLERANCE = 0;

static io_t io = BLOCK;
static const char * program = (const char *)0;
static size_t count = 0;
static size_t total = 0;
static size_t burst = 0;
static diminuto_ticks_t frequency = 0;
static diminuto_ticks_t epoch = 0;
static diminuto_ticks_t duration = 0;
static double peak = 0.0;
static double sustained = 0.0;
static uint16_t fletcher16c = 0;
static int debug = 0;
static int verbose = 0;

static void report(void)
{
    diminuto_log_emit("%s: count=%zuio\n", program, count);
    diminuto_log_emit("%s: elapsed=%lfs\n", program, (float)duration / (float)frequency);
    diminuto_log_emit("%s: total=%zuB\n", program, total);
    diminuto_log_emit("%s: mean=%lfB/io\n", program, (float)total / (float)count);
    diminuto_log_emit("%s: burst=%zuB\n", program, burst);
    diminuto_log_emit("%s: peak=%lfB/s\n", program, peak);
    diminuto_log_emit("%s: sustained=%lfB/s\n", program, sustained);
    diminuto_log_emit("%s: fletcher16=0x%4.4x\n", program, fletcher16c);
}

static void handler(int signum)
{
    report();
    if ((signum == SIGINT) || (signum == SIGTERM) || (signum == SIGQUIT) | (signum == SIGPIPE)) {
        exit(0);
    }
}

static ssize_t input(uint8_t * buffer, size_t size)
{
    ssize_t rc = 0;
    int ch = '\0';

    switch (io) {
    case BLOCK:
        rc = fread(buffer, 1, size, stdin);
        break;
    case CHARACTER:
        if ((ch = fgetc(stdin)) != EOF) {
            *buffer = ch;
            rc = 1;
        }
        break;
    case LINE:
        if (fgets((char *)buffer, (int)size, stdin) != NULL) {
            buffer[size - 1] = '\0';
            rc = (ssize_t)strlen((char *)buffer);
        }
        break;
    default:
        break;
    }

    if (rc > 0) {
        /* Do nothing. */
    } else if (feof(stdin)) {
        rc = 0;
    } else if (ferror(stdin)) {
        errno = EIO;
        perror("fread");
        rc = -1;
    } else {
        errno = EINVAL;
        perror("fread");
        rc = -1;
    }

    return rc;
}

static ssize_t output(const uint8_t * buffer, size_t size)
{
    ssize_t rc = 0;
    int ch = 0;

    switch (io) {
    case BLOCK:
        if ((rc = fwrite(buffer, size, 1, stdout)) == 1) {
            rc = size;
        }
        break;
    case CHARACTER:
        ch = *buffer;
        if (fputc(ch, stdout) != EOF) {
            rc = 1;
        }
        break;
    case LINE:
        if (fputs((char *)buffer, stdout) > 0) {
            rc = size;
        }
        break;
    default:
        break;
    }

    if (rc > 0) {
        /* Do nothing. */
    } else if (feof(stdout)) {
        rc = 0;
    } else if (ferror(stdout)) {
        errno = EIO;
        perror("fwrite");
        rc = -1;
    } else {
        errno = EINVAL;
        perror("fwrite");
        rc = -1;
    }

    return rc;
}

int main(int argc, char * argv[])
{
    diminuto_shaper_t shaper;
    size_t peakrate = 0;
    size_t sustainedrate = 0;
    size_t maximumburstsize = 0;
    diminuto_ticks_t peakincrement = 0;
    diminuto_ticks_t jittertolerance = 0;
    diminuto_ticks_t sustainedincrement = 0;
    diminuto_ticks_t bursttolerance = 0;
    uint8_t fletcher16a = 0;
    uint8_t fletcher16b = 0;
    int shaped = 0;
    diminuto_ticks_t now = 0;
    diminuto_ticks_t then = 0;
    diminuto_ticks_t interval = 0;
    diminuto_ticks_t minimum = 0;
    diminuto_ticks_t elapsed = 0;
    ssize_t rc = -1;
    size_t size = 0;
    double delay = 0.0;
    double rate = 0.0;
    size_t blocksize = 0;
    uint8_t * buffer = (uint8_t *)0;
    struct sigaction action;
    int opt;
    extern char * optarg;
    char * end;

    /* INITIALIZATION */

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    frequency = diminuto_frequency();

    io = BLOCK;
    blocksize = BLOCKSIZE;
    jittertolerance = JITTERTOLERANCE;

    minimum = frequency / diminuto_delay_frequency();

    /* PARAMETERS */

    while ((opt = getopt(argc, argv, "b:cdj:lm:p:s:v?")) >= 0) {

        switch (opt) {

        case 'b':
            blocksize = strtoull(optarg, &end, 0);
            if ((blocksize == 0) || (*end != '\0')) {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 'c':
            io = CHARACTER;
            break;

        case 'd':
            debug = !0;
            break;

        case 'j':
            jittertolerance = strtoull(optarg, &end, 0);
            if (*end != '\0') {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 'l':
            io = LINE;
            break;

        case 'm':
            maximumburstsize = strtoull(optarg, &end, 0);
            if (*end != '\0') {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 'p':
            peakrate = strtoull(optarg, &end, 0);
            if ((peakrate == 0) || (*end != '\0')) {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 's':
            sustainedrate = strtoull(optarg, &end, 0);
            if ((sustainedrate == 0) || (*end != '\0')) {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 'v':
            verbose = !0;
            break;

        case '?':
        default:
            fprintf(stderr, "usage: %s [ -d ] [ -v ] [ -l | -c ] [ -b BYTES ] [ -p BYTESPERSECOND ] [ -j USECONDS ] [ -s BYTESPERSECOND ] [ -m BYTES ]\n", program);
            fprintf(stderr, "       -b BYTES             Use an I/O buffer of size BYTES instead of %zu bytes.\n", blocksize);
            fprintf(stderr, "       -c                   Do character I/O instead of block I/O.\n");
            fprintf(stderr, "       -d                   Enable debugging output.\n");
            fprintf(stderr, "       -j USECONDS          Use a jitter tolerance of USECONDS instead of %lld microseconds.\n", (long long int)jittertolerance);
            fprintf(stderr, "       -l                   Do line I/O instead of block I/O.\n");
            fprintf(stderr, "       -p BYTESPERSECOND    Set the peak rate to BYTESPERSECOND bytes per second.\n");
            fprintf(stderr, "       -m BYTES             Set the maximum burst size to BYTES instead of %zu bytes.\n", maximumburstsize);
            fprintf(stderr, "       -s BYTESPERSECOND    Set the sustained rate to BYTESPERSECOND bytes per second.\n");
            fprintf(stderr, "       -v                   Enable verbose output.\n");
            return 1;
            break;

        }

    }

    if (sustainedrate == 0) {
        sustainedrate = peakrate;
        maximumburstsize = 0;
    } else if (peakrate == 0) {
        peakrate = sustainedrate;
        maximumburstsize = 0;
    } else if (maximumburstsize == 0) {
        peakrate = sustainedrate;
    } else if (sustainedrate > peakrate) {
        errno = EINVAL;
        perror("-s");
        return 1;
    }

    if (debug) {
        fprintf(stderr, "%s: -d\n", program);
        switch (io) {
        case BLOCK:
            break;
        case CHARACTER:
            fprintf(stderr, "%s: -c\n", program);
            break;
        case LINE:
            fprintf(stderr, "%s: -l\n", program);
            break;
        default:
            break;
        }
        fprintf(stderr, "%s: -b %zuB\n", program, blocksize);
        fprintf(stderr, "%s: -p %zuB/s\n", program, peakrate);
        fprintf(stderr, "%s: -j %lldusec\n", program, (long long int)jittertolerance);
        fprintf(stderr, "%s: -s %zuB/s\n", program, sustainedrate);
        fprintf(stderr, "%s: -m %zuB\n", program, maximumburstsize);
        if (verbose) {
            fprintf(stderr, "%s: -v\n", program);
        }
    }

    /* SETUP */

    shaped = (peakrate > 0) && (sustainedrate > 0) && (sustainedrate <= peakrate);

    buffer = malloc(blocksize);
    if (buffer == (uint8_t *)0) {
        perror("malloc");
        return 2;
    }

    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGHUP, &action, (struct sigaction *)0) < 0) {
        perror("sigaction(SIGHUP)");
        return 2;
    }
    if (sigaction(SIGINT, &action, (struct sigaction *)0) < 0) {
        perror("sigaction(SIGINT)");
        return 2;
    }
    if (sigaction(SIGQUIT, &action, (struct sigaction *)0) < 0) {
        perror("sigaction(SIGQUIT)");
        return 2;
    }
    if (sigaction(SIGTERM, &action, (struct sigaction *)0) < 0) {
        perror("sigaction(SIGTERM)");
        return 2;
    }

    /* CONTRACT */

    epoch = diminuto_shaper_now();
    if (shaped) {
        peakincrement = diminuto_throttle_interarrivaltime(peakrate, 1, frequency);
        sustainedincrement = diminuto_throttle_interarrivaltime(sustainedrate, 1, frequency);
        jittertolerance = diminuto_frequency_units2ticks(jittertolerance, 1000000LL);
        bursttolerance = diminuto_shaper_bursttolerance(peakincrement, jittertolerance, sustainedincrement, maximumburstsize);
        diminuto_shaper_init(&shaper, peakincrement, jittertolerance, sustainedincrement, bursttolerance, epoch);
    }

    /* WORKLOOP */

    while (!0) {

        /* PEAK */

        now = diminuto_shaper_now();
        if (count > 1) {
            elapsed = now - then;
            if (elapsed > 0) {
                rate = size;
                rate *= frequency;
                rate /= elapsed;
                if (rate > peak) {
                    peak = rate;
                }
            }
        }
        then = now;

        /* INPUT */

        if ((rc = input(buffer, blocksize)) > 0) {
            size = rc;
        } else if (rc == 0) {
            break;
        } else {
            return 3;
        }

        /* BURST */

        count += 1;
        total += size;
        if (size > burst) {
            burst = size;
        }

        /* CHECKSUM */

        fletcher16c = diminuto_fletcher_16(buffer, size, &fletcher16a, &fletcher16b);

        /* SHAPE */

        now = diminuto_shaper_now();
        if (shaped) {
            delay = 0.0;
            while (!0) {
                interval = diminuto_shaper_request(&shaper, now);
                if (interval <= 0) {
                    break;
                } else if (interval < minimum) {
                    diminuto_yield();
                } else {
                    diminuto_delay_uninterruptible(interval);
                }
                delay += (double)interval / (double)frequency;
                now = diminuto_shaper_now();
            }
            diminuto_shaper_commitn(&shaper, size);
        }

        if (verbose) {
            fprintf(stderr, "%s: count=%zuio size=%zuB total=%zuB delay=%lfs fletcher16=0x%4.4x\n", program, count, size, total, delay, fletcher16c);
        }

        /* SUSTAINED */

        duration = now - epoch;
        if (duration > 0) {
            rate = total;
            rate *= frequency;
            rate /= duration;
            sustained = rate;
        }

        /* OUTPUT */

        if ((rc = output(buffer, size)) > 0) {
            /* Do nothing. */
        } else if (rc == 0) {
            break;
        } else {
            return 4;
        }

    }

    /* TIMELINE */

    if (shaped) {
        interval = diminuto_shaper_getexpected(&shaper);
        if (interval <= 0) {
            /* Do nothing. */
        } else if (interval < minimum) {
            diminuto_yield();
        } else {
            diminuto_delay_uninterruptible(interval);
        }
        now = diminuto_shaper_now();
        diminuto_shaper_update(&shaper, now);
    }

    /* SUSTAINED */

    duration = now - epoch;
    if (elapsed > 0) {
        rate = total;
        rate *= frequency;
        rate /= duration;
        sustained = rate;
    }

    /* REPORT */

    report();

    diminuto_shaper_fini(&shaper);

    return 0;
}
