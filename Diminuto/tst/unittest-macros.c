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
 * See the comments in the macro header file for more information
 * about this C preprocessor madness.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_macros.h"

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
        ASSERT(FIRST(2, 3, 5, 7) == 2);
        ASSERT(SECOND(2, 3, 5, 7) == 3);

        STATUS();
    }

    {
        TEST();

        ASSERT(BOOL(0) == 0);
        ASSERT(BOOL(1) == 1);
        ASSERT(BOOL(123) == 1);
        ASSERT(BOOL(anything) == 1);

        STATUS();
    }

    {
        TEST();

        ASSERT(NOT(0) == 1);
        ASSERT(NOT(1) == 0);
        ASSERT(NOT(123) == 0);
        ASSERT(NOT(anything) == 0);

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

#define SQUARE(_X_) square(_X_);

        total = 0;
        FORALL(APPLY(SQUARE, 2, 3, 5, 7, 9))
        ASSERT(total == 168);

        STATUS();
    }

    {
        TEST();
        const char B[]="BB";
        const char * D = "DDDD";

#define DISPLAY(_STRING_) display(_STRING_);

        FORALL(APPLY(DISPLAY, "A", B, "CCC", D));

        STATUS();
    }

    EXIT();

}
