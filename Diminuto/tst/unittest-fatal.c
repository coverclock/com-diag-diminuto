/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Fatal portion of the Core feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Fatal portion of the Core feature.
 * To succeed it must fail abnormally and produce a core file.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();
    TEST();
    ASSERT(diminuto_core_enable() >= 0);
    diminuto_core_fatal();
    FAILURE();
    STATUS();
    EXIT();
}
