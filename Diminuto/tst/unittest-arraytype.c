/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the array_t feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the array_t feature.
 *
 * Compiling this translation unit may produce all kinds of warnings.
 *
 * This unit test toys with the ability to declare arrays as a type. IT
 * WILL GENERATE WARNINGS because the compiler thinks I don't know what
 * I'm doing. The compiler may have a point.
 *
 * Originally from a program I wrote outside of Diminuto, hence it doesn't use
 * the unit test framework.
 *
 * The use of an array type can be pretty counter-intuitive. Especially, note
 * that it's semantics as a variable in main() differs from its use as a
 * parameter in a function. This is why I ended up making the IPv6 address
 * type a struct: structs can be passed by value into functions.
 *
 * The gcc compiler really does NOT like doing sizeof() on an array passed
 * as an argument to a function parameter. But the whole point of this unit
 * test is to experimentally determine exactly what the semantics of that are.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef uint16_t (array_t)[128 / 8 / sizeof(uint16_t)];

static void foo(array_t * pointer) {
    CHECKPOINT("foo(pointer)\n");
    CHECKPOINT("pointer=%p\n", __FILE__, __LINE__, (void *)pointer);
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(*pointer));
    ASSERT(sizeof(*pointer) == 16);
    CHECKPOINT("zeroth=%zu\n", __FILE__, __LINE__, sizeof((*pointer)[0]));
    ASSERT(sizeof((*pointer)[0]) == 2);
    CHECKPOINT("countof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) / sizeof((*pointer)[0]));
    ASSERT((sizeof(*pointer) / sizeof((*pointer)[0])) == 8);
    CHECKPOINT("eighth=%u\n", __FILE__, __LINE__, (*pointer)[7]);
    ASSERT((*pointer)[7] == 88);
}

static void bar(array_t variable) {
    /*
     * N.B. Produces different results on x86_64 versus ARMv7 because of the difference
     * in pointer sizes (8 versus 4).
     */
    CHECKPOINT("bar(variable)\n");
    CHECKPOINT("variable=%p\n", __FILE__, __LINE__, (void *)variable);
#if defined(PROBLEMATIC)
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(variable));
    ASSERT(sizeof(variable) == sizeof(void *));
    CHECKPOINT("zeroth=%zu\n", __FILE__, __LINE__, sizeof(variable[0]));
    ASSERT(sizeof(variable[0]) == 2);
    CHECKPOINT("countof=%zu\n", __FILE__, __LINE__, sizeof(variable) / sizeof(variable[0]));
#endif
    CHECKPOINT("variable=%p\n", __FILE__, __LINE__, (void *)&variable);
    CHECKPOINT("eighth=%u\n", __FILE__, __LINE__, variable[7]);
    ASSERT(variable[7] == 88);
}

int main() {
    array_t variable = { 11, 22, 33, 44, 55, 66, 77, 88, };
    array_t * pointer = &variable;

    SETLOGMASK();

    TEST();

    CHECKPOINT("array_t\n");
    CHECKPOINT("widthof=%zu\n", __FILE__, __LINE__, sizeof(array_t) * 8);
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(array_t));
    ASSERT(sizeof(array_t) == 16);
    CHECKPOINT("variable\n");
    CHECKPOINT("variable=%p\n", __FILE__, __LINE__, (void *)variable);
    CHECKPOINT("widthof=%zu\n", __FILE__, __LINE__, sizeof(variable) * 8);
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(variable));
    ASSERT(sizeof(variable) == 16);
    CHECKPOINT("zeroth=%zu\n", __FILE__, __LINE__, sizeof(variable[0]));
    ASSERT(sizeof(variable[0]) == 2);
    CHECKPOINT("countof=%zu\n", __FILE__, __LINE__, sizeof(variable) / sizeof(variable[0]));
    ASSERT((sizeof(variable) / sizeof(variable[0])) == 8);
    CHECKPOINT("eighth=%u\n", __FILE__, __LINE__, variable[7]);
    ASSERT(variable[7] == 88);
    CHECKPOINT("&variable=%p\n", __FILE__, __LINE__, (void *)&variable);
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(&variable));
    CHECKPOINT("widthof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) * 8);
    CHECKPOINT("pointer\n");
    CHECKPOINT("pointer=%p\n", __FILE__, __LINE__, (void *)pointer);
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(pointer));
    CHECKPOINT("sizeof=%zu\n", __FILE__, __LINE__, sizeof(*pointer));
    ASSERT(sizeof(*pointer) == 16);
    CHECKPOINT("zeroth=%zu\n", __FILE__, __LINE__, sizeof((*pointer)[0]));
    ASSERT(sizeof((*pointer)[0]) == 2);
    CHECKPOINT("countof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) / sizeof((*pointer)[0]));
    ASSERT((sizeof(*pointer) / sizeof((*pointer)[0])) == 8);
    CHECKPOINT("eighth=%u\n", __FILE__, __LINE__, (*pointer)[7]);
    ASSERT((*pointer)[7] == 88);
    foo(&variable);
    bar(variable);

    STATUS();

    EXIT();
}
