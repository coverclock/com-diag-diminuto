/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_well.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

ssize_t diminuto_well_pagesize()
{
	ssize_t pagesize;

#if defined(DIMINUTO_PAGESIZE)

	pagesize = DIMINUTO_PAGESIZE;

#elif defined(_SC_PAGESIZE)

	if ((pagesize = sysconf(_SC_PAGESIZE)) < 0) {
		diminuto_perror("sysconf(_SC_PAGESIZE)");
		pagesize = DIMINUTO_WELL_PAGESIZE;
	}

#else

	if ((pagesize = getpagesize()) < 0) {
		diminuto_perror("getpagesize");
		pagesize = DIMINUTO_WELL_PAGESIZE;
	}

#endif

	return pagesize;
}

ssize_t diminuto_well_linesize()
{
	ssize_t linesize;

#if defined(DIMINUT_LINESIZE)

	linesize = DIMINUTO_LINESIZE;

#else

	FILE * fp;
	int rc;

	do {

#if defined(_SC_LEVEL1_DCACHE_LINESIZE)

		if ((linesize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE)) >= 0) {
			break;
		}
		diminuto_perror("sysconf(_SC_LEVEL1_DCACHE_LINESIZE)");

#endif

		if ((fp = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r")) == (FILE *)0) {
			/* Do nothing. */
		} else if ((rc = fscanf(fp, "%zd", &linesize)) > 0) {
			/* Do nothing. */
		} else {
			errno = EINVAL;
			diminuto_perror("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size");
		}
		close(fp);
		if (fp == (FILE *)0) {
			/* Do nothing. */
		} else if (rc <= 0) {
			/* Do nothing. */
		} else {
			break;
		}

		if ((fp = fopen("/sys/devices/system/cpu/cpu0/cache/coherency_line_size", "r")) == (FILE *)0) {
			/* Do nothing. */
		} else if ((rc = fscanf(fp, "%zd", &linesize)) > 0) {
			/* Do nothing. */
		} else {
			errno = EINVAL;
			diminuto_perror("/sys/devices/system/cpu/cpu0/cache/coherency_line_size");
		}
		close(fp);
		if (fp == (FILE *)0) {
			/* Do nothing. */
		} else if (rc <= 0) {
			/* Do nothing. */
		} else {
			break;
		}

		linesize = DIMINUTO_WELL_LINESIZE;

	} while (0);

#endif

	return linesize;
}

diminuto_well_t * diminuto_well_init(size_t size, size_t count)
{
	diminuto_well_t * wellp = (diminuto_well_t *)0;
	diminuto_well_t * well;
	size_t pagesize;
	int rc;
	void * control = (void *)0;
	void * data = (void *)0;
	size_t ii;

	do {

		pagesize = diminuto_well_pagesize();
		if (pagesize < 0) {
			errno = EINVAL;
			diminuto_perror("diminuto_well_pagesize");
			break;
		}

		rc = posix_memalign(&control, pagesize, sizeof(diminuto_list_t) * (DIMINUTO_WELL_NODE + count));
		if (rc != 0) {
			errno = rc;
			diminuto_perror("posix_memalign(control)");
			break;
		}

		rc = posix_memalign(&data, pagesize, size * count);
		if (rc != 0) {
			errno = rc;
			diminuto_perror("posix_memalign(data)");
			free(control);
			break;
		}

		well = (diminuto_well_t *)control;
		diminuto_list_datainit(&well[DIMINUTO_WELL_FREE], control);
		diminuto_list_datainit(&well[DIMINUTO_WELL_USED], data);
		for (ii = 0; ii < count; ++ii) {
			diminuto_list_insert(&well[DIMINUTO_WELL_FREE], diminuto_list_datainit(&well[DIMINUTO_WELL_NODE + ii], data));
			data += size;
		}

		wellp = &well[0];

	} while (0);

	return wellp;
}

void diminuto_well_fini(diminuto_well_t * wellp)
{
	free(diminuto_list_data(&wellp[DIMINUTO_WELL_USED])); /* data */
	free(diminuto_list_data(&wellp[DIMINUTO_WELL_FREE])); /* control */
}

void * diminuto_well_alloc(diminuto_well_t * wellp)
{
    void * pointer = (void *)0;
    diminuto_list_t * nodep;

    wellp = diminuto_list_root(wellp);
    if (!diminuto_list_isempty(&wellp[DIMINUTO_WELL_FREE])) {
        nodep = diminuto_list_remove(diminuto_list_next(&wellp[DIMINUTO_WELL_FREE]));
        if (nodep != (diminuto_well_t *)0) {
        	pointer = diminuto_list_data(nodep);
        	diminuto_list_insert(diminuto_list_prev(&wellp[DIMINUTO_WELL_USED]), nodep);
        } else {
        	errno = EFAULT;
        }
    } else {
    	errno = ENOMEM;
    }

    return pointer;
}

int diminuto_well_free(diminuto_well_t * wellp, void * pointer)
{
	int rc = -1;
    diminuto_list_t * nodep;

    wellp = diminuto_list_root(wellp);
    if (!diminuto_list_isempty(&wellp[DIMINUTO_WELL_USED])) {
    	nodep = diminuto_list_remove(diminuto_list_next(&wellp[DIMINUTO_WELL_USED]));
    	if (nodep != (diminuto_well_t *)0) {
    		diminuto_list_insert(diminuto_list_prev(&wellp[DIMINUTO_WELL_FREE]), diminuto_list_datainit(nodep, pointer));
    		rc = 0;
    	} else {
    		errno = EFAULT;
    	}
    } else {
    	errno = ENOMEM;
    }

    return rc;
}
