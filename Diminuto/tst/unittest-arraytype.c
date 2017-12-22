/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Originally from a program I wrote outside of Diminuto, hence it doesn't use
 * the unit test framework.
 *
 * The use of an array type can be pretty counter-intuitive. Especially, note
 * that it's semantics as a variable in main() differs from its use as a
 * parameter in a function. This is why I ended up making the IPv6 address
 * type a struct: structs can be passed by value into functions.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef uint16_t (array_t)[128 / 8 / sizeof(uint16_t)];

static void foo(array_t * pointer) {
	fprintf(stderr, "foo(pointer)\n");
	fprintf(stderr, "%s@%d: pointer=%p\n", __FILE__, __LINE__, pointer);
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
	fprintf(stderr, "bar(variable)\n");
	fprintf(stderr, "%s@%d: variable=%p\n", __FILE__, __LINE__, variable);
	fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(variable));
	assert(sizeof(variable) == sizeof(void *)); /* Because it's a pointer. */
	fprintf(stderr, "%s@%d: zeroth=%zu\n", __FILE__, __LINE__, sizeof(variable[0]));
	assert(sizeof(variable[0]) == 2);
	fprintf(stderr, "%s@%d: countof=%zu\n", __FILE__, __LINE__, sizeof(variable) / sizeof(variable[0]));
	assert((sizeof(variable) / sizeof(variable[0])) == 4); /* Because it's a pointer. */
	fprintf(stderr, "%s@%d: variable=%p\n", __FILE__, __LINE__, &variable);
	fprintf(stderr, "%s@%d: eighth=%u\n", __FILE__, __LINE__, variable[7]);
	assert(variable[7] == 88);
}

void main() {
	array_t variable = { 11, 22, 33, 44, 55, 66, 77, 88, };
	array_t * pointer = &variable;
	fprintf(stderr, "array_t\n");
	fprintf(stderr, "%s@%d: widthof=%zu\n", __FILE__, __LINE__, sizeof(array_t) * 8);
	fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(array_t));
	assert(sizeof(array_t) == 16);
	fprintf(stderr, "variable\n");
	fprintf(stderr, "%s@%d: variable=%p\n", __FILE__, __LINE__, variable);
	fprintf(stderr, "%s@%d: widthof=%zu\n", __FILE__, __LINE__, sizeof(variable) * 8);
	fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(variable));
	assert(sizeof(variable) == 16);
	fprintf(stderr, "%s@%d: zeroth=%zu\n", __FILE__, __LINE__, sizeof(variable[0]));
	assert(sizeof(variable[0]) == 2);
	fprintf(stderr, "%s@%d: countof=%zu\n", __FILE__, __LINE__, sizeof(variable) / sizeof(variable[0]));
	assert((sizeof(variable) / sizeof(variable[0])) == 8);
	fprintf(stderr, "%s@%d: eighth=%u\n", __FILE__, __LINE__, variable[7]);
	assert(variable[7] == 88);
	fprintf(stderr, "%s@%d: &variable=%p\n", __FILE__, __LINE__, &variable);
	fprintf(stderr, "%s@%d: sizeof=%zu\n", __FILE__, __LINE__, sizeof(&variable));
	fprintf(stderr, "%s@%d: widthof=%zu\n", __FILE__, __LINE__, sizeof(*pointer) * 8);
	fprintf(stderr, "pointer\n");
	fprintf(stderr, "%s@%d: pointer=%p\n", __FILE__, __LINE__, pointer);
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
}
