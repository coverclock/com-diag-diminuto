/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * EXAMPLES
 *
 * pintool -p 20 -x -i -b 10000
 * pintool -p 20 -x -i; pintool -b 10000
 * pintool -p 20 -x -i -B -U -M
 * pintool -p 20 -x -i -B -U -m 1000000
 * pintool -p 160 -x -o -H -r -s -r -u 5000000 -r -c -r -p 161 -x -h -r -w 0 -r -u 5000000 -r -w 1 -r -p 160 -n -p 161 -n
 *
 * ABSTRACT
 *
 * Allows manipulation of general purpose input/output (GPIO) pins using the
 * lower level calls of the pin facility. Uses the sysfs GPIO driver interface.
 * Probably must be run as root.
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_string.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_poll.h"
#include "com/diag/diminuto/diminuto_cue.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

static void usage(const char * program)
{
    fprintf(stderr, "usage: %s [ -d ] [ -S ] [ -D PATH ] -p PIN [ -x ] [ -i | -o ] [ -h | -l ] [ -N | -R | -F | -B ] [ -1 ] [ -b ] [ -X COMMAND ] [ -r | -m USECONDS | -M | -b USECONDS | -w BOOLEAN | -s | -c ] [ -U ] [ -t | -f ] [ -u USECONDS ] [ -n ] [ ... ]\n", program);
    fprintf(stderr, "       -1            Read PIN initially when multiplexing.\n");
    fprintf(stderr, "       -B            Set PIN edge to both.\n");
    fprintf(stderr, "       -D PATH       Use PATH instead of /sys for subsequent operations.\n");
    fprintf(stderr, "       -F            Set PIN edge to falling.\n");
    fprintf(stderr, "       -H            Set PIN active to high.\n");
    fprintf(stderr, "       -L            Set PIN active to low.\n");
    fprintf(stderr, "       -M            Multiplex upon PIN edge.\n");
    fprintf(stderr, "       -N            Set PIN edge to none.\n");
    fprintf(stderr, "       -R            Set PIN edge to rising.\n");
    fprintf(stderr, "       -S            Daemonize.\n");
    fprintf(stderr, "       -U            Filter out non-unique edge changes.\n");
    fprintf(stderr, "       -X COMMAND    Execute COMMAND PIN STATE PRIOR in shell when multiplexed edge changes.\n");
    fprintf(stderr, "       -b USECONDS   Poll with debounce every USECONDS (try 10000) microseconds.\n");
    fprintf(stderr, "       -c            Clear PIN by writing 0.\n");
    fprintf(stderr, "       -d            Enable debug mode.\n");
    fprintf(stderr, "       -f            Proceed if the last PIN state was 0.\n");
    fprintf(stderr, "       -h            Set PIN direction to output and write !0.\n");
    fprintf(stderr, "       -i            Set PIN direction to input.\n");
    fprintf(stderr, "       -l            Set PIN direction to output and write 0.\n");
    fprintf(stderr, "       -m USECONDS   Multiplex upon PIN edge or until USECONDS microseconds.\n");
    fprintf(stderr, "       -n            Unexport PIN.\n");
    fprintf(stderr, "       -o            Set PIN direction to output.\n");
    fprintf(stderr, "       -p PIN        Use PIN for subsequent operations.\n");
    fprintf(stderr, "       -r            Read PIN and print to standard output.\n");
    fprintf(stderr, "       -s            Set PIN by writing !0.\n");
    fprintf(stderr, "       -t            Proceed if the last PIN state was !0.\n");
    fprintf(stderr, "       -u USECONDS   Delay for USECONDS microseconds.\n");
    fprintf(stderr, "       -w BOOLEAN    Write BOOLEAN to PIN.\n");
    fprintf(stderr, "       -x            Export PIN.\n");
    fprintf(stderr, "       -?            Print menu.\n");
}

