/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

static const char * SYS_LINE_SIZE[] = {
	"/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size",
	"/sys/devices/system/cpu/cpu0/cache/coherency_line_size",
};

size_t diminuto_memory_pagesize(int * methodp)
{
	ssize_t pagesize = 0;
	int method = -1;

	do {

#if defined(COM_DIAG_DIMINUTO_MEMORY_PAGESIZE_BYTES)

		if ((pagesize = COM_DIAG_DIMINUTO_MEMORY_PAGESIZE_BYTES) > 0) {
			method = 1;
			break;
		} else if (pagesize < 0) {
			diminuto_perror("COM_DIAG_DIMINUTO_MEMORY_PAGESIZE_BYTES");
		} else {
			/* Do nothing. */
		}

#endif

#if defined(_SC_PAGESIZE)

		if ((pagesize = sysconf(_SC_PAGESIZE)) > 0) {
			method = 2;
			break;
		} else if (pagesize < 0) {
			diminuto_perror("sysconf(_SC_PAGESIZE)");
		} else {
			/* Do nothing. */
		}

#endif

		if ((pagesize = getpagesize()) > 0) {
			method = 3;
			break;
		} else if (pagesize < 0) {
			diminuto_perror("getpagesize");
		} else {
			/* Do nothing. */
		}

		pagesize = DIMINUTO_MEMORY_PAGESIZE_BYTES;
		method = 0;

	} while (0);

	if (methodp != (int *)0) {
		*methodp = method;
	}

	return pagesize;
}

size_t diminuto_memory_linesize(int * methodp)
{
	ssize_t linesize = 0;
	int method = -1;
	FILE * fp;
	unsigned int ii;

	do {

#if defined(COM_DIAG_DIMINUTO_MEMORY_LINESIZE_BYTES)

		if ((linesize = COM_DIAG_DIMINUTO_MEMORY_LINESIZE_BYTES) > 0) {
			method = 1;
			break;
		} else if (linesize < 0) {
			diminuto_perror("COM_DIAG_DIMINUTO_MEMORY_LINESIZE_BYTES");
		} else {
			/* Do nothing. */
		}

#endif

#if defined(_SC_LEVEL1_DCACHE_LINESIZE)

		if ((linesize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE)) > 0) {
			method = 2;
			break;
		} else if (linesize < 0) {
			diminuto_perror("sysconf(_SC_LEVEL1_DCACHE_LINESIZE)");
		} else {
			/* Do nothing. */
		}

#endif

		for (ii = 0; ii < countof(SYS_LINE_SIZE); ++ii) {

			if ((fp = fopen(SYS_LINE_SIZE[ii], "r")) == (FILE *)0) {
				continue;
			}

			if ((fscanf(fp, "%zd", &linesize) <= 0) || (linesize < 0)) {
				errno = EINVAL;
				diminuto_perror(SYS_LINE_SIZE[ii]);
			}

			fclose(fp);

			if (linesize > 0) {
				method = 3 + ii;
				break;
			}

		}

		if (linesize > 0) {
			break;
		}

		linesize = DIMINUTO_MEMORY_LINESIZE_BYTES;
		method = 0;

	} while (0);

	if (methodp != (int *)0) {
		*methodp = method;
	}

	return linesize;
}

size_t diminuto_memory_power(size_t alignment)
{
	size_t power = 1;

	while ((0 < power) && (power < alignment)) {
		power <<= 1;
	}

	return power;
}

int diminuto_memory_is_power(size_t value)
{
	int bits = 0;

	while (value > 0) {
		if ((value & 1) != 0) {
			++bits;
		}
		value >>= 1;
	}

	return (bits == 1);
}

size_t diminuto_memory_alignment(size_t size, size_t alignment)
{
	--alignment;
	return (size + alignment) & (~alignment);
}
