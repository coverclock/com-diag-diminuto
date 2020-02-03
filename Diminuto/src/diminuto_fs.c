/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

static int Debug = 0;

int diminuto_fs_debug(int now)
{
    int was = 0;

    was = Debug;
    Debug = now;

    return was;
}

diminuto_fs_type_t diminuto_fs_type(mode_t mode)
{
    diminuto_fs_type_t type = DIMINUTO_FS_TYPE_NONE;

    if (S_ISREG(mode)) {
        type = DIMINUTO_FS_TYPE_FILE;
    } else if (S_ISDIR(mode)) {
        type = DIMINUTO_FS_TYPE_DIRECTORY;
    } else if (S_ISLNK(mode)) {
        type = DIMINUTO_FS_TYPE_SYMLINK;
    } else if (S_ISCHR(mode)) {
        type = DIMINUTO_FS_TYPE_CHARACTERDEV;
    } else if (S_ISBLK(mode)) {
        type = DIMINUTO_FS_TYPE_BLOCKDEV;
    } else if (S_ISFIFO(mode)) {
        type = DIMINUTO_FS_TYPE_FIFO;
    } else if (S_ISSOCK(mode)) {
        type = DIMINUTO_FS_TYPE_SOCKET;
    } else {
        mode = (mode & S_IFMT) >> 12 /* Fragile assumed constant! */;
        if ((0 <= mode) && (mode <= 9)) {
            type = (diminuto_fs_type_t)('0' + mode);
        }  else if ((0xa <= mode) && (mode <= 0xf)) {
            type = (diminuto_fs_type_t)('A' + mode - 0xa);
        } else {
            type = DIMINUTO_FS_TYPE_UNKNOWN; /* Not unless S_IFMT changes. */
        }
    }

    return type;
}

int diminuto_fs_walker(const char * name, char * path, size_t total, size_t depth, diminuto_fs_walker_t * walkerp, void * statep)
{
    DIR * dp = (DIR *)0;
    struct dirent * ep = (struct dirent *)0;
    struct stat status = { 0 };
    int rc = 0;
    size_t dd = 0;
    size_t prior = 0;
    size_t length = 0;

    /*
     * Insure the path buffer has sufficient room. I'd be surprised
     * if this failed on a modern system, but back when MAXPATHLEN
     * was 512 I have seen file systems for which an absolute path
     * string could not be represented.
     */

    length = strnlen(name, PATH_MAX);
    if (length == 0) {
        errno = EINVAL;
        diminuto_perror(name);
        return -3;
    } else if ((total + 1 /* '/' */ + length + 1 /* '\0' */) > PATH_MAX) {
        errno = E2BIG;
        diminuto_perror(path);
        return -4;
    } else {
        /* Do nothing. */
    }

    if (Debug) { fprintf(stderr, "%s@%d: \"%s\" [%zu] \"%s\" [%zu]\n", __FILE__, __LINE__, path, total, name, length); }

    /*
     * Contstruct a path (maybe be relative or absolute depending
     * on the root).
     */

    prior = total;
    if (total == 0) {
        /* Do nothing. */
    } else if (path[total - 1] == '/') {
        /* Do nothing. */
    } else {
        path[total++] = '/';
        path[total] = '\0';
    }
    strcat(path, name);
    total += length;

    if (Debug) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, total); }

    /*
     * Get the attributes for the file identified by the path.
     */

    rc = lstat(path, &status);
    if (rc >= 0) {
        /* Do nothing. */
    } else if ((errno == EACCES) || (errno == ENOENT)) {
        diminuto_perror(path);
        path[prior] = '\0';
        return 0;
    } else {
        diminuto_perror(path);
        return -5;
    }

    /*
     * Invoke the callback.
     */

    if (walkerp == (diminuto_fs_walker_t *)0) {
        /* Do nothing. */
    } else if ((rc = (*walkerp)(statep, name, path, depth, &status)) == 0) {
        /* Do nothing. */
    } else {
        return rc;
    }

    /*
     * If a flat file, we're done; if a directory, recurse and descend.
     */

    if (S_ISDIR(status.st_mode)) {

        dp = opendir(path);
        if (dp != (DIR *)0) {
            /* Do nothing. */
        } else if ((errno == EACCES) || (errno == ENOENT)) {
            diminuto_perror(path);
            path[prior] = '\0';
            return 0;
        } else {
            diminuto_perror(path);
            return -3;
        }

        depth += 1;

        while (!0) {

            errno = 0;
            if ((ep = readdir(dp)) != (struct dirent *)0) {
                /* Do nothing. */
            } else if (errno == 0) {
                break;
            } else {
                diminuto_perror(path);
                return -4;
            }

            if (strcmp(ep->d_name, "..") == 0) {
                /* Do ntohing. */
            } else if (strcmp(ep->d_name, ".") == 0) {
                /* Do ntohing. */
            } else if ((rc = diminuto_fs_walker(ep->d_name, path, total, depth, walkerp, statep)) == 0) {
                /* Do ntohing. */
            } else {
                return rc;
            }

        }

        if (closedir(dp) < 0) {
            diminuto_perror(path);
            return -5;
        }

    }

    path[prior] = '\0';

    if (Debug) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, prior); }

    return 0;
}

int diminuto_fs_walk(const char * root, diminuto_fs_walker_t * walkerp, void * statep)
{
    int rc = 0;
    char real[PATH_MAX] = { '\0', };
    char path[PATH_MAX] = { '\0', };

    if (realpath(root, real) == (char *)0) {
        diminuto_perror(root);
        return -2;
    }

    return diminuto_fs_walker(real, path, 0, 0, walkerp, statep);
}
