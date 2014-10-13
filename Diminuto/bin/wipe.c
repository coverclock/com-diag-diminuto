/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms of the GPL v2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>
#include <sys/stat.h>
#undef COM_DIAG_DIMINUTO_UNDEFINE___USE_GNU
#if !defined(__USE_GNU)
#       define __USE_GNU
#       define COM_DIAG_DIMINUTO_UNDEFINE___USE_GNU
#endif
#include <fcntl.h>
#if defined(COM_DIAG_DIMINUTO_UNDEFINE___USE_GNU)
#       undef __USE_GNU
#        undef COM_DIAG_DIMINUTO_UNDEFINE___USE_GNU
#endif
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    ssize_t pagesize = 0;
    void * buffer = 0;
    int fd = -1;
    ssize_t rc = 0;
    size_t size = 0;
    size_t count = 0;
    char * pointer = 0;
    uint64_t datum = 0;
    uint64_t * data = 0;
    uint64_t total = 0;
    uint64_t written = 0;
    uint64_t mismatches = 0;
    uint64_t errors = 0;
    long seed = 0;
    time_t start = 0;
    time_t end = 0;
    static const uint64_t MODULO = 100000000ULL;
    uint64_t modulo = 0;

    do {

        if (argc <= 1) {
            fprintf(stderr, "usage: %s DEVICE\n", argv[0]);
            ++errors;
            break;
        }

        printf("%s: device \"%s\"\n", argv[0], argv[1]);

        seed = time(0);
        if (seed < 0) {
            perror("time");
            ++errors;
        }
        srandom(seed);
        seed = random();

        printf("%s: seed %d\n", argv[0], seed);

        pagesize = getpagesize();
        if (pagesize <= 0) {
            perror("getpagesize: ERROR");
            pagesize = 4096;
            ++errors;
        }

        printf("%s: pagesize %d bytes\n", argv[0], pagesize);

        modulo = ((MODULO + pagesize - 1) / pagesize) * pagesize;

        printf("%s: modulo %d bytes\n", argv[0], modulo);

        buffer = memalign(pagesize, pagesize);
        if (buffer == 0) {
            perror("memalign: ERROR");
            ++errors;
            break;
        }

        fd = open(argv[1], O_WRONLY | O_DIRECT | O_SYNC, 0);
        if (fd < 0) {
            perror("open: ERROR");
            ++errors;
            break;
        }

        printf("%s: writing\n", argv[0]);

        datum = seed;
        total = 0;
        start = time(0);
        if (start < 0) {
            perror("time: ERROR");
            ++errors;
        }

        do {

            data = (uint64_t *)buffer;
            count = pagesize / sizeof(datum);
            while ((count--) > 0) {
                *(data++) = datum;
                datum = datum * 4294967311ULL + 17;
            }

            pointer = (char *)buffer;
            size = pagesize;
            while (size > 0) {
                rc = write(fd, pointer, size);
                if (rc > 0) {
                    total += rc;
                    if ((total % modulo) == 0ULL) {
                    	fprintf(stderr, "write: progress %llu bytes\n", total);
                    }
                    pointer += rc;
                    size -= rc;
                } else if (rc == 0) {
                    fprintf(stderr, "write: EOF\n");
                    break;
                } else if (errno == ENOSPC) {
                    perror("write");
                    break;
                } else {
                    perror("write: ERROR");
                    ++errors;
                    break;
                }
            }

        } while (rc > 0);

        end = time(0);
        if (end < 0) {
            perror("time: ERROR");
            ++errors;
        }

        printf("%s: written %llu bytes\n", argv[0], total);
        printf("%s: elapsed %d seconds\n", argv[0], end - start);

        fd = close(fd);
        if (fd < 0) {
            perror("close: ERROR");
            ++errors;
        }

        fd = open(argv[1], O_RDONLY | O_DIRECT, 0);
        if (fd < 0) {
            perror("open: ERROR");
            ++errors;
            break;
        }

        printf("%s: reading\n", argv[0]);

        datum = seed;
        written = total;
        total = 0;
        start = time(0);
        if (start < 0) {
            perror("time: ERROR");
            ++errors;
        }

        do {

            pointer = (char *)buffer;
            size = pagesize;
            while (size > 0) {
                rc = read(fd, pointer, size);
                if (rc > 0) {
                    total += rc;
                    if ((total % modulo) == 0ULL) {
                    	fprintf(stderr, "read: progress %llu bytes\n", total);
                    }
                    pointer += rc;
                    size -= rc;
                } else if (rc == 0) {
                    fprintf(stderr, "read: EOF\n");
                    break;
                } else {
                    perror("read: ERROR");
                    ++errors;
                    break;
                }
            }

            if (size == pagesize) {
                break;
            }

            if (size > 0) {
                fprintf(stderr, "read: short %llu bytes\n", size);
            }

            data = (uint64_t *)buffer;
            count = (pagesize - size) / sizeof(datum);
            while ((count--) > 0) {
                if (*(data++) != datum) {
                    fprintf(stderr, "read: ERROR: mismatch offset %llu bytes\n", total);
                    ++mismatches;
                    ++errors;
                }
                datum = (datum * 4294967311ULL) + 17;
            }

        } while (rc > 0);

        end = time(0);
        if (end < 0) {
            perror("time: ERROR");
            ++errors;
        }

        if (total != written) {
            fprintf(stderr, "read: ERROR: incomplete");
            ++errors;
        }

        printf("%s: read %llu bytes\n", argv[0], total);
        printf("%s: elapsed %d seconds\n", argv[0], end - start);
        printf("%s: mismatches %llu\n", argv[0], mismatches);

        fd = close(fd);
        if (fd < 0) {
            perror("close: ERROR");
        }

        fd = -1;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if ((fd = close(fd))) {
        perror("close: ERROR");
    } else {
        /* Do nothing. */
    }

    printf("%s: errors %llu\n", argv[0], errors);

    return (errors > 0) ? 1 : 0;
}
