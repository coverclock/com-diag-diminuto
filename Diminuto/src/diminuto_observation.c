/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_observation.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

static const char SUFFIX[] = "XXXXXX";

int diminuto_observation_create(const char * path, char ** tempp)
{
    int fd = -1;
    size_t length = 0;
    char * temp = (char *)0;
    int rc = -1;

    do {

        if (sizeof(SUFFIX) > PATH_MAX) {
            errno = E2BIG;
            fd = -2;
            diminuto_perror("diminuto_observation_create: sizeof");
            break;
        }

        length = strlen(path);
        if (length > (PATH_MAX - sizeof(SUFFIX) - 1)) {
            errno = ENAMETOOLONG;
            fd = -3;
            diminuto_perror("diminuto_observation_create: strlen");
            break;
        }

        temp = (char *)malloc(length + sizeof(SUFFIX));
        if (temp == (char *)0) {
            errno = EFAULT;
            fd = -4;
            diminuto_perror("diminuto_observation_create: malloc");
            break;
        }

        strcpy(temp, path);
        strcat(temp, SUFFIX);

        fd = mkstemp(temp);
        if (fd < 0) {
            fd = -5;
            diminuto_perror("diminuto_observation_create: mkstemp");
            free(temp);
            break;
        }

        *tempp = temp;

    } while (0);

    return fd;
}

int diminuto_observation_commit(int fd, char * temp)
{
    int rc = -1;
    size_t length = 0;
    char * path = (char *)0;

    do {

        length = strlen(temp);
        if (!((sizeof(SUFFIX) < length) && (length < PATH_MAX))) {
            errno = ENAMETOOLONG;
            diminuto_perror("diminuto_observation_commit: strlen");
            break;
        }

        path = strdup(temp);
        if (path == (char *)0) {
            errno = EFAULT;
            diminuto_perror("diminuto_observation_commit: strdup");
            break;
        }

        path[length - sizeof(SUFFIX) + 1] = '\0';

        rc = close(fd);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_commit: close");
            free(path);
            break;
        }

        rc = rename(temp, path);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_commit: rename");
            free(path);
            break;
        }

        free(path);
        free(temp);

        fd = -1;

    } while (0);

    return fd;
}

int diminuto_observation_discard(int fd, char * temp)
{
    int rc = -1;
    size_t length = 0;
    char * path = (char *)0;

    do {

        rc = close(fd);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_discard: close");
            break;
        }

        rc = unlink(temp);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_discard: unlink");
            break;
        }

        free(temp);

        fd = -1;

    } while (0);

    return fd;
}
