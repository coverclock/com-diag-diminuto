/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * N.B. This unit test suite uses knowledge of the internal implementation.
 * It is NOT a blackbox test.
 */

#include <string.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_countof.h"

static int memverify(void * s, int c, size_t n)
{
    unsigned char * p;
    unsigned char ch;
    for (p = (char *)s, ch = c; n > 0; --n, ++p) {
        if (*p != ch) {
            return 0;
        }
    }
    return !0;
}

int main(void)
{
    SETLOGMASK();

    {
        size_t ii;
        ASSERT(diminuto_buffer_countof == 10);
        for (ii = 0; ii < diminuto_buffer_countof; ++ii) {
            ASSERT(DIMINUTO_BUFFER_POOL[ii] == (1 << (ii + 3)));
        }
        for (ii = 0; ii < diminuto_buffer_countof; ++ii) {
            ASSERT(diminuto_buffer_pool[ii] == (diminuto_buffer_t *)0);
        }
        STATUS();
    }

    {
        size_t requested;
        size_t actual;
        size_t expected;
        size_t effective;
        size_t item;
        size_t hash;
        size_t header;
        for (requested = 0; requested <= 8192; ++requested) {
            if (requested <= (expected = 8)) {
                item = 0;
            } else if (requested <= (expected = 16)) {
                item = 1;
            } else if (requested <= (expected = 32)) {
                item = 2;
            } else if (requested <= (expected = 64)) {
                item = 3;
            } else if (requested <= (expected = 128)) {
                item = 4;
            } else if (requested <= (expected = 256)) {
                item = 5;
            } else if (requested <= (expected = 512)) {
                item = 6;
            } else if (requested <= (expected = 1024)) {
                item = 7;
            } else if (requested <= (expected = 2048)) {
                item = 8;
            } else if (requested <= (expected = 4096)) {
                item = 9;
            } else {
                expected = requested;
                item = 10;
            }
            expected += sizeof(diminuto_buffer_t);
            hash = diminuto_buffer_hash(requested, &actual);
            header = (item < diminuto_buffer_countof) ? item : expected;
            effective = diminuto_buffer_effective(header);
#if 0
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%zu %zu %zu %zu %zu %d %d\n", requested, actual, header, effective, expected, item, hash);
#endif
            ASSERT(hash == item);
            ASSERT(actual == expected);
            ASSERT(effective == expected);
        }
        STATUS();
    }

    {
        EXPECT(diminuto_buffer_log() == 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        STATUS();
    }

    {
        void * one;
        void * two;
        void * three;
        void * four;
        void * five;
        ASSERT(!diminuto_buffer_debug(!0));
        EXPECT(diminuto_buffer_log() == 0);
        /**/
        one = diminuto_buffer_malloc(0);
        EXPECT(one == (void *)0);
        EXPECT(errno == 0);
        one = diminuto_buffer_realloc((void *)0, 0);
        EXPECT(one == (void *)0);
        EXPECT(errno == 0);
        one = diminuto_buffer_calloc(0, 1);
        EXPECT(one == (void *)0);
        EXPECT(errno == 0);
        one = diminuto_buffer_calloc(1, 0);
        EXPECT(one == (void *)0);
        EXPECT(errno == 0);
        one = diminuto_buffer_calloc(0, 0);
        EXPECT(one == (void *)0);
        EXPECT(errno == 0);
        /**/
        one = diminuto_buffer_malloc(1);
        ASSERT(one != (void *)0);
        memset(one, 0xa5, 1);
        two = diminuto_buffer_realloc(one, 2);
        ASSERT(two != (void *)0);
        EXPECT(one == two); /* Optimization. */
        EXPECT(memverify(two, 0xa5, 1));
        memset(two, 0x5a, 2); /* For valgrind(1). */
        /**/
        one = diminuto_buffer_malloc(3);
        ASSERT(one != (void *)0);
        memset(one, 0xa5, 3);
        three = diminuto_buffer_realloc(one, 9);
        ASSERT(three != (void *)0);
        EXPECT(three != one);
        EXPECT(memverify(three, 0xa5, 3));
        memset(three, 0x5a, 9); /* For valgrind(1). */
        /**/
        one = diminuto_buffer_calloc(3, 5);
        ASSERT(one != (void *)0);
        EXPECT(memverify(one, 0, 15));
        memset(one, 0xa5, 3 * 5);
        four = diminuto_buffer_realloc(one, 4097);
        ASSERT(four != (void *)0);
        EXPECT(four != one);
        EXPECT(memverify(four, 0xa5, 15));
        memset(four, 0x5a, 4097); /* For valgrind(1). */
        /**/
        one = diminuto_buffer_malloc(7);
        ASSERT(one != (void *)0);
        memset(one, 0xa5, 7);
        five = diminuto_buffer_realloc(one, 0);
        EXPECT(five == (void *)0);
        /**/
        one = diminuto_buffer_realloc((void *)0, 11);
        ASSERT(one != (void *)0);
        memset(one, 0x5a, 11); /* For valgrind(1). */
        /**/
        EXPECT(diminuto_buffer_log() > 0);
        /**/
        diminuto_buffer_free(four);
        diminuto_buffer_free(three);
        diminuto_buffer_free(two);
        diminuto_buffer_free(one);
        diminuto_buffer_free((void *)0);
        /**/
        EXPECT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        ASSERT(diminuto_buffer_debug(0));
        STATUS();
    }

    {
        int ii;
        size_t requested;
        void * buffer[14][2] = { (void *)0 };
        EXPECT(diminuto_buffer_log() == 0);
        for (ii = 0, requested = 1; requested <= (1 << 13); ++ii, requested <<= 1) {
            buffer[ii][0] = diminuto_buffer_malloc(requested);
            ASSERT(buffer[ii][0] != (void *)0);
            memset(buffer[ii][0], 0xa5, requested); /* For valgrind(1). */
            buffer[ii][1] = diminuto_buffer_malloc(requested);
            ASSERT(buffer[ii][1] != (void *)0);
            memset(buffer[ii][1], 0x5a, requested); /* For valgrind(1). */
        }
        EXPECT(diminuto_buffer_log() == 0);
        for (--ii; ii >= 0; --ii) {
            diminuto_buffer_free(buffer[ii][1]);
            diminuto_buffer_free(buffer[ii][0]);
        }
        EXPECT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        STATUS();
    }

    {
        static char STRING[] = "England expects each man to do his duty.";
        char * string;
        EXPECT(diminuto_buffer_log() == 0);
        /**/
        string = diminuto_buffer_strdup("");
        ASSERT(string != (char *)0);
        ASSERT(strcmp(string, "") == 0);
        diminuto_buffer_free(string);
        /**/
        string = diminuto_buffer_strdup(STRING);
        ASSERT(string != (char *)0);
        ASSERT(strcmp(string, STRING) == 0);
        diminuto_buffer_free(string);
        /**/
        string = diminuto_buffer_strndup("", ~(size_t)0);
        ASSERT(string != (char *)0);
        ASSERT(strcmp(string, "") == 0);
        diminuto_buffer_free(string);
        /**/
        string = diminuto_buffer_strndup(STRING, 0);
        ASSERT(string != (char *)0);
        ASSERT(strcmp(string, "") == 0);
        diminuto_buffer_free(string);
        /**/
        string = diminuto_buffer_strndup(STRING, strlen("England"));
        ASSERT(string != (char *)0);
        ASSERT(strcmp(string, "England") == 0);
        diminuto_buffer_free(string);
        /**/
        string = diminuto_buffer_strndup(STRING, ~(size_t)0);
        ASSERT(string != (char *)0);
        ASSERT(strcmp(string, STRING) == 0);
        diminuto_buffer_free(string);
        /**/
        EXPECT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        STATUS();
    }

    {
        void * pointer;
        ASSERT(!diminuto_buffer_debug(!0));
        EXPECT(diminuto_buffer_log() == 0);
        /**/
        ASSERT(!diminuto_buffer_nomalloc(!0));
        /**/
        ASSERT(diminuto_buffer_malloc(0) == (void *)0);
        ASSERT(errno == 0);
        ASSERT(diminuto_buffer_malloc(1) == (void *)0);
        ASSERT(errno == ENOMEM);
        ASSERT(diminuto_buffer_malloc(~(size_t)0) == (void *)0);
        ASSERT(errno == ENOMEM);
        ASSERT(diminuto_buffer_calloc(1, 1) == (void *)0);
        ASSERT(errno == ENOMEM);
        ASSERT(diminuto_buffer_realloc((void *)0, 1) == (void *)0);
        ASSERT(errno == ENOMEM);
        ASSERT(diminuto_buffer_prealloc(1, 1) == 0);
        ASSERT(errno == ENOMEM);
        /**/
        ASSERT(diminuto_buffer_nomalloc(0));
        /**/
        EXPECT(diminuto_buffer_log() == 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        ASSERT(diminuto_buffer_debug(0));
        STATUS();
    }

    {
        void * one;
        void * two;
        void * three;
        void * four;
        /**/
        EXPECT(diminuto_buffer_log() == 0);
        ASSERT(diminuto_buffer_prealloc(0, 0) == 0);
        ASSERT(diminuto_buffer_prealloc(1, 0) == 0);
        ASSERT(diminuto_buffer_prealloc(0, 1) == 0);
        ASSERT(diminuto_buffer_prealloc(1, (1 << 12) + 1) == 0);
        ASSERT(diminuto_buffer_prealloc(1, ~(size_t)0) == 0);
        ASSERT(diminuto_buffer_prealloc(1, 1) > 0);
        ASSERT(diminuto_buffer_prealloc(2, 1 << 12) > 0);
        ASSERT(!diminuto_buffer_nomalloc(!0));
        EXPECT(diminuto_buffer_log() > 0);
        /**/
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        ASSERT((two = diminuto_buffer_malloc(1 << 12)) != (void *)0);
        ASSERT((three = diminuto_buffer_malloc(1 << 12)) != (void *)0);
        ASSERT((four = diminuto_buffer_malloc(1)) == (void *)0);
        ASSERT(errno == ENOMEM);
        ASSERT((four = diminuto_buffer_malloc(1 << 12)) == (void *)0);
        ASSERT(errno == ENOMEM);
        /**/
        diminuto_buffer_free(three);
        diminuto_buffer_free(two);
        diminuto_buffer_free(one);
        /**/
        EXPECT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        ASSERT(diminuto_buffer_nomalloc(0));
        STATUS();
    }

    {
        size_t MYPOOL[] = { 0 };
        void * mypool[countof(MYPOOL)] = { (void *)0 };
        void * one;
        /**/
        ASSERT(!diminuto_buffer_set(0, (size_t *)0, (void **)0));
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        /**/
        ASSERT(!diminuto_buffer_set(0, MYPOOL, mypool));
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        /**/
        ASSERT(!diminuto_buffer_set(countof(MYPOOL), (size_t *)0, mypool));
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        /**/
        ASSERT(!diminuto_buffer_set(countof(MYPOOL), MYPOOL, (void **)0));
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        /**/
        EXPECT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        STATUS();
    }

    {
        size_t MYPOOL[] = { 0 };
        void * mypool[countof(MYPOOL)] = { (void *)0 };
        void * one;
        /**/
        EXPECT(diminuto_buffer_log() == 0);
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        EXPECT(diminuto_buffer_log() > 0);
        /**/
        ASSERT(diminuto_buffer_set(countof(MYPOOL), MYPOOL, mypool));
        EXPECT(diminuto_buffer_log() == 0);
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        EXPECT(diminuto_buffer_log() == 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        /**/
        ASSERT(!diminuto_buffer_set(0, (size_t *)0, (void **)0));
        EXPECT(diminuto_buffer_log() > 0);
        ASSERT((one = diminuto_buffer_malloc(1)) != (void *)0);
        diminuto_buffer_free(one);
        EXPECT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        STATUS();
    }

    {
        size_t MYPOOL[] = { 10, 100, 1000 };
        void * mypool[countof(MYPOOL)] = { (void *)0 };
        /**/
        ASSERT(diminuto_buffer_set(countof(MYPOOL), MYPOOL, mypool));
        {
            int ii;
            size_t requested;
            void * buffer[14][2] = { (void *)0 };
            EXPECT(diminuto_buffer_log() == 0);
            for (ii = 0, requested = 1; requested <= (1 << 13); ++ii, requested <<= 1) {
                buffer[ii][0] = diminuto_buffer_malloc(requested);
                ASSERT(buffer[ii][0] != (void *)0);
                memset(buffer[ii][0], 0xa5, requested); /* For valgrind(1). */
                buffer[ii][1] = diminuto_buffer_malloc(requested);
                ASSERT(buffer[ii][1] != (void *)0);
                memset(buffer[ii][1], 0x5a, requested); /* For valgrind(1). */
            }
            EXPECT(diminuto_buffer_log() == 0);
            for (--ii; ii >= 0; --ii) {
                diminuto_buffer_free(buffer[ii][1]);
                diminuto_buffer_free(buffer[ii][0]);
            }
            EXPECT(diminuto_buffer_log() > 0);
            diminuto_buffer_fini();
            EXPECT(diminuto_buffer_log() == 0);
        }
        /**/
        ASSERT(!diminuto_buffer_set(0, (size_t *)0, (void **)0));
        EXPECT(diminuto_buffer_log() == 0);
        diminuto_buffer_fini();
        EXPECT(diminuto_buffer_log() == 0);
        STATUS();
    }

    EXIT();
}
