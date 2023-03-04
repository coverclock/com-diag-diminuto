/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the New (as in New and Delete) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the New (as in New and Delete) feature.
 * Best if tested using valgrind(1).
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_new.h"
#include <stdint.h>

typedef struct Thing {
    int integer;
    float floatingpoint;
    char character;
    void * pointer;
} thing_t;

static thing_t * thing_t_ctor(thing_t * that, int ii, float ff, char cc)
{
    that->integer = ii;
    that->floatingpoint = ff;
    that->character = cc;
    that->pointer = malloc(8);

    return that;
}

static thing_t * thing_t_dtor(thing_t * that)
{
    free(that->pointer);

    return that;
}

static inline int * int_ctor(int * that) { return that; }

static inline int * int_dtor(int * that) { return that; }

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        thing_t thing;
        thing_t * thingp;

        thingp = diminuto_ctor(thing_t, &thing, 0xc0edbabe, 0.25, '?');
        ASSERT(thingp == &thing);
        ASSERT(thingp->integer == 0xc0edbabe);
        ASSERT(thingp->floatingpoint == 0.25);
        ASSERT(thingp->character == '?');
        ASSERT(thingp->pointer != (void *)0);
        diminuto_dtor(thing_t, thingp);
    }

    {
        thing_t * thingp;

        thingp = diminuto_new(thing_t, 0xdeadbeef, 0.5, '!');
        ASSERT(thingp != (thing_t *)0);
        ASSERT(thingp->integer == 0xdeadbeef);
        ASSERT(thingp->floatingpoint == 0.5);
        ASSERT(thingp->character == '!');
        ASSERT(thingp->pointer != (void *)0);
        diminuto_delete(thing_t, thingp);
    }

    {
        int integer = 0x5a5a5a5a;
        int * integerp;

        integerp = diminuto_ctor(int, &integer);
        ASSERT(integerp == &integer);
        ASSERT(*integerp == 0x5a5a5a5a);
        diminuto_dtor(int, integerp);
    }

    {
        int * integerp;

        integerp = diminuto_new(int);
        ASSERT(integerp != (int *)0);
        ASSERT(*integerp == 0);
        diminuto_delete(int, integerp);
    }

    EXIT();
}
