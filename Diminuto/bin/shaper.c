/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * ipcalc [ ipaddress [ netmask [ hostpart ] ] ]<BR>
 *
 * EXAMPLES
 *
 * shaper < INPUT_FILE > OUTPUT_FILE
 *
 * source | shaper -p PEAK_BYTES_PER_SECOND -j JITTER_TOLERANCE_MICROSECONDS -s SUSTAINED_BYTES_PER_SECOND -m MAXIMUM_BURST_SIZE_BYTES | sink
 *
 * ABSTRACT
 *
 * Implements both a traffic shaper and a traffic throughput measurer.
 * Throughput is always measured and reported at EOF of the incoming stream
 * or upon receipt of SIGHUP, SIGINT, SIGTERM, or SIGQUIT. If traffic shaping
 * parameters are provided on the command line, traffic is shaped to the
 * specified contract.
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

static const size_t BLOCKSIZE = 32768;
static const char * program = "shaper";
static pid_t pid = 0;
static double peak = 0.0;
static size_t total = 0;
static size_t burst = 0;
static diminuto_ticks_t frequency = 0;
static diminuto_ticks_t duration = 0;

static void report(void)
{
    double elapsed;
    double sustained;

    fprintf(stderr, "shaper[%d]: total=%zu\n", pid, total);
    elapsed = duration;
    elapsed /= frequency;
    fprintf(stderr, "shaper[%d]: elapsed=%lf\n", pid, elapsed);
    fprintf(stderr, "shaper[%d]: peak=%lf\n", pid, peak);
    if (duration > 0) {
        sustained = total;
        sustained /= elapsed;
        fprintf(stderr, "shaper[%d]: sustained=%lf\n", pid, sustained);
    }
    fprintf(stderr, "shaper[%d]: burst=%zu\n", pid, burst);
}

static void handler(int signum)
{
    report();
    if ((signum == SIGINT) || (signum == SIGTERM) || (signum == SIGQUIT)) {
        exit(0);
    }
}

int main(int argc, char * argv[])
{
	int debug = 0;
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
    size_t size;
    double instantaneous;
    double elapsed;
    size_t blocksize;
    uint8_t * buffer;
    struct sigaction action;
    int opt;
    extern char * optarg;
    char * end;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;
    pid = getpid();
    frequency = diminuto_frequency();
    blocksize = BLOCKSIZE;

    while ((opt = getopt(argc, argv, "dp:j:s:m:")) >= 0) {

        switch (opt) {

        case 'd':
            debug = !0;
            break;

        case 'p':
            peakrate = strtoull(optarg, &end, 0);
            if ((peakrate == 0) || (*end != '\0')) {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        case 'j':
            jittertolerance = strtoull(optarg, &end, 0);
            if (*end != '\0') {
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

        case 'm':
            maximumburstsize = strtoull(optarg, &end, 0);
            if (*end != '\0') {
                errno = EINVAL;
                perror(optarg);
                return 1;
            }
            break;

        default:
            fprintf(stderr, "usage: %s [ -p BYTESPERSECOND ] [ -j USECONDS ] [ -s BYTESPERSECOND ] [ -m BYTES ]\n", program);
            fprintf(stderr, "       -p BYTESPERSECOND    Set the peak rate in bytes per second\n");
            fprintf(stderr, "       -j USECONDS          Set the jitter tolerance in microseconds\n");
            fprintf(stderr, "       -s BYTESPERSECOND    Set the sustained rate in bytes per second\n");
            fprintf(stderr, "       -m BYTES             Set the maximum burst size in bytes\n");
            return 1;
            break;

        }

    }

    if (sustainedrate == 0) {
        sustainedrate = peakrate;
        maximumburstsize = 0;
    }

    if (peakrate == 0) {
        peakrate = sustainedrate;
        maximumburstsize = 0;
    }

    if (maximumburstsize == 0) {
        peakrate = sustainedrate;
    }

    if (sustainedrate > peakrate) {
        errno = EINVAL;
        perror("-s");
        return 1;
    }

    if (debug) {
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

    then = diminuto_time_elapsed();
    if (shaped) {
        diminuto_shaper_init(&shaper, peakrate, jittertolerance, sustainedrate, maximumburstsize, then);
        diminuto_shaper_log(&shaper);
    }

    while (!0) {
        if ((size = fread(buffer, 1, blocksize, stdin)) > 0) {
            /* Do nothing. */
        } else if (feof(stdin)) {
            break;
        } else if (ferror(stdin)) {
            errno = EIO;
            perror("fread");
            return 3;
        } else {
            errno = EINVAL;
            perror("fread");
            return 3;
        }
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
        instantaneous = size;
        interval = now - then;
        if (interval > 0) {
            elapsed = interval;
            elapsed /= frequency;
            instantaneous = total;
            instantaneous /= elapsed;
            if (instantaneous > peak) {
                peak = instantaneous;
            }
        }
        total += size;
        if (size > burst) {
            burst = size;
        }
        duration += interval;
        then = now;
        if ((size = fwrite(buffer, size, 1, stdout)) == size) {
            /* Do nothing. */
        } else if (feof(stdout)) {
            break;
        } else if (ferror(stdout)) {
            errno = EIO;
            perror("fwrite");
            return 3;
        } else {
            errno = EINVAL;
            perror("fwrite");
            return 3;
        }
    }

    report();

    return 0;
}
