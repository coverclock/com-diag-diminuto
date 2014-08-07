/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * EXAMPLE (for stampede)
 *
 * pintool -p 160 -x -o -H -r -s -r -u 5000000 -r -c -r -p 161 -x -h -L -N -r -w 0 -r -u 5000000 -r -w 1 -r -p 160 -n -p 161 -n
 *
 * ABSTRACT
 *
 * Allows manipulation of general purpose input/output (GPIO) pins using the
 * lower level calls of the pin facility. Usees the /sys GPIO driver interface.
 * Probably must be run as root.
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_string.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

static void usage(const char * program)
{
	fprintf(stderr, "usage: %s [ -d ] [ -D PATH ] -p PIN [ -x ] [ -i | -o | -h | -l ] [ -N | -R | -F | -B ] [ -r ] [ -w BOOLEAN | -s | -c ] [ -t | -f ] [ -U MICROSECONDS ] [ -n ] [ ... ]\n", program);
	fprintf(stderr, "       -B            Set PIN edge to both\n");
	fprintf(stderr, "       -D PATH       Use PATH instead of /sys for subsequent operations\n");
	fprintf(stderr, "       -F            Set PIN edge to falling\n");
	fprintf(stderr, "       -H            Set PIN active to high\n");
	fprintf(stderr, "       -L            Set PIN active to low\n");
	fprintf(stderr, "       -N            Set PIN edge to none\n");
	fprintf(stderr, "       -R            Set PIN edge to rising\n");
	fprintf(stderr, "       -c            Clear PIN by writing 0\n");
	fprintf(stderr, "       -d            Enable debug mode\n");
	fprintf(stderr, "       -f            Proceed if the last PIN state was 0\n");
	fprintf(stderr, "       -h            Set PIN direction to output and write !0\n");
	fprintf(stderr, "       -i            Set PIN direction to input\n");
	fprintf(stderr, "       -l            Set PIN direction to output and write 0\n");
	fprintf(stderr, "       -n            Unexport PIN\n");
	fprintf(stderr, "       -o            Set PIN direction to output\n");
	fprintf(stderr, "       -p PIN        Use PIN for subsequent operations\n");
	fprintf(stderr, "       -r            Read PIN and print to standard output\n");
	fprintf(stderr, "       -s            Set PIN by writing !0\n");
	fprintf(stderr, "       -t            Proceed if the last PIN state was !0\n");
	fprintf(stderr, "       -u USECONDS   Delay for USECONDS microseconds\n");
	fprintf(stderr, "       -w BOOLEAN    Write BOOLEAN to PIN\n");
	fprintf(stderr, "       -x            Export PIN\n");
	fprintf(stderr, "       -?            Print menu\n");
}

int main(int argc, char * argv[])
{
	int rc = 0;
	const char * program = "pintool";
	int done = 0;
	int error = 0;
	int debug = 0;
	FILE * fp = (FILE *)0;
	diminuto_unsigned_t value = 0;
	int pin = -1;
	int state = 0;
	const char * path = "/sys";
	char opts[2] = { '\0', '\0' };
	int opt;
	extern char * optarg;

	program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

	while ((opt = getopt(argc, argv, "BD:FHLNRcdfhilnop:rstu:vw:x?")) >= 0) {

		opts[0] = opt;

		switch (opt) {

		case 'B':
			if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
			if (pin < 0) {
				errno = EINVAL;
				perror(opts);
				error = !0;
				break;
			} else if (diminuto_pin_edge(pin, DIMINUTO_PIN_EDGE_BOTH) < 0) {
				error = !0;
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
			if (pin < 0) {
				errno = EINVAL;
				perror(opts);
				error = !0;
				break;
			} else if (diminuto_pin_edge(pin, DIMINUTO_PIN_EDGE_FALLING) < 0) {
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
			if (pin < 0) {
				errno = EINVAL;
				perror(opts);
				error = !0;
				break;
			} else if (diminuto_pin_edge(pin, DIMINUTO_PIN_EDGE_NONE) < 0) {
				error = !0;
				break;
			} else {
				/* Do nothing. */
			}
			break;

		case 'R':
			if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
			if (pin < 0) {
				errno = EINVAL;
				perror(opts);
				error = !0;
				break;
			} else if (diminuto_pin_edge(pin, DIMINUTO_PIN_EDGE_RISING) < 0) {
				error = !0;
				break;
			} else {
				/* Do nothing. */
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
				error = !0;
				break;
			} else {
				/* Do nothing. */
			}
			if (diminuto_pin_put(fp, state) < 0) {
				error = !0;
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
				error = !0;
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
				error = !0;
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
				error = !0;
				break;
			} else {
				/* Do nothing. */
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
				error = !0;
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
				error = !0;
				break;
			} else {
				/* Do nothing. */
			}
			break;

		case 'p':
			if (*diminuto_number(optarg, &value) != '\0') {
				perror(optarg);
				error = !0;
				break;
			} else {
				pin = value;
				if (debug) { fprintf(stderr, "%s -%c %d\n", program, opt, pin); }
				if (pin < 0) {
					errno = EINVAL;
					perror(optarg);
					error = !0;
					break;
				} else if (fp == (FILE *)0) {
					/* Do nothing. */
				} else if ((fp = diminuto_pin_close(fp)) != (FILE *)0) {
					error = !0;
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
			} else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
				error = !0;
				break;
			} else {
				/* Do nothing. */
			}
			if ((state = diminuto_pin_get(fp)) < 0) {
				error = !0;
				break;
			} else {
				state = !!state;
				printf("%d\n", state);
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
				error = !0;
				break;
			} else {
				/* Do nothing. */
			}
			if (diminuto_pin_put(fp, state) < 0) {
				error = !0;
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
			if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
				perror(optarg);
			} else {
				if (debug) { fprintf(stderr, "%s -%c %llu\n", program, opt, value); }
				value *= diminuto_delay_frequency();
				value /= 1000000;
				diminuto_delay(value, 0);
			}
			break;

		case 'w':
			if (*diminuto_number(optarg, &value) != '\0') {
				perror(optarg);
				error = !0;
				break;
			} else {
				state = !!value;
				if (debug) { fprintf(stderr, "%s -%c %d\n", program, opt, state); }
				if (fp != (FILE *)0) {
					/* Do nothing. */
				} else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
					error = !0;
					break;
				} else {
					/* Do nothing. */
				}
				if (diminuto_pin_put(fp, state) < 0) {
					error = !0;
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
				error = !0;
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
			rc = 1;
			break;
		}

		if (done) {
			break;
		}

	}

	diminuto_pin_close(fp);

	return rc;
}
