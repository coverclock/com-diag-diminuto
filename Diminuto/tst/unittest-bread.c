/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Buffered Read (BREAD) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Buffered Read (BREAD) feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_bread.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../src/diminuto_bread.h"

static uint8_t datum = 0x00;

static int reader(void * state, void * buffer, size_t size)
{
    int result = 0;
    size_t * lengthp;
    size_t length;
    uint8_t * here;

    lengthp = (size_t *)state;
    length = *lengthp;

    if (length > 0) {
        here = (uint8_t *)buffer;
        while ((result < size) && (result < length)) {
            *(here++) = datum++;
            ++result;
        }
    }

    return result;
}

int main(int argc, char * argv[])
{
    int debug;

    SETLOGMASK();

    {
        diminuto_bread_t * sp;
        static const size_t SIZE = 64;
        static const size_t LENGTH = 32;

        TEST();

        sp = diminuto_bread_alloc((diminuto_bread_f *)0, (void *)0, SIZE);
        ASSERT(sp != (diminuto_bread_t *)0);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == SIZE);
        EXPECT(diminuto_bread_consumeable(sp) == 0);

        diminuto_bread_produced(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == LENGTH);
        EXPECT(diminuto_bread_consumeable(sp) == LENGTH);

        diminuto_bread_produced(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == 0);
        EXPECT(diminuto_bread_consumeable(sp) == SIZE);

        diminuto_bread_consumed(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == LENGTH);
        EXPECT(diminuto_bread_consumeable(sp) == LENGTH);

        diminuto_bread_consumed(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == SIZE);
        EXPECT(diminuto_bread_consumeable(sp) == 0);

        diminuto_bread_free(sp);

        STATUS();
    }

    {
        /*
         * This unit test exercises the unit test framework, not the feature.
         */

        uint8_t buffer[1024] = { 0x00, };
        uint8_t expected;
        size_t length;
        int result;
        int index;

        TEST();

        datum = 0x01;
        length = sizeof(buffer) / 2;
        expected = 0x01;

        result = reader(&length, buffer, sizeof(buffer));
        diminuto_dump(stderr, buffer, sizeof(buffer));
        ASSERT(result == length);
        for (index = 0; index < length; ++index) {
            ASSERT(buffer[index] == (expected++));
        }
        for (index = length; index < sizeof(buffer); ++index) {
            ASSERT(buffer[index] == 0x00);
        }

        result = reader(&length, buffer, sizeof(buffer));
        diminuto_dump(stderr, buffer, sizeof(buffer));
        ASSERT(result == length);
        for (index = 0; index < length; ++index) {
            ASSERT(buffer[index] == (expected++));
        }
        for (index = length; index < sizeof(buffer); ++index) {
            ASSERT(buffer[index] == 0x00);
        }

        STATUS();
    }

    {
        diminuto_bread_t bread;
        diminuto_bread_t * sp;
        size_t length;
        size_t size;
        uint8_t feature[64];
        uint8_t caller[64];
        int result;
        int index;
        uint8_t expected;

        TEST();

        sp = diminuto_bread_init(&bread, reader, &length, feature, sizeof(feature));
        ASSERT(sp != (diminuto_bread_t *)0);

        datum = 0x00;
        expected = 0x00;

        for (length = 1; length <= sizeof(feature); length += 1) {
            for (size = 1; size <= sizeof(caller); size += 1) {
                result = diminuto_bread_read(&bread, &caller, size);
                ASSERT(result > 0);
                ASSERT(result <= size);
                for (index = 0; index < size; ++index) {
                    if (caller[index] != expected) {
                        COMMENT("caller[%d]=0x%x expected=0x%x\n", index, caller[index], expected);
                        diminuto_dump(stderr, caller, sizeof(caller));
                        diminuto_bread_dump(&bread);
                        diminuto_dump(stderr, feature, sizeof(feature));
                        ASSERT(0);
                    }
                    expected += 1;
                }
            }
            for (size = sizeof(caller); size > 0; size -= 1) {
                result = diminuto_bread_read(&bread, &caller, size);
                ASSERT(result > 0);
                ASSERT(result <= size);
                for (index = 0; index < size; ++index) {
                    if (caller[index] != expected) {
                        COMMENT("caller[%d]=0x%x expected=0x%x\n", index, caller[index], expected);
                        diminuto_dump(stderr, caller, sizeof(caller));
                        diminuto_bread_dump(&bread);
                        diminuto_dump(stderr, feature, sizeof(feature));
                        ASSERT(0);
                    }
                    expected += 1;
                }
            }
        }

        for (length = sizeof(feature); length > 0; length -= 1) {
            for (size = 1; size < sizeof(caller); size += 1) {
                result = diminuto_bread_read(&bread, &caller, size);
                ASSERT(result > 0);
                ASSERT(result <= size);
                for (index = 0; index < size; ++index) {
                    if (caller[index] != expected) {
                        COMMENT("caller[%d]=0x%x expected=0x%x\n", index, caller[index], expected);
                        diminuto_dump(stderr, caller, sizeof(caller));
                        diminuto_bread_dump(&bread);
                        diminuto_dump(stderr, feature, sizeof(feature));
                        ASSERT(0);
                    }
                    expected += 1;
                }
            }
            for (size = sizeof(caller); size > 0; size -= 1) {
                result = diminuto_bread_read(&bread, &caller, size);
                ASSERT(result > 0);
                ASSERT(result <= size);
                for (index = 0; index < size; ++index) {
                    if (caller[index] != expected) {
                        COMMENT("caller[%d]=0x%x expected=0x%x\n", index, caller[index], expected);
                        diminuto_dump(stderr, caller, sizeof(caller));
                        diminuto_bread_dump(&bread);
                        diminuto_dump(stderr, feature, sizeof(feature));
                        ASSERT(0);
                    }
                    expected += 1;
                }
            }
        }

        sp = diminuto_bread_fini(&bread);
        ASSERT(sp == (diminuto_bread_t *)0);

        STATUS();
    }

    EXIT();
}
