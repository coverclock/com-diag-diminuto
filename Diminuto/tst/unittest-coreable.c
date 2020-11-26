/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Coreable feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Coreable feature.
 * To succeed it must fail and in doing so create a core file.
 */

#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();
    TEST();
    ASSERT(diminuto_core_enable() >= 0);
    *((volatile int *)0) = 0;
    FAILURE();
    EXIT();
}
