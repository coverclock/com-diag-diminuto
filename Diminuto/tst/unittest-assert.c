/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include <errno.h>

int main(int argc, char ** argv)
{
    int condition;

    condition = 1;
    errno = 0;
    diminuto_expect(condition > 0);
    errno = 0;
    diminuto_expect(condition < 0);
    errno = 0;
    diminuto_expect(condition == 0);

    condition = 0;
    errno = EINVAL;
    diminuto_expect(condition > 0);
    errno = EBADF;
    diminuto_expect(condition < 0);
    errno = EACCES;
    diminuto_expect(condition == 0);

    condition = 1;
    errno = E2BIG;
    diminuto_assert(condition);

    condition = 0;
    errno = EAGAIN;
    diminuto_assert(condition);
}

/*
2020-06-19T15:24:36.792638Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[23]: FAIL! (condition < 0) <0> "" {0}
2020-06-19T15:24:36.793400Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[25]: FAIL! (condition == 0) <0> "" {0}
2020-06-19T15:24:36.793463Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[29]: FAIL! (condition > 0) <22> "Invalid argument" {0}
2020-06-19T15:24:36.793507Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[31]: FAIL! (condition < 0) <9> "Bad file descriptor" {0}
2020-06-19T15:24:36.793549Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[41]: FAIL! (condition) <11> "Resource temporarily unavailable" {1}
*/
