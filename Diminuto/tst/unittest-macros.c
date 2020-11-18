/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the unit test of the code generator macros.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test of the code generator macros.
 *
 * Try the command
 *
 * cpp -I inc < tst/unittest-macros.c
 *
 * just to see what the generated code looks like. The
 * unit test macros busy up the output; the macros under
 * test here just generate one line of code... although it
 * may be a very long line.
 *
 * See the comments in the macro header file for more information
 * about this C preprocessor madness.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_macros.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <stdio.h>

static int result(int arg)
{
    COMMENT("result=%d\n", arg);
    return arg;
}

static int total = 0;

static void square(int arg)
{
    int squared;

    squared = arg * arg;
    COMMENT("%d^2=%d\n", arg, squared);
    total += squared;
}

static void display(const char * arg)
{
    COMMENT("string=\"%s\"\n", arg);
}

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(CONCATENATE(5, 7)  == 57);
        ASSERT(TAKE_FIRST(2, 3, 5, 7) == 2);
        ASSERT(TAKE_SECOND(2, 3, 5, 7) == 3);

        STATUS();
    }

    {
        TEST();

        ASSERT(MAKE_BOOLEAN(0) == 0);
        ASSERT(MAKE_BOOLEAN(1) == 1);
        ASSERT(MAKE_BOOLEAN(123) == 1);
        ASSERT(MAKE_BOOLEAN(anything) == 1);

        STATUS();
    }

    {
        TEST();

        ASSERT(LOGICAL_NOT(0) == 1);
        ASSERT(LOGICAL_NOT(1) == 0);
        ASSERT(LOGICAL_NOT(123) == 0);
        ASSERT(LOGICAL_NOT(anything) == 0);

        STATUS();
    }

    {
        TEST();

        ASSERT((IF_ELSE(1)(123)(456)) == 123);
        ASSERT((IF_ELSE(0)(123)(456)) == 456);

        ASSERT((IF_ELSE(1)(result(123))(result(456))) == 123);
        ASSERT((IF_ELSE(0)(result(123))(result(456))) == 456);

        STATUS();
    }

    {
        TEST();

        ASSERT(HAS_ARGUMENTS(a, b, c) == 1);
        ASSERT(HAS_ARGUMENTS() == 0);

        STATUS();
    }

    {
        TEST();

#define SQUARE(_NUMBER_) square(_NUMBER_);

        total = 0;
        FOREACH(APPLY(SQUARE, 2, 3, 5, 7, 9))
        ASSERT(total == 168);

        STATUS();
    }

    {
        TEST();
        const char B[]="BB";
        const char * D = "DDDD";

#define DISPLAY(_STRING_) display(_STRING_);

        FOREACH(APPLY(DISPLAY, "A", B, "CCC", D))

        STATUS();
    }

    {
        size_t total = 0;
        uint8_t eight = 0x12U;
        uint16_t sixteen = 0x3456U;
        uint32_t thirtytwo = 0x789abcdeUL;
        uint64_t sixtyfour = 0xf0123456789abcdeULL;

        TEST();

#define DUMP(_OBJECT_) \
    { \
        void * pointer; \
        size_t size; \
        pointer = &_OBJECT_; \
        size = sizeof(_OBJECT_); \
        total += size; \
        fprintf(stderr, "%s:\n", #_OBJECT_); \
        diminuto_dump(stderr, pointer, size); \
    }

        FOREACH(APPLY(DUMP, eight, sixteen, thirtytwo, sixtyfour))

        ASSERT(total == 15);

        STATUS();
    }

    EXIT();

}
