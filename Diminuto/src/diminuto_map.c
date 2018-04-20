/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_map.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

void * diminuto_map_map(uintptr_t start, size_t length, void ** startp, size_t * lengthp)
{
    void * result = (void *)0;
    int fd = -1;
    void * base = (void *)0;
    size_t size = 0;
    int pagesize;
    size_t offset;
    off_t address;

    do {

        *startp = (void *)0;
        *lengthp = 0;

        pagesize = getpagesize();
        offset = start % pagesize;
        address = start - offset;
        size = length + offset;

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
            diminuto_perror("diminuto_map_map: open");
            break;
        }

        if ((base = mmap(0, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, address)) == (void *)-1) {
            diminuto_perror("diminuto_map_map: mmap");
            break;
        }

        *startp = base;
        *lengthp = size;

        result = (unsigned char *)base + offset;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if ((fd = close(fd)) < 0) {
        diminuto_perror("diminuto_map_map: close");
        /* Proceed anyway. */
    }

    return result;
}

int diminuto_map_unmap(void ** startp, size_t * lengthp)
{
    int rc = 0;

    if (*lengthp == 0) {
        /* Do nothing. */
    } else if ((rc = munmap(*startp, *lengthp)) < 0) {
        diminuto_perror("diminuto_map_unmap: munmap");
    } else {
        *startp = (void *)0;
        *lengthp = 0;
    }

    return rc;
}

int diminuto_map_minimum(uintptr_t minimum)
{
    int fd = -1;
    FILE * fp = (FILE *)0;
    int result = 0;
    unsigned long value;

    do {

        if ((getuid() != 0) && (geteuid() != 0)) {
            result = -1;
            errno = EPERM;
            diminuto_perror("diminuto_map_minimum: open");
            break;
        }

        if ((fd = open("/proc/sys/vm/mmap_min_addr", O_WRONLY)) < 0) {
            break;
        }

        if ((fp = fdopen(fd, "w")) == (FILE *)0) {
            result = -2;
            diminuto_perror("diminuto_map_minimum: fdopen");
            break;
        }

        value = minimum;

        if (fprintf(fp, "%lu", value) < 0) {
            result = -3;
            diminuto_perror("diminuto_map_minimum: fprintf");
            break;
        }

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        result = -4;
        diminuto_perror("diminuto_map_minimum: fclose");
    } else {
        fp = (FILE *)0;
        fd = -1;
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        result = -5;
        diminuto_perror("diminuto_map_minimum: close");
    } else {
        fd = -1;
    }

    return result;
}
