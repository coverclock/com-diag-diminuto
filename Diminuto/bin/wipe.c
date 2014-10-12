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
#include <errno.h>
#include <errno.h>

int main(int argc, void ** argv)
{
    ssize_t alignment = 0;
    void * buffer = 0;
    int fd = -1;
    ssize_t rc = 0;
    uint64_t sn = 0;

    do {

        if (argc <= 1) {
            perror("usage");
            break;
        }

        alignment = getpagesize();
        if (alignment <= 0) {
            perror("getpagesize");
            break;
        }

        buffer = memalign(alignment, alignment);
        if (buffer == 0) {
            perror("memalign");
            break;
        }

        fd = open(argv[1], O_DIRECT | O_SYNC, 0);
        if (fd < 0) {
            perror(argv[1]);
            break;
        }

    } while (0);

    if (fd >= 0) {
        close(fd);
    }

    return 0;
}
