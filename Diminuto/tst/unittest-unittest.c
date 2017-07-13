/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv)
{
    int status;
    SETLOGMASK();
    TEST();
    CHECKPOINT();
    COMMENT();
    EXPECT(!0);
    ASSERT(!0);
    STATUS();
    TEST("Test");
    CHECKPOINT("%s", argv[0]);
    COMMENT("%s", argv[0]);
    if (fork() == 0) {
        EXPECT(0);
        ASSERT(0);
        STATUS("Status");
        EXIT("Exit");
        exit(0);
    }
    if (fork() == 0) {
        FATAL("Fatal");
        FAILURE();
        EXIT();
        exit(0);
    }
    if (fork() == 0) {
        FATAL();
        FAILURE();
        EXIT();
        exit(0);
    }
    if (fork() == 0) {
        PANIC("Panic");
        FAILURE();
        EXIT();
        exit(0);
    }
    if (fork() == 0) {
        PANIC();
        FAILURE();
        EXIT();
        exit(0);
    }
    if (fork() == 0) {
        FAILURE();
        STATUS();
        EXIT();
        exit(0);
    }
    if (fork() == 0) {
        FAILURE("Failure");
        STATUS("Status");
        EXIT("Exit");
        exit(0);
    }
    while (!0) {
        status = 0;
        if (wait(&status) < 0) {
            break;
        }
        EXPECT(status != 0);
    }
    STATUS();
    EXIT();
}
