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

static const char SUFFIX[] = "XXXXXX";

FILE * diminuto_observation_create(const char * path, char ** tempp)
{
    FILE * fp = (FILE *)0;
    size_t length = 0;
    char * temp = (char *)0;
    int fd = -1;

    do {

        if (sizeof(SUFFIX) > PATH_MAX) {
            errno = E2BIG;
            diminuto_perror("diminuto_observation_create: sizeof");
            break;
        }

        length = strlen(path);
        if (length > (PATH_MAX - sizeof(SUFFIX) - 1)) {
            errno = ENAMETOOLONG;
            diminuto_perror("diminuto_observation_create: strlen");
            break;
        }

        temp = (char *)malloc(length + sizeof(SUFFIX));
        if (temp == (char *)0) {
            errno = EFAULT;
            diminuto_perror("diminuto_observation_create: malloc");
            break;
        }

        strcpy(temp, path);
        strcat(temp, SUFFIX);

        fd = mkstemp(temp);
        if (fd < 0) {
            diminuto_perror("diminuto_observation_create: mkstemp");
            free(temp);
            break;
        }

        fp = fdopen(fd, "r+");
        if (fp == (FILE *)0) {
            diminuto_perror("diminuto_observation_create: fdopen");
            free(temp);
        	break;
        }

        *tempp = temp;

    } while (0);

    return fp;
}

FILE * diminuto_observation_commit(FILE * fp, char ** tempp)
{
    int rc = -1;
    size_t length = 0;
    char * path = (char *)0;

    do {

        length = strlen(*tempp);
        if (!((sizeof(SUFFIX) < length) && (length < PATH_MAX))) {
            errno = ENAMETOOLONG;
            diminuto_perror("diminuto_observation_commit: strlen");
            break;
        }

        path = strdup(*tempp);
        if (path == (char *)0) {
            errno = EFAULT;
            diminuto_perror("diminuto_observation_commit: strdup");
            break;
        }

        path[length - sizeof(SUFFIX) + 1] = '\0';

        rc = fclose(fp);
        if (rc != 0) {
            diminuto_perror("diminuto_observation_commit: fclose");
            free(path);
            break;
        }

        /*
         * If fclose(3) succeeds but rename(2) does not, we still return the
         * original FILE pointer to indicate failure, even though it is no
         * longer useful. We must do the fclose(3) prior to the rename(2) to
         * insure the renamed file is immediately accessible.
         */

        rc = rename(*tempp, path);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_commit: rename");
            free(path);
            break;
        }

        free(path);
        free(*tempp);

        *tempp = (char *)0;
        fp = (FILE *)0;

    } while (0);

    return fp;
}

FILE * diminuto_observation_discard(FILE * fp, char ** tempp)
{
    int rc = -1;

    do {

        rc = fclose(fp);
        if (rc != 0) {
            diminuto_perror("diminuto_observation_discard: fclose");
            break;
        }

        /*
         * If fclose(3) succeeds but unlink(2) does not, we still return the
         * original FILE pointer to indicate failure, even though it is no
         * longer useful. We must do the fclose(3) prior to the rename(2) to
         * insure the renamed file is immediately inaccessible.
         */

        rc = unlink(*tempp);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_discard: unlink");
            break;
        }

        free(*tempp);

        *tempp = (char *)0;
        fp = (FILE *)0;

    } while (0);

    return fp;
}
