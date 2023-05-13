/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Assert contract feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Assert contract feature.
 * To succeed it must fail and in doing so create a core file.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include <errno.h>
#include <stdbool.h>

int main(int argc, char ** argv)
{
    SETLOGMASK();

    TEST();

    errno = EIO;
    diminuto_contract(true == false);

    STATUS();

    EXIT();
}
