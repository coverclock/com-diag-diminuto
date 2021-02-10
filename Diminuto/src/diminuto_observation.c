/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2019-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Observation feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Observation feature.
 */

#include "com/diag/diminuto/diminuto_observation.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char SUFFIX[] = "-" "XXXXXX";

static const char TIMESTAMP[] = "-" "YYYYMMDD" "T" "HHMMSS" "Z" "UUUUUU";

FILE * diminuto_observation_create_generic(const char * path, char ** tempp, mode_t mode)
{
    FILE * fp = (FILE *)0;
    size_t length = 0;
    char * temp = (char *)0;
    int fd = -1;
    int rc = -1;

    do {

        if (sizeof(SUFFIX) > sizeof(diminuto_path_t)) {
            errno = E2BIG;
            diminuto_perror("diminuto_observation_create_generic: sizeof");
            break;
        }

        length = strlen(path);
        if (length > (sizeof(diminuto_path_t) - sizeof(SUFFIX) - 1)) {
            errno = ENAMETOOLONG;
            diminuto_perror("diminuto_observation_create_generic: strlen");
            break;
        }

        temp = (char *)malloc(length + sizeof(SUFFIX));
        if (temp == (char *)0) {
            diminuto_perror("diminuto_observation_create_generic: malloc");
            break;
        }

        strcpy(temp, path);
        strcat(temp, SUFFIX);

        fd = mkstemp(temp);
        if (fd < 0) {
            diminuto_perror("diminuto_observation_create_generic: mkstemp");
            break;
        }

        /*
         * According to POSIX, the default file creation mask used
         * by mkstemp(3) is 0600.
         */

        rc = fchmod(fd, mode);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_create_generic: fchmod");
            break;
        }

        fp = fdopen(fd, "r+");
        if (fp == (FILE *)0) {
            diminuto_perror("diminuto_observation_create_generic: fdopen");
            break;
        }

        *tempp = temp;

    } while (0);

    if (temp == (char *)0) {
        /* Do nothing. */
    } if (*tempp == temp) {
        /* Do nothing. */
    } else {
        free(temp);
    }

    if (fp != (FILE *)0) {
        /* Do nothing. */
    } else if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) >= 0) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_observation_create_generic: close");
    }

    return fp;
}

FILE * diminuto_observation_commit(FILE * fp, char ** tempp)
{
    int rc = -1;
    size_t length = 0;
    char * path = (char *)0;

    do {

        length = strlen(*tempp);
        if (!((sizeof(SUFFIX) < length) && (length < sizeof(diminuto_path_t)))) {
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
         * If fclose(3) succeeds but renameat(2) does not, we still return the
         * original FILE pointer to indicate failure, even though it is no
         * longer useful. We must do the fclose(3) prior to the renameat(2) to
         * insure the renamed file is immediately accessible.
         */

        rc = renameat(AT_FDCWD, *tempp, AT_FDCWD, path);
        if (rc < 0) {
            diminuto_perror("diminuto_observation_commit: renameat");
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
        if (!((sizeof(SUFFIX) < length) && (length < sizeof(diminuto_path_t)))) {
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

        snprintf(&(path[length - sizeof(SUFFIX) + 1]), sizeof(TIMESTAMP), "-%04d%02d%02dT%02d%02d%02dZ%06lld", year, month, day, hour, minute, second, (long long signed int)ticks);

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
