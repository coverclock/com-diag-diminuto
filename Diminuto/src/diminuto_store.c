/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Store feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Store feature.
 */

#include "com/diag/diminuto/diminuto_store.h"
#include "com/diag/diminuto/diminuto_log.h"

/*******************************************************************************
 * AUDITS
 ******************************************************************************/

void diminuto_store_log(diminuto_store_t * nodep)
{
    if (nodep) {
        diminuto_log_emit("diminuto_store_t@%p[%zu]: { key=%p value=%p }\n", nodep, sizeof(*nodep), nodep->key, nodep->value);
        diminuto_tree_log(diminuto_store_upcast(nodep));
    } else {
        diminuto_log_emit("diminuto_store_t@%p[%zu]\n", nodep, sizeof(*nodep));
    }
}
