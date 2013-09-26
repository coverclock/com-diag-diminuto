/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

static const char * file = "/tmp/unittest-lock.pid";

int main(int argc, char ** argv)
{
    int rc;
    pid_t pid1;
    pid_t pid2;
    pid_t pid3;
    pid_t pid4;

    pid1 = getpid();
    ASSERT(pid1 >= 0);

    pid2 = diminuto_lock_check(file);
    ASSERT(pid2 < 0);

    rc = diminuto_lock_lock(file);
    ASSERT(rc == 0);

    pid3 = diminuto_lock_check(file);
    ASSERT(pid3 > 0);
    ASSERT(pid1 == pid3);

    rc = diminuto_lock_lock(file);
    ASSERT(rc < 0);

    rc = diminuto_lock_unlock(file);
    ASSERT(rc == 0);

    pid4 = diminuto_lock_check(file);
    ASSERT(pid4 < 0);

    rc = diminuto_lock_unlock(file);
    ASSERT(rc < 0);

    EXIT();
}

