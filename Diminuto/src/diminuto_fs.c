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
#include <sys/types.h>
#include <sys/stat.h>

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
