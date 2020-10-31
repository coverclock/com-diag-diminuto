/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016-2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Unit Test feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Unit Test feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"

int diminuto_unittest_test = -1;
int diminuto_unittest_tests = 0;
int diminuto_unittest_errors = 0;
int diminuto_unittest_total = 0;
int diminuto_unittest_errno = 0;
