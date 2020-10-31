/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Comparator feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Comparator feature.
 */

#include "com/diag/diminuto/diminuto_comparator.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <string.h>

int diminuto_compare_strings(const void * thisp, const void * thatp)
{
    return strcmp(thisp, thatp);
}

int diminuto_compare_pointers(const void * thisp, const void * thatp)
{
    return ((intptr_t)thisp) - ((intptr_t)thatp);
}
