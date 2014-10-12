/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>
#include <sys/stat.h>
#if !defined(__USE_GNU)
#       define __USE_GNU
#       define UNDEF__USE_GNU
#endif
#include <fcntl.h>
#if defined(UNDEF__USE_GNU)
#       undef __USE_GNU
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
    int xc = 1;
    ssize_t pagesize = 0;
    void * buffer = 0;
    int fd = -1;
    ssize_t rc = 0;
    size_t size = 0;
    char * pointer = 0;
    uint64_t datum = 0;
    uint64_t * data = 0;
    uint64_t total = 0;
    uint64_t errors = 0;
    long base = 0;
    time_t start = 0;
    time_t end = 0;
    int error = 0;

    do {

        if (argc <= 1) {
            fprintf(stderr, "usage: %s DEVICE\n", argv[0]);
            break;
        }

        base = random();

        pagesize = getpagesize();
        if (pagesize <= 0) {
            perror("getpagesize");
            pagesize = 4096;
        }

        fprintf(stderr, "%s: %d bytes pagesize\n", argv[0], pagesize);

        buffer = memalign(pagesize, pagesize);
        if (buffer == 0) {
            perror("memalign");
            break;
        }

        fd = open(argv[1], O_WRONLY | O_DIRECT | O_SYNC, 0);
        if (fd < 0) {
            perror(argv[1]);
            break;
        }

        fprintf(stderr, "%s: writing\n", argv[0]);

        datum = base;
        total = 0;
        start = time(0);

        do {

            data = (uint64_t *)buffer;
            size = pagesize / sizeof(datum);
            while ((size--) > 0) {
                *(data++) = datum;
                datum = datum * 4294967311ULL + 17;
            }

            pointer = (char *)buffer;
            size = pagesize;
            while (size > 0) {
                rc = write(fd, pointer, size);
                if (rc < 0) {
                    perror("write");
                    break;
                } else if (rc == 0) {
                    fprintf(stderr, "write: eof\n");
                    break;
                } else {
                    total += rc; 
                    pointer += rc;
                    size -= rc;
                }
            }

        } while (rc > 0);

        end = time(0);

    	fprintf(stderr, "%s: %llu bytes written\n", argv[0], total);
    	fprintf(stderr, "%s: %lu seconds elapsed\n", argv[0], end - start);

        fd = close(fd);
        if (fd < 0) {
            perror("close");
            break;
        }

        fd = open(argv[1], O_RDONLY | O_DIRECT, 0);
        if (fd < 0) {
            perror(argv[1]);
            break;
        }

        fprintf(stderr, "%s: reading\n", argv[0]);

        datum = base;
        total = 0;
        start = time(0);

        do {

            pointer = (char *)buffer;
            size = pagesize;
            while (size > 0) {
                rc = read(fd, pointer, size);
                if (rc < 0) {
                    perror("read");
                    break;
                } else if (rc == 0) {
                    fprintf(stderr, "read: eof\n");
                    break;
                } else {
                    total += rc; 
                    pointer += rc;
                    size -= rc;
                }
            }

            if (rc <= 0) {
                break;
            }

            error = 0;
            data = (uint64_t *)buffer;
            size = pagesize / sizeof(datum);
            while ((size--) > 0) {
                if (*(data++) != datum) {
                    ++errors;
                    error = !0;
                }
                datum = datum * 4294967311ULL + 17;
            }
            if (error) {
                end = time(0);
    	        fprintf(stderr, "%s: %llu errors %llu bytes %lu seconds\n", argv[0], errors, total, end - start);
            }

        } while (rc > 0);

        end = time(0);

    	fprintf(stderr, "%s: %llu bytes read\n", argv[0], total);
    	fprintf(stderr, "%s: %lu seconds elapsed\n", argv[0], end - start);
    	fprintf(stderr, "%s: %llu errors\n", argv[0], errors);

        fd = close(fd);
        if (fd < 0) {
            perror("close");
            break;
        }

        fd = -1;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if ((fd = close(fd))) {
        perror("close");
    } else {
        /* Do nothing. */
    }

    return xc;
}
