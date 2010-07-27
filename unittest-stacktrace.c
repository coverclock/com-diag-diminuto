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
#include "diminuto_number.h"
#include "diminuto_countof.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int f(int depth, int limit)
{
    int fd;
    int count;
    void * buffer[16] = { 0 };
    int ndx;

    if (depth > 0) {
        count = f(depth - 1, limit);
        count = count + 1;
        return count;
    }

    fd = open("/dev/null", O_WRONLY);
    ASSERT(fd >= 0);

    count = diminuto_stacktrace3(buffer, countof(buffer), fd);

    close(fd);

    printf("unittest-stacktrace: countof(buffer)=%lu limit=%d count=%d\n", countof(buffer), limit, count);

    ASSERT(count > 0);
    for (ndx = 0; ndx < count; ++ndx) {
        EXPECT(buffer[ndx] != 0);
    }

    diminuto_stacktrace();

    return 0;
}

int main(int argc, char ** argv)
{
    diminuto_unsigned_t value;

    ASSERT(argc == 2);
    ASSERT(*diminuto_number(argv[1], &value) == '\0');
    ASSERT(f(value, value) == value);

    return 0;
}
