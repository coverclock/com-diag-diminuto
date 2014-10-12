/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, void ** argv)
{
    void * buffer = 0;
    int fd = -1;
    ssize_t rc = 0;
    uint64_t sn = 0;
    static const size_t SIZE = 4096;

    buffer = diminuto_fd_direct_alloc(SIZE);
    ASSERT(buffer != 0);
    EXPECT(((intptr_t)buffer & (intptr_t)0x1ff) == 0);
    fd = diminuto_fd_direct_acquire(fd, "/tmp/FILE", O_CREAT, 0777);
    EXPECT(fd >= 0);
    fd = diminuto_fd_relinquish(fd, "");
    EXPECT(fd < 0);

    EXIT();
}
