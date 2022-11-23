/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Is A Constant feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Is A Constant feature.
 *
 * REFERENCE
 *
 * Fe'lix Cloutier (fay59), "Quirks of C",
 * https://gist.github.com/fay59/5ccbe684e6e56a7df8815c3486568f01
 * (from Martin Uecker via the Linux Kernel mailing list)
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_isaconstant.h"

int main(void)
{
    SETLOGMASK();

    {
        int variable = 4;
        static const int fixed = 4;
        /* On some platforms, sizeof(int) == sizeof(long) or even sizeof(short) or sizeof(char) e.g. microcontrollers. */
        long longvariable = 4;
        short shortvariable = 4;
        char tinyvariable = 4;
        /* float and double cannot be converted to a pointer type so will not compile. */
#       define CONSTANT 4
#       define expression(_ARGUMENT_) sizeof(*(1?((void*)((_ARGUMENT_)*0l)):(int *)1))

        TEST();

        CHECKPOINT("sizeof(int)=%zu\n", sizeof(int));
        CHECKPOINT("sizeof(variable)=%zu\n", sizeof(variable));
        CHECKPOINT("expression(variable)=%zu\n", expression(variable));
        CHECKPOINT("sizeof(fixed)=%zu\n", sizeof(fixed));
        CHECKPOINT("expression(fixed)=%zu\n", expression(fixed));
        CHECKPOINT("sizeof(longvariable)=%zu\n", sizeof(longvariable));
        CHECKPOINT("expression(longvariable)=%zu\n", expression(longvariable));
        CHECKPOINT("sizeof(shortvariable)=%zu\n", sizeof(shortvariable));
        CHECKPOINT("expression(shortvariable)=%zu\n", expression(shortvariable));
        CHECKPOINT("sizeof(tinyvariable)=%zu\n", sizeof(tinyvariable));
        CHECKPOINT("expression(tinyvariable)=%zu\n", expression(tinyvariable));
        CHECKPOINT("sizeof(4)=%zu\n", sizeof(4));
        CHECKPOINT("expression(4)=%zu\n", expression(4));
        CHECKPOINT("sizeof(-4)=%zu\n", sizeof(-4));
        CHECKPOINT("expression(-4)=%zu\n", expression(-4));
        CHECKPOINT("sizeof(0)=%zu\n", sizeof(0));
        CHECKPOINT("expression(0)=%zu\n", expression(0));
        CHECKPOINT("sizeof(CONSTANT)=%zu\n", sizeof(CONSTANT));
        CHECKPOINT("expression(CONSTANT)=%zu\n", expression(CONSTANT));
        CHECKPOINT("sizeof(4L)=%zu\n", sizeof(4L));
        CHECKPOINT("expression(4L)=%zu\n", expression(4L));
        CHECKPOINT("sizeof(4LL)=%zu\n", sizeof(4LL));
        CHECKPOINT("expression(4LL)=%zu\n", expression(4LL));

        ASSERT(!isaconstant(variable));
        ASSERT(!isaconstant(fixed));
        ADVISE(!isaconstant(longvariable));
        ADVISE(!isaconstant(shortvariable));
        ADVISE(!isaconstant(tinyvariable));
        ASSERT(isaconstant(4));
        ASSERT(isaconstant(-4));
        ASSERT(isaconstant(0));
        ASSERT(isaconstant(CONSTANT));
        ASSERT(isaconstant(4L));
        ASSERT(isaconstant(4LL));

        STATUS();
    }

    {
        int is_a_constant = isaconstant(4);
        int is_not_a_constant = isaconstant(is_a_constant);

        TEST();

        ASSERT(is_a_constant);
        ASSERT(!is_not_a_constant);

        STATUS();
    }

    EXIT();
}
