/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * shaper [ -d ] [ -b BYTES ] [ -p BYTESPERSECOND ] [ -j USECONDS ] [ -s BYTESPERSECOND ] [ -m BYTES ]
 *
 * Consider letting the jitter tolerance ("-j") default to zero microseconds
 * and setting the maximum burst size ("-m") to the same value in bytes as the
 * I/O block size ("-b").
 *
 * EXAMPLES
 *
 * source | shaper | sink
 *
 * source | shaper -b 4096 -p 2048 -s 1024 -m 4096 | sink
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
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_types.h"
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

static const size_t BLOCKSIZE = 32768;
static const diminuto_ticks_t JITTERTOLERANCE = 0;

static io_t io = BLOCK;
static const char * program = (const char *)0;
static pid_t pid = 0;
static size_t total = 0;
static size_t burst = 0;
static diminuto_ticks_t frequency = 0;
static diminuto_ticks_t epoch = 0;
static diminuto_ticks_t duration = 0;
static int64_t peak = -1;
static int64_t sustained = -1;
static int debug = 0;

static void report(void)
{
    fprintf(stderr, "shaper[%d]: total=%zu bytes\n", pid, total);
    fprintf(stderr, "shaper[%d]: elapsed=%lld seconds\n", pid, duration / frequency);
    fprintf(stderr, "shaper[%d]: peak=%lld bytes per second\n", pid, peak);
    fprintf(stderr, "shaper[%d]: sustained=%lld bytes per second\n", pid, sustained);
    fprintf(stderr, "shaper[%d]: burst=%zu bytes\n", pid, burst);
}

static void handler(int signum)
{
    report();
    if ((signum == SIGINT) || (signum == SIGTERM) || (signum == SIGQUIT)) {
        exit(0);
    }
}

static ssize_t input(uint8_t * buffer, size_t size)
{
    ssize_t rc = 0;
    int ch;

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
        if (fgets(buffer, size, stdin) != NULL) {
            buffer[size - 1] = '\0';
            rc = strlen(buffer);
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
    int ch;

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
        if (fputs(buffer, stdout) > 0) {
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
    diminuto_ticks_t jittertolerance = 0;
    size_t sustainedrate = 0;
    size_t maximumburstsize = 0;
    int shaped = 0;
    diminuto_ticks_t now;
    diminuto_ticks_t then;
    diminuto_ticks_t interval;
    diminuto_ticks_t minimum;
    ssize_t rc;
    size_t size;
    int64_t rate;
    uint64_t elapsed;
    size_t blocksize;
    uint8_t * buffer;
    struct sigaction action;
    int opt;
    extern char * optarg;
    char * end;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    pid = getpid();
    frequency = diminuto_frequency();

    io = BLOCK;
    blocksize = BLOCKSIZE;
    jittertolerance = JITTERTOLERANCE;

    while ((opt = getopt(argc, argv, "b:cdj:lm:p:s:")) >= 0) {

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

        default:
            fprintf(stderr, "usage: %s [ -d ] [ -l | -c ] [ -b BYTES ] [ -p BYTESPERSECOND ] [ -j USECONDS ] [ -s BYTESPERSECOND ] [ -m BYTES ]\n", program);
            fprintf(stderr, "       -b BYTES             Use an I/O buffer of size BYTES instead of %zu bytes.\n", blocksize);
            fprintf(stderr, "       -c                   Do character I/O instead of block I/O.\n");
            fprintf(stderr, "       -d                   Enable debugging output.\n");
            fprintf(stderr, "       -j USECONDS          Use a jitter tolerance of USECONDS instead of %lld microseconds.\n", jittertolerance);
            fprintf(stderr, "       -l                   Do line I/O instead of block I/O.\n");
            fprintf(stderr, "       -p BYTESPERSECOND    Set the peak rate to BYTESPERSECOND bytes per second.\n");
            fprintf(stderr, "       -m BYTES             Set the maximum burst size to BYTES bytes.\n");
            fprintf(stderr, "       -s BYTESPERSECOND    Set the sustained rate to BYTESPERSECOND bytes per second.\n");
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
        switch (io) {
        case BLOCK:
            break;
        case CHARACTER:
            fprintf(stderr, "shaper[%d]: -c\n", pid);
            break;
        case LINE:
            fprintf(stderr, "shaper[%d]: -l\n", pid);
            break;
        default:
            break;
        }
        fprintf(stderr, "shaper[%d]: -b %zu\n", pid, blocksize);
        fprintf(stderr, "shaper[%d]: -p %zu\n", pid, peakrate);
        fprintf(stderr, "shaper[%d]: -j %lld\n", pid, jittertolerance);
        fprintf(stderr, "shaper[%d]: -s %zu\n", pid, sustainedrate);
        fprintf(stderr, "shaper[%d]: -m %zu\n", pid, maximumburstsize);
    }

    shaped = (peakrate > 0) && (sustainedrate > 0) && (sustainedrate <= peakrate);

    minimum = frequency / diminuto_delay_frequency();

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
        perror("sigaction(SIGTERM)");
        return 2;
    }
    if (sigaction(SIGTERM, &action, (struct sigaction *)0) < 0) {
        perror("sigaction(SIGTERM)");
        return 2;
    }

    epoch = then = diminuto_time_elapsed();
    if (shaped) {
        diminuto_shaper_init(&shaper, peakrate, diminuto_frequency_units2ticks(jittertolerance, 1000000LL), sustainedrate, maximumburstsize, then);
    }

    while (!0) {

        /* INPUT */

        if ((rc = input(buffer, blocksize)) > 0) {
            size = rc;
        } else if (rc == 0) {
            break;
        } else {
            return 3;
        }

        /* SHAPE */

        now = diminuto_time_elapsed();
        if (shaped) {
            while (!0) {
                interval = diminuto_shaper_request(&shaper, now);
                if (interval <= 0) {
                    break;
                } else if (interval < minimum) {
                    diminuto_yield();
                } else {
                    diminuto_delay_uninterruptible(interval);
                }
                now = diminuto_time_elapsed();
            }
            diminuto_shaper_commitn(&shaper, size);
        }

        /* OUTPUT */

        if ((rc = output(buffer, size)) > 0) {
            /* Do nothing. */
        } else if (rc == 0) {
            break;
        } else {
            return 4;
        }

        /* MEASURE */

        duration = now - epoch;
        interval = now - then;
        then = now;

        total += size;
        if (size > burst) {
            burst = size;
        }

        if (total > size) {
            elapsed = interval;
            elapsed /= frequency;
            if (elapsed > 0) {
                rate = size;
                rate /= elapsed;
                if (rate > peak) {
                    peak = rate;
                }
            }
            elapsed = duration;
            elapsed /= frequency;
            if (elapsed > 0) {
                rate = total;
                rate /= elapsed;
                sustained = rate;
            }
        }

    }

    report();

    return 0;
}
