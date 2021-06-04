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
#include "com/diag/diminuto/diminuto_minmaxof.h"
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

static const char TEXT[] =
    "SONNET NINETY-NINE\n"
    "Love is not all: It is not meat nor drink\n"
    "Nor slumber nor roof against the rain,\n"
    "Nor yet a floating spar to men that sink\n"
    "and rise and sink and rise and sink again.\n"
    "Love cannot fill the thickened lung with breath\n"
    "Nor cleanse the blood, nor set the fractured bone;\n"
    "Yet many a man is making friends with death\n"
    "even as I speak, for lack of love alone.\n"
    "It may well be that in a difficult hour,\n"
    "pinned down by pain and moaning for release\n"
    "or nagged by want past resolutions power,\n"
    "I might be driven to sell your love for peace,\n"
    "Or trade the memory of this night for food.\n"
    "It may well be. I do not think I would.\n"
    "-- Edna St. Vincent Milay (1892 - 1950)\n";

static int abstraction(void * state, void * buffer, size_t size)
{
    const char ** sp;
    size_t nn;

    sp = (const char **)state;
    if (*sp == (char *)0) { return 0; }
    nn = strlen(*sp);
    if (nn == 0) { *sp = (char *)0; return 0; }
    if (nn > size) { nn = size; }
    strncpy(buffer, *sp, nn);
    *sp += nn;

    return nn;
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
        EXPECT(!diminuto_bread_ready(sp));

        diminuto_bread_produced(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == LENGTH);
        EXPECT(diminuto_bread_consumeable(sp) == LENGTH);
        EXPECT(diminuto_bread_ready(sp));

        diminuto_bread_produced(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == 0);
        EXPECT(diminuto_bread_consumeable(sp) == SIZE);
        EXPECT(diminuto_bread_ready(sp));

        diminuto_bread_consumed(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == LENGTH);
        EXPECT(diminuto_bread_consumeable(sp) == LENGTH);
        EXPECT(diminuto_bread_ready(sp));

        diminuto_bread_consumed(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == SIZE);
        EXPECT(diminuto_bread_consumeable(sp) == 0);
        EXPECT(!diminuto_bread_ready(sp));

        diminuto_bread_free(sp);

        STATUS();
    }

    {
        /*
         * This unit test exercises this unit test framework, not the feature.
         */

        uint8_t buffer[1024] = { 0x00, };
        uint8_t expected;
        size_t length;
        int result;
        int index;

        TEST();

        datum = 0x01;
        expected = 0x01;
        length = sizeof(buffer) / 2;

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
        uint8_t feature[8];
        uint8_t caller[sizeof(feature)];
        int result;
        int index;
        uint8_t expected;

        TEST();

        sp = diminuto_bread_init(&bread, reader, &length, feature, sizeof(feature));
        ASSERT(sp != (diminuto_bread_t *)0);

        datum = 0x00;
        expected = 0x00;

        length = sizeof(feature);
        size = sizeof(caller);
        result = diminuto_bread_read(&bread, &caller, size);
        COMMENT("length=%zu size=%zu result=%zu\n", length, size, result);
        ASSERT(result == sizeof(caller));

        for (index = 0; index < result; ++index) {
            if (caller[index] != expected) {
                COMMENT("caller[%d]=0x%x expected=0x%x\n", index, caller[index], expected);
                diminuto_dump(stderr, caller, sizeof(caller));
                diminuto_bread_dump(&bread);
                diminuto_dump(stderr, feature, sizeof(feature));
                ASSERT(0);
            }
            expected += 1;
        }

        length = 0;
        size = sizeof(caller);
        result = diminuto_bread_read(&bread, &caller, size);
        COMMENT("length=%zu size=%zu result=%zu\n", length, size, result);
        ASSERT(result == 0);

        sp = diminuto_bread_fini(&bread);
        ASSERT(sp == (diminuto_bread_t *)0);

        STATUS();
    }

    {
        diminuto_bread_t bread;
        diminuto_bread_t * sp;
        size_t length;
        size_t size;
        uint8_t feature[8];
        uint8_t caller[8];
        int result;
        int index;
        int minimum;
        int maximum;
        uint8_t expected;

        TEST();

        sp = diminuto_bread_init(&bread, reader, &length, feature, sizeof(feature));
        ASSERT(sp != (diminuto_bread_t *)0);

        minimum = maximumof(int);
        maximum = minimumof(int);

        datum = 0x00;
        expected = 0x00;

        /*
         * length is the maximum number of octets the abstract read function
         * in this unit test may return. size of the maximum number of
         * octets the caller requests. The actual number returned by the
         * buffered read will be greater than zero but will depend on the
         * number of contiguous bytes available in the feature buffer, which
         * may not be either of these values. The only time the buffered read
         * will block the caller is if there is no data in the feature buffer.
         */

        for (length = 1; length <= sizeof(feature); length += 1) {
            for (size = 1; size <= sizeof(caller); size += 1) {
                result = diminuto_bread_read(&bread, &caller, size);
                ASSERT(result > 0);
                ASSERT(result <= size);
                if (result < minimum) { minimum = result; }
                if (result > maximum) { maximum = result; }
                for (index = 0; index < result; ++index) {
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
                if (result < minimum) { minimum = result; }
                if (result > maximum) { maximum = result; }
                for (index = 0; index < result; ++index) {
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
            for (size = 1; size <= sizeof(caller); size += 1) {
                result = diminuto_bread_read(&bread, &caller, size);
                ASSERT(result > 0);
                ASSERT(result <= size);
                if (result < minimum) { minimum = result; }
                if (result > maximum) { maximum = result; }
                for (index = 0; index < result; ++index) {
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
                if (result < minimum) { minimum = result; }
                if (result > maximum) { maximum = result; }
                for (index = 0; index < result; ++index) {
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

        COMMENT("minimum=%d maximum=%d\n", minimum, maximum);
        ASSERT(minimum == 1);
        ASSERT(maximum == sizeof(caller));

        sp = diminuto_bread_fini(&bread);
        ASSERT(sp == (diminuto_bread_t *)0);

        STATUS();
    }

    {
        diminuto_bread_t * sp;
        char buffer[sizeof(TEXT)];
        const char * source;
        char * sink;
        int result;

        TEST();

        source = TEXT;
        sink = buffer;

        sp = diminuto_bread_alloc(abstraction, &source, 13);
        ASSERT(sp != (diminuto_bread_t *)0);

        for (;;) {
            result = diminuto_bread_read(sp, sink, sizeof(buffer) - (sink - buffer));
            if (result == 0) { break; }
            sink += result;
        }

        fputs(buffer, stderr);

        ASSERT(strcmp(TEXT, buffer) == 0);

        diminuto_bread_free(sp);

        STATUS();
    }

    EXIT();
}
