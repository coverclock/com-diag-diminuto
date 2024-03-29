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

typedef struct Thing2 {
    thing_t * thing;
} thing2_t;

static thing2_t * thing2_t_ctor(thing2_t * that)
{
    that->thing = diminuto_new(thing_t, 0xa5a5a5a5, 0.125, '*');
    return that;
}

static thing2_t * thing2_t_dtor(thing2_t * that)
{
    diminuto_delete(thing_t, that->thing);
    return that;
}

typedef thing_t thing3_t;

static thing3_t * thing3_t_ctor(thing3_t * that)
{
    return (thing3_t *)diminuto_ctor(thing_t, (thing_t *)that, 0xa5a5a5a5, 0.125, '*');
}

static thing3_t * thing3_t_dtor(thing3_t * that)
{
    return diminuto_dtor(thing_t, (thing_t *)that);
}

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        thing_t thing;
        thing_t * thingp;

        TEST();

        thingp = diminuto_ctor(thing_t, &thing, 0xc0edbabe, 0.25, '?');
        ASSERT(thingp == &thing);
        ASSERT(thingp->integer == 0xc0edbabe);
        ASSERT(thingp->floatingpoint == 0.25);
        ASSERT(thingp->character == '?');
        ASSERT(thingp->pointer != (void *)0);
        diminuto_dtor(thing_t, thingp);

        STATUS();
    }

    {
        thing_t * thingp;

        TEST();

        thingp = diminuto_new(thing_t, 0xdeadbeef, 0.5, '!');
        ASSERT(thingp != (thing_t *)0);
        ASSERT(thingp->integer == 0xdeadbeef);
        ASSERT(thingp->floatingpoint == 0.5);
        ASSERT(thingp->character == '!');
        ASSERT(thingp->pointer != (void *)0);
        diminuto_delete(thing_t, thingp);

        STATUS();
    }

    {
        int integer = 0x5a5a5a5a;
        int * integerp;

        TEST();

        integerp = diminuto_ctor(int, &integer);
        ASSERT(integerp == &integer);
        ASSERT(*integerp == 0x5a5a5a5a);
        diminuto_dtor(int, integerp);

        STATUS();
    }

    {
        int * integerp;

        TEST();

        integerp = diminuto_new(int);
        ASSERT(integerp != (int *)0);
        ASSERT(*integerp == 0);
        diminuto_delete(int, integerp);

        STATUS();
    }

    {
        thing2_t thing2;
        thing2_t * thing2p;

        TEST();

        thing2p = diminuto_ctor(thing2_t, &thing2);
        ASSERT(thing2p == &thing2);
        ASSERT(thing2p->thing != (thing_t *)0);
        ASSERT(thing2p->thing->integer == 0xa5a5a5a5);
        ASSERT(thing2p->thing->floatingpoint == 0.125);
        ASSERT(thing2p->thing->character == '*');
        ASSERT(thing2p->thing->pointer != (void *)0);
        diminuto_dtor(thing2_t, thing2p);

        STATUS();
    }

    {
        thing2_t * thing2p;

        TEST();

        thing2p = diminuto_new(thing2_t);
        ASSERT(thing2p != (thing2_t *)0);
        ASSERT(thing2p->thing != (thing_t *)0);
        ASSERT(thing2p->thing->integer == 0xa5a5a5a5);
        ASSERT(thing2p->thing->floatingpoint == 0.125);
        ASSERT(thing2p->thing->character == '*');
        ASSERT(thing2p->thing->pointer != (void *)0);
        diminuto_delete(thing2_t, thing2p);

        STATUS();
    }

    {
        thing3_t thing3;
        thing3_t * thing3p;

        TEST();

        thing3p = diminuto_ctor(thing3_t, &thing3);
        ASSERT(thing3p == &thing3);
        ASSERT(thing3p->integer == 0xa5a5a5a5);
        ASSERT(thing3p->floatingpoint == 0.125);
        ASSERT(thing3p->character == '*');
        ASSERT(thing3p->pointer != (void *)0);
        diminuto_dtor(thing3_t, thing3p);

        STATUS();
    }

    {
        thing3_t * thing3p;

        TEST();

        thing3p = diminuto_new(thing3_t);
        ASSERT(thing3p != (thing3_t *)0);
        ASSERT(thing3p->integer == 0xa5a5a5a5);
        ASSERT(thing3p->floatingpoint == 0.125);
        ASSERT(thing3p->character == '*');
        ASSERT(thing3p->pointer != (void *)0);
        diminuto_delete(thing3_t, thing3p);

        STATUS();
    }

    EXIT();
}
