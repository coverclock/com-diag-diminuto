/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Map feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Map feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_map.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char ** argv)
{
    int rc;
    int pagesize;
    void * pointer;
    void * start = (void *)0;
    size_t length = 0;

    SETLOGMASK();

    rc = diminuto_map_minimum(0);
    EXPECT(rc == 0);

    pagesize = getpagesize();
    ASSERT(pagesize > 0);

    pointer = diminuto_map_map(0, pagesize, &start, &length);
    ASSERT(pointer != (void *)0);
    ASSERT(start != (void *)0);
    ASSERT(length != 0);

    DIMINUTO_LOG_NOTICE("unittest-map: *(void *)0=0x%08x start=0x%08x length=%lu\n", *(int *)pointer, start, length);

    rc = diminuto_map_unmap(&start, &length);
    ASSERT(rc == 0);
    ASSERT(start == (void *)0);
    ASSERT(length == 0);

    EXIT();
}
