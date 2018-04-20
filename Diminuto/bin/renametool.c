/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Uses the renameat2(2) system call to either atomically rename an existing
 * file to a name that is guaranteed not to already exist, or to atomically
 * swap names of two existing files. If renameat2(2) does its job, there should
 * be no window during which a partial result of either operation can be
 * observed PROVIDING both the first and the second files are both in the same
 * file system (the underlying renameat2(2) system call checks for this.
 *
 * This is particularly useful when used in conjunction with mktemp(1).
 *
 * renameat2(2) was introduced around Linux 3.15, but has not been supported
 * by GNU libc as of version 6.
 */

#include "diminuto_renameat2.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
	int xc = 0;
    int rc = -1;
    int exchange = 0;
    const char * program = "renametool";
    const char * fromfile = (const char *)0;
    const char * tofile = (const char *)0;
    int ndx = 0;

    do {

		program = ((program = strrchr(argv[ndx], '/')) == (char *)0) ? argv[ndx] : program + 1;
		ndx += 1;

		if (argc < 3) {
			fprintf(stderr, "usage: %s [ -x ] FROMFILE TOFILE\n", program);
			xc = 2;
			break;
		}

		if (strcmp(argv[ndx], "-x") == 0) {
			exchange = !0;
			ndx += 1;
		}

		if (argc != (ndx + 2)) {
			fprintf(stderr, "usage: %s [ -x ] FROMFILE TOFILE\n", program);
			xc = 2;
			break;
		}

		fromfile = argv[ndx++];
		tofile = argv[ndx++];

		if (renameat2(AT_FDCWD, fromfile, AT_FDCWD, tofile, exchange ? RENAME_EXCHANGE : RENAME_NOREPLACE) < 0) {
			perror("renameat2");
			xc = 1;
			break;
		}

    } while (0);

    exit(xc);
}
