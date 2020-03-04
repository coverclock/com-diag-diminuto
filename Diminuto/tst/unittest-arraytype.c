/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Compiling this translation unit will produce all kinds of warnings.
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

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef uint16_t (array_t)[128 / 8 / sizeof(uint16_t)];

static void foo(array_t * pointer) {
    fprintf(stderr, "foo(pointer)\n");
    fprintf(stderr, "%s@%d: pointer=%p\n", __FILE__, __LINE__, (void *)pointer);
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(*pointer));
    assert(sizeof(*pointer) == 16);
    fprintf(stderr, "%s@%d: zeroth=%zu\n", __FILE__, __LINE__, sizeof((*pointer)[0]));
    assert(sizeof((*pointer)[0]) == 2);
    fprintf(stderr, "%s@%d: countof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) / sizeof((*pointer)[0]));
    assert((sizeof(*pointer) / sizeof((*pointer)[0])) == 8);
    fprintf(stderr, "%s@%d: eighth=%u\n", __FILE__, __LINE__, (*pointer)[7]);
    assert((*pointer)[7] == 88);
}

static void bar(array_t variable) {
    /*
     * N.B. Produces different results on x86_64 versus ARMv7 because of the difference
     * in pointer sizes (8 versus 4).
     */
    fprintf(stderr, "bar(variable)\n");
    fprintf(stderr, "%s@%d: variable=%p\n", __FILE__, __LINE__, (void *)variable);
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(variable));
    assert(sizeof(variable) == sizeof(void *));
    fprintf(stderr, "%s@%d: zeroth=%zu\n", __FILE__, __LINE__, sizeof(variable[0]));
    assert(sizeof(variable[0]) == 2);
    fprintf(stderr, "%s@%d: countof=%zu\n", __FILE__, __LINE__, sizeof(variable) / sizeof(variable[0]));
    fprintf(stderr, "%s@%d: variable=%p\n", __FILE__, __LINE__, (void *)&variable);
    fprintf(stderr, "%s@%d: eighth=%u\n", __FILE__, __LINE__, variable[7]);
    assert(variable[7] == 88);
}

int main() {
    array_t variable = { 11, 22, 33, 44, 55, 66, 77, 88, };
    array_t * pointer = &variable;
    fprintf(stderr, "array_t\n");
    fprintf(stderr, "%s@%d: widthof=%zu\n", __FILE__, __LINE__, sizeof(array_t) * 8);
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(array_t));
    assert(sizeof(array_t) == 16);
    fprintf(stderr, "variable\n");
    fprintf(stderr, "%s@%d: variable=%p\n", __FILE__, __LINE__, (void *)variable);
    fprintf(stderr, "%s@%d: widthof=%zu\n", __FILE__, __LINE__, sizeof(variable) * 8);
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(variable));
    assert(sizeof(variable) == 16);
    fprintf(stderr, "%s@%d: zeroth=%zu\n", __FILE__, __LINE__, sizeof(variable[0]));
    assert(sizeof(variable[0]) == 2);
    fprintf(stderr, "%s@%d: countof=%zu\n", __FILE__, __LINE__, sizeof(variable) / sizeof(variable[0]));
    assert((sizeof(variable) / sizeof(variable[0])) == 8);
    fprintf(stderr, "%s@%d: eighth=%u\n", __FILE__, __LINE__, variable[7]);
    assert(variable[7] == 88);
    fprintf(stderr, "%s@%d: &variable=%p\n", __FILE__, __LINE__, (void *)&variable);
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(&variable));
    fprintf(stderr, "%s@%d: widthof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) * 8);
    fprintf(stderr, "pointer\n");
    fprintf(stderr, "%s@%d: pointer=%p\n", __FILE__, __LINE__, (void *)pointer);
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(pointer));
    fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(*pointer));
    assert(sizeof(*pointer) == 16);
    fprintf(stderr, "%s@%d: zeroth=%zu\n", __FILE__, __LINE__, sizeof((*pointer)[0]));
    assert(sizeof((*pointer)[0]) == 2);
    fprintf(stderr, "%s@%d: countof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) / sizeof((*pointer)[0]));
    assert((sizeof(*pointer) / sizeof((*pointer)[0])) == 8);
    fprintf(stderr, "%s@%d: eighth=%u\n", __FILE__, __LINE__, (*pointer)[7]);
    assert((*pointer)[7] == 88);
    foo(&variable);
    bar(variable);
    return 0;
}
