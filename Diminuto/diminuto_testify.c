/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <string.h>

int diminuto_testify(const char * testimony, int otherwise) {
	int result;
	char * endp;

	result = !!otherwise;

	if (testimony != (const char *)0) {
		switch (testimony[0]) {
		case 'a': case 'A': /* asserted */
		case 'c': case 'C': /* closed, confirmed */
		case 'e': case 'E': /* enabled */
		case 'h': case 'H': /* high */
		case 't': case 'T': /* true */
		case 'y': case 'Y': /* yes */
			result = !0;
			break;
		case 'd': case 'D': /* disabled, deasserted, denied */
		case 'f': case 'F': /* false */
		case 'n': case 'N': /* no */
		case 'l': case 'L': /* low */
			result = 0;
			break;
		case 'o': case 'O':
			switch (testimony[1]) {
			case 'n': case 'N': /* on */
				result = !0;
				break;
			case 'f': case 'F': /* off */
			case 'p': case 'P': /* open */
				result = 0;
				break;
			default:
				errno = EINVAL;
				diminuto_perror(testimony);
				break;
			}
			break;
		case '0': /* 0, 01, 0x1, etc. */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			result = (strtol(testimony, &endp, 0) != 0);
			break;
		case '\0':
			break;
		default:
			errno = EINVAL;
			diminuto_perror(testimony);
			break;
		}
	}

	return result;
}