int main(int argc, char * argv[])
{
    int xc = 0;
    const char * program = "pintool";
    int done = 0;
    int fail = 0;
    int error = 0;
    int debug = 0;
    int first = 0;
    int unique = 0;
    int pin = -1;
    int state = 0;
    int prior = 0;
    int act = 0;
    FILE * fp = (FILE *)0;
    const char * path = "/sys";
    const char * command = (const char *)0;
    diminuto_unsigned_t uvalue = 0;
    diminuto_signed_t svalue = -1;
    diminuto_mux_t mux = { 0 };
    diminuto_ticks_t ticks = 0;
    diminuto_sticks_t sticks = -2;
    diminuto_cue_state_t cue = { 0 };
    diminuto_pin_edge_t edge = DIMINUTO_PIN_EDGE_NONE;
    int fd = -1;
    int nfds = 0;
    int rc = -1;
    char opts[2] = { '\0', '\0' };
	char buffer[1024] = { '\0' };
    int opt = '\0';
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "1BD:FHLMNRSUX:b:cdfhilm:nop:rstu:vw:x?")) >= 0) {

        opts[0] = opt;

        uvalue = 0;
        svalue = -1;

        switch (opt) {

        case '1':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            first = !0;
            break;

        case 'B':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            edge = DIMINUTO_PIN_EDGE_BOTH;
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_edge(pin, edge) < 0) {
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'D':
            if (debug) { fprintf(stderr, "%s -%c \"%s\"\n", program, opt, optarg); }
            path = diminuto_pin_debug(optarg);
            break;

        case 'F':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            edge = DIMINUTO_PIN_EDGE_FALLING;
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_edge(pin, edge) < 0) {
                error = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'H':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_active(pin, !0) < 0) {
                error = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'L':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_active(pin, 0) < 0) {
                error = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'N':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            edge = DIMINUTO_PIN_EDGE_NONE;
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_edge(pin, edge) < 0) {
                error = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'R':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            edge = DIMINUTO_PIN_EDGE_RISING;
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_edge(pin, edge) < 0) {
                error = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'S':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            rc = diminuto_daemon(program);
            if (rc < 0) {
            	fail = !0;
            }
            break;

        case 'U':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            unique = !0;
            break;

        case 'X':
            if (debug) { fprintf(stderr, "%s -%c \"%s\"\n", program, opt, optarg); }
            command = optarg;
            break;

        case 'b':
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                perror(optarg);
                error = !0;
                break;
            }
            if (debug) { fprintf(stderr, "%s -%c %llu\n", program, opt, uvalue); }
            ticks = uvalue;
            ticks *= diminuto_frequency();
            ticks /= 1000000;
            if (fp != (FILE *)0) {
                /* Do nothing. */
            } else if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            if ((state = diminuto_pin_get(fp)) < 0) {
                fail = !0;
                break;
            }
            state = !!state;
            printf("%d\n", state);
            fflush(stdout);
            diminuto_cue_init(&cue, state);
            while (!0) {
                prior = state;
                diminuto_delay(ticks, 0);
                if ((state = diminuto_pin_get(fp)) < 0) {
                    fail = !0;
                    break;
                }
                state = diminuto_cue_debounce(&cue, !!state);
                if (state != prior) {
                    printf("%d\n", state);
                    fflush(stdout);
                }
            }
            if (fail) {
                break;
            }
            break;

        case 'c':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            state = 0;
            if (fp != (FILE *)0) {
                /* Do nothing. */
            } else if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            if (diminuto_pin_put(fp, state) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'd':
            debug = !0;
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            break;

        case 'f':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            done = (state != 0);
            break;

        case 'h':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_initialize(pin, !0) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'i':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_direction(pin, 0) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'l':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_initialize(pin, 0) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'm':
            if ((*diminuto_number_signed(optarg, &svalue) != '\0')) {
                perror(optarg);
                error = !0;
                break;
            } else if (svalue > 0) {
                sticks = svalue;
                sticks *= diminuto_frequency();
                sticks /= 1000000;
            } else {
                sticks = svalue;
            }
            /* no break */

        case 'M':
            if (debug) { fprintf(stderr, "%s -%c %lld\n", program, opt, svalue); }
            if (fp != (FILE *)0) {
                /* Do nothing. */
            } else if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            prior = -1;
            if (!first) {
                /* Do nothing. */
            } else if ((state = diminuto_pin_get(fp)) < 0) {
                fail = !0;
                break;
            } else {
                state = !!state;
                printf("%d\n", state);
                fflush(stdout);
                prior = state;
            }
            fd = fileno(fp);
            diminuto_mux_init(&mux);
            if (diminuto_mux_register_interrupt(&mux, fd) < 0) {
                fail = !0;
                break;
            }
            while (!0) {
                if ((nfds = diminuto_mux_wait(&mux, sticks)) < 0) {
                    fail = !0;
                    break;
                }
                if (nfds > 0) {
                    while (!0) {
                        if ((fd = diminuto_mux_ready_interrupt(&mux)) < 0) {
                            break;
                        } else if (fd != fileno(fp)) {
                            /* Do nothing. */
                        } else if ((state = diminuto_pin_get(fp)) < 0) {
                            fail = !0;
                            break;
                        } else {
                            state = !!state;
                            act = (!unique) || (prior < 0) || (prior != state);
                            if (!act) {
                            	/* Do nothing. */
                            } else if (command != (const char *)0) {
                            	snprintf(buffer, sizeof(buffer), "%s %d %d %d", command, pin, state, prior);
                            	buffer[sizeof(buffer) - 1] = '\0';
                            	rc = diminuto_system(buffer);
                            	if (rc < 0) {
                            		fail = !0;
                            		break;
                            	}
                            } else {
                            	printf("%d\n", state);
                            	fflush(stdout);
                            }
                            prior = state;
                        }
                    }
                    if (fail) {
                        break;
                    }
                } else if ((state = diminuto_pin_get(fp)) < 0) {
                    fail = !0;
                    break;
                } else {
                    state = !!state;
                    act = (!unique) || (prior < 0) || (prior != state);
                    if (!act) {
                    	/* Do nothing. */
                    } else if (command != (const char *)0) {
                    	snprintf(buffer, sizeof(buffer), "%s %d %d %d", command, pin, state, prior);
                    	buffer[sizeof(buffer) - 1] = '\0';
                    	rc = diminuto_system(buffer);
                    	if (rc < 0) {
                    		fail = !0;
                    		break;
                    	}
                    } else {
                    	printf("%d\n", state);
                    	fflush(stdout);
                    }
                    prior = state;
                }
            }
            if (diminuto_mux_unregister_interrupt(&mux, fd) < 0) {
                fail = !0;
                break;
            }
            if (fail) {
                break;
            }
            break;

        case 'n':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_unexport(pin) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'o':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_direction(pin, !0) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 'p':
            if (*diminuto_number_unsigned(optarg, &uvalue) != '\0') {
                perror(optarg);
                error = !0;
                break;
            } else {
                pin = uvalue;
                if (debug) { fprintf(stderr, "%s -%c %d\n", program, opt, pin); }
                if (pin < 0) {
                    errno = EINVAL;
                    perror(optarg);
                    error = !0;
                    break;
                } else if (fp == (FILE *)0) {
                    /* Do nothing. */
                } else if ((fp = diminuto_pin_close(fp)) != (FILE *)0) {
                    fail = !0;
                    break;
                } else {
                    /* Do nothing. */
                }
            }
            break;

        case 'r':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (fp != (FILE *)0) {
                /* Do nothing. */
            } else if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            if ((state = diminuto_pin_get(fp)) < 0) {
                fail = !0;
                break;
            } else {
                state = !!state;
                printf("%d\n", state);
                fflush(stdout);
            }
            break;

        case 's':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            state = !0;
            if (fp != (FILE *)0) {
                /* Do nothing. */
            } else if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            if (diminuto_pin_put(fp, state) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case 't':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            done = (state == 0);
            break;

        case 'u':
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                perror(optarg);
                error = !0;
            } else {
                if (debug) { fprintf(stderr, "%s -%c %llu\n", program, opt, uvalue); }
                ticks = uvalue;
                ticks *= diminuto_frequency();
                ticks /= 1000000;
                diminuto_delay(ticks, 0);
            }
            break;

        case 'w':
            if (*diminuto_number_unsigned(optarg, &uvalue) != '\0') {
                perror(optarg);
                error = !0;
                break;
            } else {
                state = !!uvalue;
                if (debug) { fprintf(stderr, "%s -%c %d\n", program, opt, state); }
                if (fp != (FILE *)0) {
                    /* Do nothing. */
                } else if (pin < 0) {
                    errno = EINVAL;
                    perror(opts);
                    error = !0;
                    break;
                } else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
                    fail = !0;
                    break;
                } else {
                    /* Do nothing. */
                }
                if (diminuto_pin_put(fp, state) < 0) {
                    fail = !0;
                    break;
                } else {
                    /* Do nothing. */
                }
            }
            break;

        case 'x':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (pin < 0) {
                errno = EINVAL;
                perror(opts);
                error = !0;
                break;
            } else if (diminuto_pin_export(pin) < 0) {
                fail = !0;
                break;
            } else {
                /* Do nothing. */
            }
            break;

        case '?':
            usage(program);
            break;

        default:
            error = !0;
            break;

        }

        if (error) {
            usage(program);
            xc = 1;
            break;
        }

        if (fail) {
            xc = 2;
            break;
        }

        if (done) {
            break;
        }

    }

    diminuto_pin_close(fp);

    return xc;
}
