/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_unittest.h"
#include "diminuto_coreable.h"
#include "diminuto_map.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char ** argv)
{
    int rc;
    int pagesize;
    void * pointer;
    void * start;
    size_t length;

    rc = diminuto_map_minimum(0);
    EXPECT(rc == 0);

    pagesize = getpagesize();
    ASSERT(pagesize >= 0);

    pointer = diminuto_map(0, pagesize, &start, &length);
    ASSERT(pointer != (void *)0);

    diminuto_emit("unittest-map: *(void *)0=0x%08x\n", *(int *)pointer);

    rc = diminuto_unmap(start, length);
    ASSERT(rc == 0);

    return errors > 255 ? 255 : errors;
}
