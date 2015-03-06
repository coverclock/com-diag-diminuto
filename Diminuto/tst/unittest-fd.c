/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Most of the Diminuto fd API is tested in the mux unit test, for which that
 * portion of the fd API was written.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#define DIMINUTO_FD_TYPE(_FD_, _EXPECTED_) \
    do { \
        diminuto_fd_type_t type; \
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%s=%d type=%d expected=%d\n", #_FD_, _FD_, type = diminuto_fd_type(_FD_), _EXPECTED_); \
        if (_EXPECTED_ >= 0) { EXPECT(type == _EXPECTED_); } \
    } while (0)

int main(void)
{

    SETLOGMASK();

    {
        {
            DIMINUTO_FD_TYPE(STDIN_FILENO, -1);
            DIMINUTO_FD_TYPE(STDOUT_FILENO, -1);
            DIMINUTO_FD_TYPE(STDERR_FILENO, -1);
        }
        {
            DIMINUTO_FD_TYPE(fileno(stdin), -1);
            DIMINUTO_FD_TYPE(fileno(stdout), -1);
            DIMINUTO_FD_TYPE(fileno(stderr), -1);
        }
        {
            int tty;
            ASSERT((tty = open("/dev/tty", O_RDONLY)) >= 0);
            DIMINUTO_FD_TYPE(tty, DIMINUTO_FD_TYPE_TTY);
            ASSERT(close(tty) == 0);
        }
        {
            int sock;
            ASSERT((sock = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
            DIMINUTO_FD_TYPE(sock, DIMINUTO_FD_TYPE_SOCKET);
            ASSERT(close(sock) == 0);
        }
#if 0
        {
            int softlink;
            ASSERT((softlink = open("/dev/fd", O_RDONLY)) >= 0);
            DIMINUTO_FD_TYPE(softlink, DIMINUTO_FD_TYPE_SYMLINK);
            ASSERT(close(softlink) == 0);
        }
#endif
        {
            int file;
            if ((file = open("/bin/busybox", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/usr/bin/busybox", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/system/bin/busybox", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/system/xbin/busybox", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/bin/bash", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/usr/bin/bash", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/system/bin/bash", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((file = open("/system/xbin/bash", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else {
            	file = -1;
            }
            ASSERT(file >= 0);
            DIMINUTO_FD_TYPE(file, DIMINUTO_FD_TYPE_FILE);
            ASSERT(close(file) == 0);
        }
        if ((getuid() == 0) || (geteuid() == 0)) {
            int blockdev;
            if ((blockdev = open("/dev/sda", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((blockdev = open("/dev/ram0", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((blockdev = open("/dev/sr0", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((blockdev = open("/dev/loop0", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else {
                blockdev = -1;
            }
            ASSERT(blockdev >= 0);
            DIMINUTO_FD_TYPE(blockdev, DIMINUTO_FD_TYPE_BLOCKDEV);
            ASSERT(close(blockdev) == 0);
        }
        {
            int dir;
            if ((dir = open("/bin", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((dir = open("/usr/bin", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((dir = open("/system/bin", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else if ((dir = open("/system/xbin", O_RDONLY)) >= 0) {
                /* Do nothing. */
            } else {
                dir = -1;
            }
            ASSERT(dir >= 0);
            DIMINUTO_FD_TYPE(dir, DIMINUTO_FD_TYPE_DIRECTORY);
            ASSERT(close(dir) == 0);
        }
        {
            int chardev;
            ASSERT((chardev = open("/dev/null", O_RDONLY)) >= 0);
            DIMINUTO_FD_TYPE(chardev, DIMINUTO_FD_TYPE_CHARACTERDEV);
            ASSERT(close(chardev) == 0);
        }
        {
            int pipeline[2];
            ASSERT(pipe(pipeline) == 0);
            DIMINUTO_FD_TYPE(pipeline[0], DIMINUTO_FD_TYPE_FIFO);
            DIMINUTO_FD_TYPE(pipeline[1], DIMINUTO_FD_TYPE_FIFO);
            ASSERT(close(pipeline[0]) == 0);
            ASSERT(close(pipeline[1]) == 0);
        }
    }

    EXIT();
}
