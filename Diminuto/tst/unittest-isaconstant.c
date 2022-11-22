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
        int is_a_constant = isaconstant(4);
        int is_not_a_constant = isaconstant(is_a_constant);

        TEST();

        ASSERT(is_a_constant);
        ASSERT(!is_not_a_constant);

        STATUS();
    }

    {
        int variable = 4;
        static const int constant = 4;

        TEST();

        ASSERT(isaconstant(4));
        ASSERT(!isaconstant(variable));
        ASSERT(!isaconstant(constant));

        STATUS();
    }

    EXIT();
}
