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

int main(void)
{

    SETLOGMASK();

    {
        mode_t mode = 0;
        diminuto_fs_type_t type = DIMINUTO_FS_TYPE_NONE;
        int ii;
        for (ii = 0x0; ii <= 0xf; ++ii) {
            mode = ii << 12;
            type = diminuto_fs_type(mode);
            COMMENT("mode=0%06o type='%c'\n", ii << 12, type);
            EXPECT(type != DIMINUTO_FS_TYPE_NONE);
            EXPECT(type != DIMINUTO_FS_TYPE_UNKNOWN);
        }
    }

    EXIT();
}
