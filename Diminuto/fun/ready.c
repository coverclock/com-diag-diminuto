/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2019 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This may be useful to run with strace(1) to watch the actual I/Os.
 */

#include "com/diag/diminuto/diminuto_file.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>

int main(int argc, char * argv[])
{
	const char * name;
	FILE * fp;
	int fd;
	ssize_t bytes;
	ssize_t maximum;
	int ch;
	uint64_t index;

	name = ((name = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : name + 1;

	if (argc < 2) {
		fp = stdin;
	} else if (strcmp(argv[1], "-") == 0) {
		fp = stdin;
	} else if ((fp = fopen(argv[1], "r")) == (FILE *)0) {
		perror(argv[1]);
		return 1;
	} else {
		/* Do nothing. */
	}

	fd = fileno(fp);
	index = 0;
	maximum = 0;

	while (!0) {
		bytes = diminuto_file_ready(fp);
		if (bytes > maximum) { maximum = bytes; }
		ch = fgetc(fp);
		if (ch == EOF) {
			break;
		} else if (isprint(ch)) {
			fprintf(stderr, "%s: %2d %10lld %4lld %4lld 0x%02x '%c'\n", name, fd, (unsigned long long)index, (long long)maximum, (long long)bytes, ch, ch);
		} else {
			fprintf(stderr, "%s: %2d %10lld %4lld %4lld 0x%02x\n", name, fd, (unsigned long long)index, (long long)maximum, (long long)bytes, ch);
		}
		fputc(ch, stdout);
		index += 1;
	}

	return 0;
}
