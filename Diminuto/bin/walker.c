/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * ABSTRACT
 *
 * walker walks the file system tree at the specified root or by default
 * at the current directory and displays the tree on standard output and
 * the attributes of each entry on standard error.
 *
 * USAGE
 *
 * walker [ root [ root ... ] ]
 *
 * EXAMPLES
 *
 * walker
 * walker .
 * walker /
 * walker foo/bar
 */

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
#include <assert.h>

static const int DEBUG = 0;

static char classify(mode_t mode)
{
    char class = '\0';

    if (S_ISREG(mode)) {
        class = '-';
    } else if (S_ISDIR(mode)) {
        class = 'd';
    } else if (S_ISLNK(mode)) {
        class = 'l';
    } else if (S_ISCHR(mode)) {
        class = 'c';
    } else if (S_ISBLK(mode)) {
        class = 'b';
    } else if (S_ISFIFO(mode)) {
        class = 'p';
    } else if (S_ISSOCK(mode)) {
        class = 's';
    } else {
        class = '?';
    }

    return class;
}

static int walk(const char * name, char * path, size_t total, size_t depth)
{
    int fc = 0;
    DIR * dp = (DIR *)0;
    struct dirent * ep = (struct dirent *)0;
    struct stat status = { 0 };
    int rc = 0;
    size_t dd = 0;
    size_t prior = 0;
    size_t length = 0;

    /*
     * If we're at the root of the tree, initialize the path buffer.
     */

    if (depth == 0) {
        path[0] = '\0';
        path[PATH_MAX - 1] = '\0';
        total = 0;
    }

    /*
     * Insure the path buffer has sufficient room. I'd be surprised
     * if this failed on a modern system, but back when MAXPATHLEN
     * was 512 I have seen file systems for which an absolute path
     * string could not be represented.
     */

    length = strnlen(name, PATH_MAX);
    if ((total + 1 /* '/' */ + length + 1 /* '\0' */) > PATH_MAX) {
        errno = E2BIG;
        perror(path);
        return -1;
    }

    if (DEBUG) { fprintf(stderr, "%s@%d: \"%s\" [%zu] \"%s\" [%zu]\n", __FILE__, __LINE__, path, total, name, length); }

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

    if (DEBUG) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, total); }

    /*
     * Get the attributes for the file identified by the path.
     */

    rc = lstat(path, &status);
    if (rc < 0) {
        perror("stat");
        return -2;
    }

    /*
     * Display the file in the tree on stdout, and its attribytes
     * on stderr.
     */

    for (dd = 0; dd < depth; ++dd) {
        fputc(' ', stdout);
    }
    fputs(name, stdout);
    if (S_ISDIR(status.st_mode)) { fputc('/', stdout); }
    fputc('\n', stdout);

    fprintf(stderr,
        "%s %c 0%o (%d,%d) #%d [%d] %d:%d <%d,%d> [%d] [%d] [%d] %d.%09d %d.%09d %d.%09d\n"
        , path
        , classify(status.st_mode)
        , (status.st_mode & ~S_IFMT)
        , major(status.st_dev), minor(status.st_dev)
        , status.st_ino
        , status.st_nlink
        , status.st_uid
        , status.st_gid
        , major(status.st_rdev), minor(status.st_rdev)
        , status.st_size
        , status.st_blksize
        , status.st_blocks * 512
        , status.st_atim.tv_sec, status.st_atim.tv_nsec
        , status.st_mtim.tv_sec, status.st_mtim.tv_nsec
        , status.st_ctim.tv_sec, status.st_ctim.tv_nsec
    );

    /*
     * If a flat file, we're done; if a directory, recurse and descend.
     */

    if (S_ISDIR(status.st_mode)) {

        dp = opendir(path);
        if (dp == (DIR *)0) {
            perror("opendir");
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
                perror("readdir");
                fc = -4;
                break;
            }

            if (strcmp(ep->d_name, "..") == 0) {
                /* Do ntohing. */
            } else if (strcmp(ep->d_name, ".") == 0) {
                /* Do ntohing. */
            } else if ((rc = walk(ep->d_name, path, total, depth)) == 0) {
                /* Do ntohing. */
            } else {
                fc = rc;
                break;
            }

        }

        if (closedir(dp) < 0) {
            perror("closedir");
            return -5;
        }

    }

    path[prior] = '\0';
    if (DEBUG) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, prior); }

    return fc;
}

int main(int argc, char * argv[])
{
    int xc = 0;
    int rc = 0;
    int ii = 0;
    char path[PATH_MAX] = { '\0', };

    if (argc <= 1) {
        xc = walk(".", path, 0, 0);
    } else {
        for (ii = 1; ii < argc; ++ii) {
            rc = walk(argv[ii], path, 0, 0);
            if (xc == 0) {
                xc = rc;
            }
        }
    }

    return (xc < 0) ? -xc : xc;
}
