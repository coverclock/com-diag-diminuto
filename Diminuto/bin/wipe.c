/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms of the GPL v2 or later (at your option).<BR>
 * Author: Chip Overclock.<BR>
 * mailto:coverclock@diag.com<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * No warranty is expressed or implied.<BR>
 * Digital Aggregates Corporation is a registered trademark.<BR>
 * Chip Overclock is a registered trademark.<BR>
 *
 * Usage: wipe [ -D DEVICE ] [ -b BLOCKSIZE ] [ -m MODULO ] [ -p PAGESIZE ]
 *
 * Example: wipe -D /dev/sdj1
 *
 * This program wipes storage devices by writing a pattern across the device
 * until it gets an error (typically ENOSPC or "No space left on device"). Then
 * it reads the device from the beginning and verifies that the pattern read
 * matches the pattern written. By default, writes are done in a multiple of
 * the system page size that is at least 4096 bytes. I wrote it to address
 * data remanance issues on flash devices. It can wipe any storage (including
 * your root partition if you're not careful). It can also be used to test flash
 * devices for both performance and capacity, although it is destructive to
 * any data stored on the device. It bypasses the Linux block cache by using
 * direct synchronous I/O; that means it takes a long time to run, but when it
 * finishes successfully you are pretty confident it has actually wiped the
 * device. The pattern it writes is generated by a polynomial initialized by a
 * random number generator seeded by the system time, so it's difficult to
 * predict. The polynomial was adapted from the "f3" flash testing program by
 * Michel Machado (http://oss.digirati.com.br/f3/).
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
#       undef COM_DIAG_DIMINUTO_UNDEFINE___USE_GNU
#endif
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>

static const char DEVICE[] = "/dev/null";
static const size_t BLOCKSIZE = 4096UL;
static const uint64_t MODULO = 100000000ULL;
static const size_t PAGESIZE = 4096UL;

int main(int argc, char ** argv)
{
    const char * program = 0;
    const char * device = 0;
    size_t blocksize = 0;
    uint64_t modulo = 0;
    size_t pagesize = 0;
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
    int opt;
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    device = DEVICE;
    blocksize = BLOCKSIZE;
    modulo = MODULO;
    pagesize = PAGESIZE;

    do {

        while ((opt = getopt(argc, argv, "D:b:m:p:")) >= 0) {

            switch (opt) {

            case 'D':
                device = optarg;
                break;

            case 'b':
                blocksize = strtoul(optarg, 0, 0);
                break;

            case 'm':
                modulo = strtoull(optarg, 0, 0);
                break;

            case 'p':
                pagesize = strtoul(optarg, 0, 0);
                break;

            default:
                fprintf(stderr, "usage: %s [ -D DEVICE ] [ -b BLOCKSIZE ] [ -m MODULO ] [ -p PAGESIZE ]\n", program);
                ++errors;
                break;

            }

        }

        if (errors > 0) {
            break;
        }

        printf("%s: device \"%s\"\n", program, device);

        start = time(0);
        if (start == (time_t)-1) {
            perror("time");
            ++errors;
        }
        srandom(start);
        seed = random();
        printf("%s: seed %ld\n", program, seed);

        rc = getpagesize();
        if (rc < 0) {
            perror("getpagesize: ERROR");
            ++errors;
        } else if (rc == 0) {
            /* Do nothing. */
        } else {
            pagesize = rc;
        }
        printf("%s: pagesize %zu bytes\n", program, pagesize);

        blocksize = ((blocksize + pagesize - 1) / pagesize) * pagesize;
        printf("%s: blocksize %zu bytes\n", program, blocksize);

        modulo = ((modulo + blocksize - 1) / blocksize) * blocksize;
        printf("%s: modulo %llu bytes\n", program, modulo);

        buffer = memalign(pagesize, blocksize);
        if (buffer == 0) {
            perror("memalign: ERROR");
            ++errors;
            break;
        }

        fd = open(device, O_WRONLY | O_DIRECT | O_SYNC, 0);
        if (fd < 0) {
            perror("open: ERROR");
            ++errors;
            break;
        }

        printf("%s: writing\n", program);

        datum = seed;
        total = 0;
        start = time(0);
        if (start == (time_t)-1) {
            perror("time: ERROR");
            ++errors;
        }

        do {

            data = (uint64_t *)buffer;
            count = blocksize / sizeof(datum);
            while ((count--) > 0) {
                *(data++) = datum;
                datum = (datum * 4294967311ULL) + 17;
            }

            pointer = (char *)buffer;
            size = blocksize;
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
        if (end == (time_t)-1) {
            perror("time: ERROR");
            ++errors;
        }

        printf("%s: written %llu bytes\n", program, total);
        printf("%s: elapsed %lu seconds\n", program, end - start);

        fd = close(fd);
        if (fd < 0) {
            perror("close: ERROR");
            ++errors;
        }

        fd = open(device, O_RDONLY | O_DIRECT, 0);
        if (fd < 0) {
            perror("open: ERROR");
            ++errors;
            break;
        }

        printf("%s: reading\n", program);

        datum = seed;
        written = total;
        total = 0;
        start = time(0);
        if (start == (time_t)-1) {
            perror("time: ERROR");
            ++errors;
        }

        do {

            pointer = (char *)buffer;
            size = blocksize;
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

            if (size == blocksize) {
                break;
            }

            if (size > 0) {
                fprintf(stderr, "read: short %zu bytes\n", size);
            }

            data = (uint64_t *)buffer;
            count = (blocksize - size) / sizeof(datum);
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
        if (end == (time_t)-1) {
            perror("time: ERROR");
            ++errors;
        }

        if (total != written) {
            fprintf(stderr, "read: ERROR: incomplete");
            ++errors;
        }

        printf("%s: read %llu bytes\n", program, total);
        printf("%s: elapsed %lu seconds\n", program, end - start);
        printf("%s: mismatches %llu\n", program, mismatches);

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

    printf("%s: errors %llu\n", program, errors);

    return (errors > 0) ? 1 : 0;
}
