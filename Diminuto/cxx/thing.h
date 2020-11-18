/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _COM_DIAG_DIMINUTO_CXX_THING_H_
#define _COM_DIAG_DIMINUTO_CXX_THING_H_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the C header file for the CXXCAPI test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the C header file for the CXXCAPI test.
 */

#include "Thing.hxx"
#include "com/diag/diminuto/diminuto_cxxcapi.h"

/**
 * Call the C++ thing_get() fuction passing it a pointer
 * to a C or C++ Thing.
 * @param that points to a thing in either C or C++.
 * @return the result of calling C++ thing_get().
 */
CXXCAPI int logc(Thing * that);

#endif
