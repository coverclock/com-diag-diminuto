/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the File feature poll function.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        FILE * fp;
        int fd;
        int ch;
        int rc;
        static const size_t LIMIT = 1024 * 1024 * 1024;
        size_t total = 0;
        size_t reads = 0;
        size_t noreads = 0;
        ssize_t maximum = 0;
        ssize_t bytes;

        TEST();

        fp = fopen("/dev/zero", "r");
        ASSERT(fp != (FILE *)0);

        fd = fileno(fp);
        ASSERT(fd >= 0);
        ASSERT(fd == fp->_fileno);

        while (total < LIMIT) {
            bytes = diminuto_file_poll(fp);
            COMMENT("total %lu bytes %ld\n", total, bytes);
            ASSERT(bytes > 0);
            if (bytes > maximum) {
                maximum = bytes;
            }
            if (bytes > 1) {
                /*
                 * Temporarily poison file descriptor in FILE object
                 * to insure no physical I/O.
                 */
                fp->_fileno = -1;
                noreads += bytes;
            } else {
                reads += bytes;
            }
            while ((bytes--) > 0) {
                ch = fgetc(fp);
                ASSERT(ch == 0);
                total += 1;
            }
            fp->_fileno = fd;
        }

        rc = fclose(fp);
        ASSERT(rc == 0);

        CHECKPOINT("total %lu bufsize %d maximum %ld reads %lu noreads %lu sum %lu\n", total, BUFSIZ, maximum, reads, noreads, reads + noreads);

        STATUS();
    }

    EXIT();
}
