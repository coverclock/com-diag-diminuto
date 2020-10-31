/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Size Of feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Size Of feature.
 */

#include <stdint.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_unittest.h"

typedef enum Enum { ENUM = 0 } enum_t;

struct Zero {
    char field[0]; /* Produces -pedantic warning. */
};

typedef uint16_t (Array)[7];

static Array array = { 0, 0, 0, 0, 0, 0, 0, };

int main(void)
{
    SETLOGMASK();
    {
        TEST();
        EXPECT(sizeof(long long) >= sizeof(long));
        EXPECT(sizeof(long) >= sizeof(int));
        EXPECT(sizeof(int) >= sizeof(short));
        EXPECT(sizeof(short) >= sizeof(char));
        EXPECT(sizeof(double) >= sizeof(float));
        ADVISE(sizeof(double) > sizeof(float)); /* Not true on some targets. */
        STATUS();
    }
    {
        TEST();
        EXPECT(sizeof(int64_t) == 8);
        EXPECT(sizeof(uint64_t) == 8);
        EXPECT(sizeof(int32_t) == 4);
        EXPECT(sizeof(uint32_t) == 4);
        EXPECT(sizeof(int16_t) == 2);
        EXPECT(sizeof(uint16_t) == 2);
        EXPECT(sizeof(int8_t) == 1);
        EXPECT(sizeof(uint8_t) == 1);
        EXPECT(sizeof(intptr_t) >= sizeof(void *));
        ADVISE(sizeof(intptr_t) == sizeof(void *));
        EXPECT(sizeof(uintptr_t) >= sizeof(void *));
        ADVISE(sizeof(uintptr_t) == sizeof(void *));
        STATUS();
    }
    {
        TEST();
        EXPECT(sizeof(enum_t) == sizeof(int));
        EXPECT(sizeof(struct Zero) == 0);
        EXPECT(sizeof(Array) == 14);
        EXPECT(sizeof(array) == 14);
        EXPECT(sizeof(array[0]) == 2);
        EXPECT(sizeof(diminuto_ipv4_t) == (32 / 8));
        EXPECT(sizeof(diminuto_ipv6_t) == (128 / 8));
        STATUS();
    }
    EXIT();
}
