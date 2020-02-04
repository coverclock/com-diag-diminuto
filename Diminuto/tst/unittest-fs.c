/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015-2017 Digital Aggregates Corporation, Colorado, USA<BR>
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
        rc = diminuto_fs_walk("/", callback, (void *)0);
        EXPECT(rc == -123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/", callback, "");
        EXPECT(rc == -456);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/", callback, "/");
        EXPECT(rc == 123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/", callback, "/bin/true");
        EXPECT(rc == 123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/", callback, "/usr/local/bin");
        EXPECT(rc == 123);
        STATUS();
    }
    {
        int rc = 0;
        TEST();
        rc = diminuto_fs_walk("/", callback, "/usr/local/include/non-existent");
        EXPECT(rc == 0);
        STATUS();
    }

    EXIT();
}
