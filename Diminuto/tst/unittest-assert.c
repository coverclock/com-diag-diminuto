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
#include <errno.h>

int main(int argc, char ** argv)
{

/*
 * Should print nothing.
 */

#define COM_DIAG_DIMINUTO_ASSERT_NDEBUG (!0)
#include "com/diag/diminuto/diminuto_assert.h"

    {
        int condition;

        TEST();
    
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
    
        condition = 0;
        errno = EINVAL;
        diminuto_expect(++condition);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_assert(++condition);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_expect(condition++);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_assert(condition++);
        EXPECT(condition == 1);
    
        condition = 1;
        errno = E2BIG;
        diminuto_assert(condition);
    
        condition = 0;
        errno = EAGAIN;
        diminuto_assert(condition);

        STATUS();
    }

/*
 * Should print errors but succeed.
 */

#undef COM_DIAG_DIMINUTO_ASSERT_NDEBUG
#define COM_DIAG_DIMINUTO_ASSERT_DEBUG (!0)
#include "com/diag/diminuto/diminuto_assert.h"

    {
        int condition;

        TEST();
    
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
    
        condition = 0;
        errno = EINVAL;
        diminuto_expect(++condition);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_assert(++condition);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_expect(condition++);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_assert(condition++);
        EXPECT(condition == 1);
    
        condition = 1;
        errno = E2BIG;
        diminuto_assert(condition);
    
        condition = 0;
        errno = EAGAIN;
        diminuto_assert(condition);

        STATUS();
    }

/*
 * Should print errors and abort.
 */

#undef COM_DIAG_DIMINUTO_ASSERT_DEBUG
#include "com/diag/diminuto/diminuto_assert.h"

    {
        int condition;

        TEST();
    
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
    
        condition = 0;
        errno = EINVAL;
        diminuto_expect(++condition);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_assert(++condition);
        EXPECT(condition == 1);
    
        condition = 0;
        errno = EINVAL;
        diminuto_expect(condition++);
        EXPECT(condition == 1);
   
        /* 
        condition = 0;
        errno = EINVAL;
        diminuto_assert(condition++);
        EXPECT(condition == 1);
        */
    
        condition = 1;
        errno = E2BIG;
        diminuto_assert(condition);
    
        condition = 0;
        errno = EAGAIN;
        diminuto_assert(condition);

        STATUS();
    }

    EXIT();
}

/*
2020-06-19T15:24:36.792638Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[23]: FAIL! (condition < 0) <0> "" {0}
2020-06-19T15:24:36.793400Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[25]: FAIL! (condition == 0) <0> "" {0}
2020-06-19T15:24:36.793463Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[29]: FAIL! (condition > 0) <22> "Invalid argument" {0}
2020-06-19T15:24:36.793507Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[31]: FAIL! (condition < 0) <9> "Bad file descriptor" {0}
2020-06-19T15:24:36.793549Z "gold" <EROR> [5837] {76fac000} tst/unittest-assert.c[41]: FAIL! (condition) <11> "Resource temporarily unavailable" {1}
*/
