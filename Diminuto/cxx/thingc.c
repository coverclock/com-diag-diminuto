/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the C implementation file for the CXXCAPI test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the C implementation file for the CXXCAPI test.
 */

#include "com/diag/diminuto/diminuto_cxxcapi.h"
#include "thingc.h"

/**
 * Call the C++ thing_log function.
 * @param that points to a Thing in either C or C++.
 * @return the result of the C++ thing_log() function.
 */
CXXCINLINE int logi(Thing * that) { return thing_log(that); }

int logc(Thing * that) { return logi(thing_get(that)); }
