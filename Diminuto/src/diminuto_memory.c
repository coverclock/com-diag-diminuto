/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Memory feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Memory feature.
 */

#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

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
            method = DIMINUTO_MEMORY_PAGESIZE_METHOD_EXPLICIT;
            break;
        }

#endif

#if defined(_SC_PAGESIZE)

        if ((pagesize = sysconf(_SC_PAGESIZE)) > 0) {
            method = DIMINUTO_MEMORY_PAGESIZE_METHOD_SYSCONF_PAGESIZE;
            break;
        }

#endif

        if ((pagesize = getpagesize()) > 0) {
            method = DIMINUTO_MEMORY_PAGESIZE_METHOD_GETPAGESIZE;
            break;
        }

        pagesize = DIMINUTO_MEMORY_PAGESIZE_BYTES;
        method = DIMINUTO_MEMORY_PAGESIZE_METHOD_IMPLICIT;

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
            method = DIMINUTO_MEMORY_LINESIZE_METHOD_EXPLICIT;
            break;
        }

#endif

#if defined(_SC_LEVEL1_DCACHE_LINESIZE)

        if ((linesize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE)) > 0) {
            method = DIMINUTO_MEMORY_LINESIZE_METHOD_SYSCONF_LEVEL1_DCACHE_LINESIZE;
            break;
        }

#endif

        for (ii = 0; ii < countof(SYS_LINE_SIZE); ++ii) {

            if ((fp = fopen(SYS_LINE_SIZE[ii], "r")) == (FILE *)0) {
                continue;
            }

            fscanf(fp, "%zd", &linesize);

            fclose(fp);

            if (linesize > 0) {
                method = DIMINUTO_MEMORY_LINESIZE_METHOD_SYS_INDEX0_COHERENCY_LINE_SIZE + ii;
                break;
            }

        }

        if (linesize > 0) {
            break;
        }

        linesize = DIMINUTO_MEMORY_LINESIZE_BYTES;
        method = DIMINUTO_MEMORY_LINESIZE_METHOD_IMPLICIT;

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

void * diminuto_memory_aligned(size_t alignment, size_t size)
{
    void * pointer = (void *)0;
    int rc;

    /*
     * POSIX requires that size be a multiple of alignment, and that alignment
     * be a power of two and a multiple of sizeof(void *). We ensure this.
     */

    alignment = diminuto_memory_alignment(alignment, sizeof(void *));
    size = diminuto_memory_alignment(size, alignment);

    rc = posix_memalign(&pointer, alignment, size);
    if (rc > 0) {
        pointer = (void *)0;
        errno = rc;
    } else if (rc < 0) {
        pointer = (void *)0;
        errno = -rc;
    } else if (pointer == (void *)0) {
        errno = EFAULT;
    } else {
        /* Do nothing. */
    }

    if (pointer == (void *)0) {
        diminuto_perror("diminuto_memory_aligned: posix_memalign");
    }

    return pointer;
}
