/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Module functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the Module functional test.
 */

#include "com/diag/diminuto/diminuto_module.h"
#include <stdio.h>

DIMINUTO_MODULE_CTOR(diminuto_module_example_ctor);
DIMINUTO_MODULE_DTOR(diminuto_module_example_dtor);

int diminuto_module_example_ctor(void)
{
    fputs("diminuto_module_example_ctor\n", stderr);
    return 0;
}

int diminuto_module_example_dtor(void)
{
    fputs("diminuto_module_example_dtor\n", stderr);
    return 0;
}

int diminuto_module_example_function(int input) {
    return ~input;
}

int diminuto_module_example_variable = 0xc0edbabe;
