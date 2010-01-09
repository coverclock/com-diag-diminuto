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
#include "diminuto_stacktrace.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

void f5(void)
{
    int fd;
    int count;
    void * buffer[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int ndx;
    fd = open("/dev/null", O_WRONLY);
    count = diminuto_stacktrace3(buffer, sizeof(buffer) / sizeof(buffer[0]), fd);
    close(fd);
    for (ndx = 0; ndx < count; ++ndx) {
        EXPECT(buffer[ndx] != 0);
    }
    diminuto_stacktrace();
}

void f4(void) { f5(); }

void f3(void) { f4(); }

void f2(void) { f3(); }

void f1(void) { f2(); }

void f0(void) { f1(); }

int main(int argc, char ** argv)
{
    f0();
    return 0;
}
