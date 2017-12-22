/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_store.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>

/*******************************************************************************
 * PRIVATE HELPERS
 ******************************************************************************/

static diminuto_store_t * find_close(diminuto_store_t * candidatep, diminuto_store_t * targetp, diminuto_store_comparator_t * comparefp, int * rcp)
{
    return diminuto_store_downcast(diminuto_tree_search(diminuto_store_upcast(candidatep), diminuto_store_upcast(targetp), comparefp, rcp));
}

static diminuto_store_t * insert_or_replace(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp, int replace)
{
    return diminuto_store_downcast(diminuto_tree_search_insert_or_replace(diminuto_store_rootcast(rootp), diminuto_store_upcast(nodep), comparefp, replace));
}

/*******************************************************************************
 * PUBLIC ACCESSORS
 ******************************************************************************/

diminuto_store_t * diminuto_store_find(diminuto_store_t ** rootp, diminuto_store_t * targetp, diminuto_store_comparator_t * comparefp)
{
    diminuto_store_t * candidatep;
    int rc = 0;

    candidatep = find_close(*rootp, targetp, comparefp, &rc);

    return (rc == 0) ? candidatep : DIMINUTO_STORE_NULL;
}

/*******************************************************************************
 * PUBLIC MUTATORS
 ******************************************************************************/

diminuto_store_t * diminuto_store_insert(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    return insert_or_replace(rootp, nodep, comparefp, 0);
}

diminuto_store_t * diminuto_store_replace(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    return insert_or_replace(rootp, nodep, comparefp, !0);
}

diminuto_store_t * diminuto_store_remove(diminuto_store_t * nodep)
{
    return diminuto_store_downcast(diminuto_tree_remove(diminuto_store_upcast(nodep)));
}

/*******************************************************************************
 * AUDITS
 ******************************************************************************/

void diminuto_store_log(diminuto_store_t * nodep)
{
    if (nodep) {
        DIMINUTO_LOG_DEBUG("diminuto_store_t@%p[%zu]: { key=%p value=%p }\n", nodep, sizeof(*nodep), nodep->key, nodep->value);
        diminuto_tree_log(diminuto_store_upcast(nodep));
    } else {
    	DIMINUTO_LOG_DEBUG("diminuto_store_t@%p[%zu]\n", nodep, sizeof(*nodep));
    }
}
