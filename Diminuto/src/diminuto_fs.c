/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the File System (FS) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the File System (FS) feature.
 */

#include "com/diag/diminuto/diminuto_types.h"
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
    int xc = 0;
    DIR * dp = (DIR *)0;
    struct dirent * ep = (struct dirent *)0;
    struct stat status = { 0 };
    int rc = 0;
    size_t dd = 0;
    size_t prior = 0;
    size_t length = 0;

    do {

        /*
         * Limit the depth of recursion to something reasonable.
         * This is still pretty big. The check below will limit
         * it as well. This is just a safety valve.
         */

        if (depth > (sizeof(diminuto_path_t) / 2)) {
            errno = E2BIG;
            diminuto_perror(path);
            xc = -3;
            break;
        }

        /*
         * Insure the path buffer has sufficient room. I'd be surprised
         * if this failed on a modern system, but back when MAXPATHLEN
         * was 512 I have seen file systems for which an absolute path
         * string could not be represented.
         */

        length = strnlen(name, sizeof(diminuto_path_t));
        if (length == 0) {
            errno = EINVAL;
            diminuto_perror(path);
            xc = -4;
            break;
        } else if ((total + 1 /* '/' */ + length + 1 /* '\0' */) > sizeof(diminuto_path_t)) {
            errno = EFBIG;
            diminuto_perror(path);
            xc = -5;
            break;
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
         * Get the attributes for the file identified by the path. SOme errors
         * we ignore since the file in question can legitimate change between
         * the opendir(3) and the lstat(2), or because of the lack of privilege
         * of the caller.
         */

        rc = lstat(path, &status);
        if (rc >= 0) {
            /* Do nothing. */
        } else if ((errno == EACCES) || (errno == ENOENT) || (errno == ENOTDIR) || (errno == EOVERFLOW) || (errno == ENODEV)) {
            if (Debug) {
                diminuto_perror(path);
            }
            path[prior] = '\0';
            break;
        } else {
            diminuto_perror(path);
            xc = -6;
            break;
        }

        /*
         * Invoke the callback.
         */

        if (walkerp == (diminuto_fs_walker_t *)0) {
            /* Do nothing. */
        } else if ((rc = (*walkerp)(statep, name, path, depth, &status)) == 0) {
            /* Do nothing. */
        } else {
            xc = rc;
            break;
        }

        /*
         * If a flat file, we're done; if a directory, recurse and descend.
         * Some errors we ignore since the directory in question can
         * legitimately change between the lstat(2) and the opendir(3),
         * or are caused by a lack of privileged on the part of the caller.
         */

        if (S_ISDIR(status.st_mode)) {

            dp = opendir(path);
            if (dp != (DIR *)0) {
                /* Do nothing. */
            } else if ((errno == EACCES) || (errno == ENOENT) || (errno == ENOTDIR) || (errno == ENODEV)) {
                if (Debug) {
                    diminuto_perror(path);
                }
                path[prior] = '\0';
                break;
            } else {
                diminuto_perror(path);
                xc = -7;
                break;
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
                    xc = -8;
                    break;
                }

                /*
                 * See readdir(3): the memory returned by the function
                 * is not dynamically allocated.
                 */

                if (ep->d_name[0] == '\0') {
                    /* Do ntohing. */
                } else if (strcmp(ep->d_name, ".") == 0) {
                    /* Do ntohing. */
                } else if (strcmp(ep->d_name, "..") == 0) {
                    /* Do ntohing. */
                } else if ((rc = diminuto_fs_walker(ep->d_name, path, total, depth, walkerp, statep)) == 0) {
                    /* Do ntohing. */
                } else {
                    xc = rc;
                    break;
                }

            }

        }

        path[prior] = '\0';

        if (Debug) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, prior); }

    } while (0);

    if (dp == (DIR *)0) {
        /* Do nothing. */
    } else if (closedir(dp) < 0) {
        diminuto_perror(path);
        xc = -9;
    } else {
        dp = (DIR *)0;
    }

    return xc;
}

int diminuto_fs_walk(const char * root, diminuto_fs_walker_t * walkerp, void * statep)
{
    int rc = 0;
    diminuto_path_t real = { '\0', };
    diminuto_path_t path = { '\0', };

    if (realpath(root, real) == (char *)0) {
        diminuto_perror(root);
        return -2;
    }

    return diminuto_fs_walker(real, path, 0, 0, walkerp, statep);
}

int diminuto_fs_mkdir_p(const char * path, mode_t mode, int all)
{
    int rc = 0;
    size_t length = 0;
    char * source = (char *)0;
    char * sink = (char *)0;
    char * target = (char *)0;
    char * saveptr = (char *)0;
    char * token = (char *)0;
    static const char SLASH[] = "/";

    do {

        length = strlen(path);
        if (length >= sizeof(diminuto_path_t)) {
            rc = -1;
            errno = ENAMETOOLONG;
            break;
        } else if (length <= 0) {
            break;
        }

        if (Debug) { fprintf(stderr, "%s@%d: path=\"%s\"\n", __FILE__, __LINE__, path); }

        source = strdup(path);

        if (all) {
            /* Do nothing. */
        } else if ((token = strrchr(source, SLASH[0])) == (char *)0) {
            break;
        } else if (token == source) {
            break;
        } else {
            *token = '\0';
        }

        if (Debug) { fprintf(stderr, "%s@%d: source=\"%s\"\n", __FILE__, __LINE__, source); }

        sink = (char *)calloc(length + 1, sizeof(char));

        target = source;

        while ((token = strtok_r(target, SLASH, &saveptr)) != (char *)0) {
            if (Debug) { fprintf(stderr, "%s@%d: token=\"%s\"\n", __FILE__, __LINE__, token); }
            if (token != source) {
                strcat(sink, SLASH);
            }
            strcat(sink, token);
            if (Debug) { fprintf(stderr, "%s@%d: sink=\"%s\"\n", __FILE__, __LINE__, sink); }
            rc = mkdir(sink, mode);
            if (rc == 0) {
                /* Do nothing. */
            } else if (errno == EEXIST) {
                rc = 0;
            } else {
                break;
            }
            target = (char *)0;
        }

    } while (0);

    if (source != (char *)0) {
        free(source);
    }

    if (sink != (char *)0) {
        free(sink);
    }

    return rc;
}

int diminuto_fs_resolve(const char * path, char * absolutepath, size_t size)
{
    int rc = -1;
    char * file = (char *)0;
    diminuto_path_t buffer = { '\0', };

    if ((file = strrchr(path, '/')) > path) {
        *(file++) = '\0';
    }

    if (realpath(path, buffer) == (char *)0) {
        diminuto_perror(path);
    } else {
        strncpy(absolutepath, buffer, size);
        size -= strlen(absolutepath);
        if (file > path) {
            strncat(absolutepath, "/", size);
            size -= 1;
            strncat(absolutepath, file, size);
        }
        rc = 0;
    }

    return rc;
}
