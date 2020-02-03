/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
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
#include "com/diag/diminuto/diminuto_fd.h"

static int callback(void * statep, const char * name, const char * path, size_t depth, const struct stat * statp)
{
    size_t dd = 0;
    size_t ll = 0;
    FILE ** files;

    files = (FILE **)statep;

    ll = strlen(name);

    for (dd = 0; dd < depth; ++dd) {
        fputc(' ', files[0]);
    }
    fputs(name, files[0]);

    if (ll <= 0) {
        /* Do nothing. */
    } else if (name[ll - 1] == '/') {
        /* Do nothing. */
    } else if (!S_ISDIR(statp->st_mode)) {
        /* Do nothing. */
    } else {
        fputc('/', files[0]);
    }
    fputc('\n', files[0]);

    /*
     * N.B. Files with the same inode numbers are hardlinked to one
     * another: they are aliases for the same file.
     */

    fprintf(files[1],
        "%lu '%c' 0%o (%u,%u) [%zu] %u:%u <%u,%u> [%zu] [%zu] [%zu] %ld.%09lu %ld.%09lu %ld.%09lu \"%s\"\n"
        , statp->st_ino
        , diminuto_fs_type(statp->st_mode)
        , (statp->st_mode & ~S_IFMT)
        , major(statp->st_dev), minor(statp->st_dev)
        , (size_t)statp->st_nlink
        , statp->st_uid
        , statp->st_gid
        , major(statp->st_rdev), minor(statp->st_rdev)
        , (size_t)statp->st_size
        , (size_t)statp->st_blksize
        , (size_t)(statp->st_blocks * 512)
        , statp->st_atim.tv_sec, (unsigned long)statp->st_atim.tv_nsec
        , statp->st_mtim.tv_sec, (unsigned long)statp->st_mtim.tv_nsec
        , statp->st_ctim.tv_sec, (unsigned long)statp->st_ctim.tv_nsec
        , path /* May contain spaces or other problematic characters. */
    );

    return 0;
}

int main(int argc, char * argv[])
{
    int xc = 0;
    int rc = 0;
    int ii = 0;
    FILE * files[2] = { (FILE *)0, (FILE *)0, };

    /*
     * Just to test the state variable.
     */

    files[0] = stdout;
    files[1] = stderr;

    if (argc <= 1) {
        xc = diminuto_fs_walk(".", callback, files);
    } else {
        for (ii = 1; ii < argc; ++ii) {
            rc = diminuto_fs_walk(argv[ii], callback, files);
            if (xc == 0) {
                xc = rc;
            }
        }
    }

    return (xc < 0) ? -xc : xc;
}
