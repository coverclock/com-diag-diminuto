/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the FS and FD functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Use the FS and FD features to figure out stuff about a file.
 */

#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static const char * type2name(diminuto_fs_type_t type)
{
    const char * name = (const char *)0;

    switch (type) {
    case DIMINUTO_FS_TYPE_BLOCKDEV:     name = "BLOCKDEV";  break;
    case DIMINUTO_FS_TYPE_CHARACTERDEV: name = "CHARDEV";   break;
    case DIMINUTO_FS_TYPE_DIRECTORY:    name = "DIR";       break;
    case DIMINUTO_FS_TYPE_FIFO:         name = "FIFO";      break;
    case DIMINUTO_FS_TYPE_FILE:         name = "FILE";      break;
    case DIMINUTO_FS_TYPE_NONE:         name = "NONE";      break;
    case DIMINUTO_FS_TYPE_SOCKET:       name = "SOCK";      break;
    case DIMINUTO_FS_TYPE_SYMLINK:      name = "SYMLINK";   break;
    case DIMINUTO_FS_TYPE_TTY:          name = "TTY";       break;
    case DIMINUTO_FS_TYPE_UNKNOWN:      name = "UNKNOWN";	break;
    default:                            name = "ERROR";     break;
    }

    return name;
}

int main(int argc, char * argv[]) {
    int xc = 0;
    const char * program = (const char *)0;
    int ii = 0;
    const char * path = (const char *)0;
    int rc = -1;
    int fd = -1;
    diminuto_path_t buffer = { '\0', };
    diminuto_fs_type_t ogtype = DIMINUTO_FS_TYPE_UNKNOWN;
    diminuto_fs_type_t fstype = DIMINUTO_FS_TYPE_UNKNOWN;
    diminuto_fd_type_t fdtype = DIMINUTO_FS_TYPE_UNKNOWN;

    do {

        program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

        for (ii = 1; ii < argc; ++ii) {

            path = argv[ii];

            ogtype = diminuto_fs_type(path);

            if ((rc = diminuto_fs_expand(path, buffer, sizeof(buffer))) < 0) {
                xc = 1;
                continue;
            }

            fstype = diminuto_fs_type(buffer);

            if ((fd = open(buffer, 0)) < 0) {
                diminuto_perror(buffer);
                xc = 2;
                continue;
            }

            fdtype = diminuto_fd_type(fd);

            if (close(fd) < 0) {
                diminuto_perror(buffer);
                xc = 3;
            }

            printf("%s: \"%s\" %s \"%s\" %s [%d] %s\n", program, path, type2name(ogtype), buffer, type2name(fstype), fd, type2name(fdtype));

        }

    } while (0);

    return xc;
}
