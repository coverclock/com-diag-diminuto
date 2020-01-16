/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2019-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_observation.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <sys/param.h>
#include <errno.h>
#include <stdlib.h>

static const char SUFFIX[] = "-" "XXXXXX";

static const char TIMESTAMP[] = "-" "YYYYMMDD" "T" "HHMMSS" "Z" "UUUUUU";

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
            diminuto_perror("diminuto_observation_commit: strdup");
            break;
        }

        path[length - sizeof(SUFFIX) + 1] = '\0';

        rc = fclose(fp);
        if (rc != 0) {
            diminuto_perror("diminuto_observation_commit: fclose");
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
            break;
        }

        free(*tempp);

        *tempp = (char *)0;
        fp = (FILE *)0;

    } while (0);

    if (path != (char *)0) {
        free(path);
    }

    return fp;
}

FILE * diminuto_observation_checkpoint(FILE * fp, char ** tempp)
{
    FILE * result = (FILE *)0;
    int rc = -1;
    size_t length = 0;
    char * path = (char *)0;
    diminuto_sticks_t now = 0;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    diminuto_ticks_t ticks = 0;

    do {

        length = strlen(*tempp);
        if (!((sizeof(SUFFIX) < length) && (length < PATH_MAX))) {
            errno = ENAMETOOLONG;
            diminuto_perror("diminuto_observation_checkpoint: strlen");
            break;
        }

        path = malloc(length - sizeof(SUFFIX) + sizeof(TIMESTAMP) + 1);
        if (path == (char *)0) {
            diminuto_perror("diminuto_observation_checkpoint: malloc");
            break;
        }

        strncpy(path, *tempp, length - sizeof(SUFFIX) + 1);

        now = diminuto_time_clock();
        if (now < 0) {
            break;
        }

        rc = diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &ticks);
        if (rc < 0) {
            break;
        }

        ticks = diminuto_frequency_ticks2units(ticks, 1000000);

        snprintf(&(path[length - sizeof(SUFFIX) + 1]), sizeof(TIMESTAMP), "-%04d%02d%02dT%02d%02d%02dZ%06ld", year, month, day, hour, minute, second, ticks);

        rc = fflush(fp);
        if (rc != 0) {
            diminuto_perror("diminuto_observation_checkpoint: fflush");
            /* Fall through. */
        }

        rc = link(*tempp, path);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_checkpoint: link");
            break;
        }

        result = fp;

    } while (0);

    if (path != (char *)0) {
        free(path);
    }

    return result;
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
