/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Buffer Pool feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Buffer Pool feature.
 */

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_buffer_pool.h"

int main(void)
{
    SETLOGMASK();

    {
        size_t MYPOOL[] = { 0 }; /* Forces buffer to malloc(3) from heap. */
        void * mypool[countof(MYPOOL)] = { (void *)0 };
        diminuto_buffer_pool_t mine = { countof(MYPOOL), MYPOOL, mypool };
        void * one;
        TEST();
        EXPECT(diminuto_buffer_pool_log(&mine) == 0);
        ASSERT((one = diminuto_buffer_pool_get(&mine, 1, !0)) == (void *)0);
        ASSERT(errno == ENOMEM);
        ASSERT((one = diminuto_buffer_pool_get(&mine, 1, 0)) != (void *)0);
        diminuto_buffer_pool_put(&mine, one);
        EXPECT(diminuto_buffer_pool_log(&mine) == 0);
        diminuto_buffer_pool_free(&mine);
        EXPECT(diminuto_buffer_pool_log(&mine) == 0);
        STATUS();
    }

    {
        size_t MYPOOL[] = { 10, 100, 1000 };
        void * mypool[countof(MYPOOL)] = { (void *)0 };
        diminuto_buffer_pool_t mine = { countof(MYPOOL), MYPOOL, mypool };
        int ii;
        size_t requested;
        void * buffer[14][2] = { (void *)0 };
        TEST();
        EXPECT(diminuto_buffer_pool_log(&mine) == 0);
        for (ii = 0, requested = 1; requested <= (1 << 13); ++ii, requested <<= 1) {
            buffer[ii][0] = diminuto_buffer_pool_get(&mine, requested, 0);
            ASSERT(buffer[ii][0] != (void *)0);
            memset(buffer[ii][0], 0xa5, requested); /* For valgrind(1). */
            buffer[ii][1] = diminuto_buffer_pool_get(&mine, requested, 0);
            ASSERT(buffer[ii][1] != (void *)0);
            memset(buffer[ii][1], 0x5a, requested); /* For valgrind(1). */
        }
        EXPECT(diminuto_buffer_pool_log(&mine) == 0);
        for (--ii; ii >= 0; --ii) {
            diminuto_buffer_pool_put(&mine, buffer[ii][1]);
            diminuto_buffer_pool_put(&mine, buffer[ii][0]);
        }
        EXPECT(diminuto_buffer_pool_log(&mine) > 0);
        diminuto_buffer_pool_free(&mine);
        EXPECT(diminuto_buffer_pool_log(&mine) == 0);
        STATUS();
    }

    {
        size_t MYPOOL[] = { 10, 100, 1000 };
        void * mypool[countof(MYPOOL)] = { (void *)0 };
        diminuto_buffer_pool_t mine = { countof(MYPOOL), MYPOOL, mypool };
        TEST();
        EXPECT(diminuto_buffer_pool_init(&mine, 3, 10) == &mine);
        EXPECT(diminuto_buffer_pool_init(&mine, 5, 100) == &mine);
        EXPECT(diminuto_buffer_pool_init(&mine, 7, 1000) == &mine);
        EXPECT(diminuto_buffer_pool_init(&mine, 11, 10000) == (diminuto_buffer_pool_t *)0);
        EXPECT(diminuto_buffer_pool_fini(&mine) == (diminuto_buffer_pool_t *)0);
        STATUS();
    }

    EXIT();
}
