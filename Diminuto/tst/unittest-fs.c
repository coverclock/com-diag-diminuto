/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Most of the Diminuto fd API is tested in the mux unit test, for which that
 * portion of the fd API was written.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static int callback(void * vp, const char * name, const char * path, size_t depth, const struct stat * statp)
{
    if (vp == (void *)0) {
        return -123;
    } else if (*(const char *)vp == '\0') {
        return -456;
    } else if (strcmp(path, (const char *)vp) == 0) {
        return 123;
    } else {
        return 0;
    }
}

int main(void)
{

    SETLOGMASK();

    {
        mode_t mode = 0;
        diminuto_fs_type_t type = DIMINUTO_FS_TYPE_NONE;
        int ii;
        TEST();
        for (ii = 0x0; ii <= 0xf; ++ii) {
            mode = ii << 12;
            type = diminuto_fs_type(mode);
            COMMENT("mode=0%06o type='%c'\n", ii << 12, type);
            EXPECT(type != DIMINUTO_FS_TYPE_NONE);
            EXPECT(type != DIMINUTO_FS_TYPE_UNKNOWN);
        }
        STATUS();
    }

    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk(".", callback, (void *)0);
        EXPECT(rc == -123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk(".", callback, "");
        EXPECT(rc == -456);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/usr/local/bin", callback, "/usr/local/bin");
        EXPECT(rc == 123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/bin", callback, "/bin/true");
        EXPECT(rc == 123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/usr/local", callback, "/usr/local/bin");
        EXPECT(rc == 123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/usr/local", callback, "/usr/local/include/com-diag-diminuto-fs-non-existent");
        EXPECT(rc == 0);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("", 0755, 0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("", 0755, !0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/", 0755, 0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/", 0755, !0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs", &status);
        ASSERT(rc < 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs", &status);
        ASSERT(rc == 0);
        ASSERT(S_ISDIR(status.st_mode));
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("./unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("./unittest-fs", &status);
        ASSERT(rc < 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("./unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("./unittest-fs", &status);
        ASSERT(rc == 0);
        ASSERT(S_ISDIR(status.st_mode));
        rc = rmdir("./unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/tmp/unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("/tmp/unittest-fs", &status);
        ASSERT(rc < 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/tmp/unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("/tmp/unittest-fs", &status);
        ASSERT(rc == 0);
        ASSERT(S_ISDIR(status.st_mode));
        rc = rmdir("/tmp/unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs/unittest-fs", &status);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs/unittest-fs/unittest-fs", &status);
        ASSERT(rc < 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs/unittest-fs/unittest-fs", &status);
        ASSERT(rc == 0);
        ASSERT(S_ISDIR(status.st_mode));
        rc = rmdir("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs/", 0755, 0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs/unittest-fs/unittest-fs", &status);
        ASSERT(rc == 0);
        ASSERT(S_ISDIR(status.st_mode));
        rc = rmdir("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    {
        int rc = 0;
        int debug = 0;
        struct stat status;
        TEST();
        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs/", 0755, !0);
        ASSERT(rc == 0);
        memset(&status, 0, sizeof(status));
        rc = stat("unittest-fs/unittest-fs/unittest-fs", &status);
        ASSERT(rc == 0);
        ASSERT(S_ISDIR(status.st_mode));
        rc = rmdir("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);
        STATUS();
    }

    EXIT();
}
