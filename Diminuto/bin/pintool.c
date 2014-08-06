/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * gpiotool PIN [ VALUE ]
 *
 * ABSTRACT
 *
 * Allows simple manipulation of general purpose input/output (GPIO) pins.
 * Should probably only be run as root.
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_string.h"
#include <stdio.h>
#include <errno.h>

static void usage(const char * program)
{
    fprintf(stderr, "usage: %s [ -d ] [ -p PIN ] [ -e ] [ -u ] [ -i ] [ -o ] [ -r ] [ -t ] [ -f ] [ -w BOOLEAN ] [ -s ] [ -c ] [ -u MICROSECONDS ] [ ... ]\n", program);
    fprintf(stderr, "       -c            Write 0 to PIN\n");
    fprintf(stderr, "       -d            Enable debug mode\n");
    fprintf(stderr, "       -e            Export PIN\n");
    fprintf(stderr, "       -f            Proceed if the last result was 0\n");
    fprintf(stderr, "       -i            Set PIN direction to input\n");
    fprintf(stderr, "       -o            Set PIN direction to output\n");
    fprintf(stderr, "       -p PIN        Use PIN for subsequent operations\n");
    fprintf(stderr, "       -r            Read PIN\n");
    fprintf(stderr, "       -s            Write 1 to PIN\n");
    fprintf(stderr, "       -t            Proceed if the last result was !0\n");
    fprintf(stderr, "       -u USECONDS   Sleep for USECONDS microseconds\n");
    fprintf(stderr, "       -w BOOLEAN    Write BOOLEAN to PIN\n");
    fprintf(stderr, "       -?            Print menu\n");
}

int main(int argc, char * argv[])
{
	int rc = 1;
	const char * program = "gpiotool";
	FILE * fp = (FILE *)0;
	diminuto_unsigned_t pin;
	diminuto_unsigned_t state;

	do {

		if (argc < 2) {
			fprintf(stderr, "usage: %s PIN [ VALUE ]\n", argv[0]);
			break;
		}

		if (*diminuto_number_signed(argv[1], &pin) != '\0') {
			errno = EINVAL;
			diminuto_perror(argv[1]);
			break;
		}

		if (argc != 2) {
			/* Do nothing. */
		} else if ((fp = diminuto_pin_input(pin)) == (FILE *)0) {
			break;
		} else if ((state = diminuto_pin_get(fp)) < 0) {
			break;
		} else {
			printf("%d\n", !!state);
			rc = 0;
			break;
		}

		if (*diminuto_number_signed(argv[2], &state) != '\0') {
			errno = EINVAL;
			diminuto_perror(argv[2]);
			rc = 1;
			break;
		}

		if (argc != 3) {
			/* Do nothing. */
		} else if ((fp = diminuto_pin_output(pin)) == (FILE *)0) {
			break;
		} else if (diminuto_pin_put(fp, !!state) < 0) {
			break;
		} else {
			rc = 0;
			break;
		}

	} while (0);

	diminuto_pin_unused(fp, pin);

	return rc;
}
