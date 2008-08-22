/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_map.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

void * diminuto_map(uintptr_t start, size_t length, void ** startp, size_t * lengthp)
{
    void * result = NULL;
    int fd = -1;
    void * base = NULL;
    size_t size = 0;
    int pagesize;
    size_t modulo;
    off_t offset;

    do {

        pagesize = getpagesize();
        modulo = start % pagesize;
        offset = start - modulo;
        size = length + modulo;

        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd < 0) {
            perror("diminuto_map: open");
            break;
        }

        base = mmap(0, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, offset);
        if (base == (void *)-1) {
            perror("diminuto_map: mmap");
            break;
        }

        result = (unsigned char *)base + modulo;

        *startp = base;
        *lengthp = size;

    } while (0);

    if (fd >= 0) {
        fd = close(fd);
        if (fd < 0) {
            perror("diminuto_map: close");
        }
    }

    return result;
}

int diminuto_unmap(void * start, size_t length)
{
    int rc;

    rc = munmap(start, length);
    if (rc < 0) {
        perror("diminuto_unmap: munmap");
    }

    return rc;
}
