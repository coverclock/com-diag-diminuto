/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_store.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

int diminuto_store_compare_strings(const diminuto_store_t * thisp, const diminuto_store_t * thatp)
{
    return strcmp((const char *)(thisp->key), (const char *)(thatp->key));
}

/*******************************************************************************
 * PRIVATE HELPERS
 ******************************************************************************/

static diminuto_store_t * find_close(diminuto_store_t * candidatep, diminuto_store_t * targetp, diminuto_store_comparator_t * comparefp, int * rcp)
{
    *rcp = (*comparefp)(candidatep, targetp);
    if (*rcp < 0) {
        if (diminuto_tree_isleaf(candidatep->tree.right)) {
            return candidatep;
        } else {
            return find_close(diminuto_store_downcast(candidatep->tree.right), targetp, comparefp, rcp);
        }
    } else if (*rcp > 0) {
        if (diminuto_tree_isleaf(candidatep->tree.left)) {
            return candidatep;
        } else {
            return find_close(diminuto_store_downcast(candidatep->tree.left), targetp, comparefp, rcp);
        }
    } else {
        return candidatep;
    }
}

static diminuto_store_t * insert_or_replace(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp, int replace)
{
	diminuto_store_t * storep;
    diminuto_tree_t * treep;
    int rc;

    if (diminuto_store_isempty(rootp)) {
        treep = diminuto_tree_insert_root(diminuto_store_upcast(nodep), (diminuto_tree_t **)rootp);
    } else {
        storep = find_close(*rootp, nodep, comparefp, &rc);
        if (rc < 0) {
            treep = diminuto_tree_insert_right(diminuto_store_upcast(nodep), diminuto_store_upcast(storep));
        } else if (rc > 0) {
            treep = diminuto_tree_insert_left(diminuto_store_upcast(nodep), diminuto_store_upcast(storep));
        } else if (replace) {
            treep = diminuto_tree_replace(&(storep->tree), &(nodep->tree));
        } else {
            treep = DIMINUTO_TREE_NULL;
        }
    }

    return diminuto_store_downcast(treep);
}

/*******************************************************************************
 * PUBLIC ACCESSORS
 ******************************************************************************/

diminuto_store_t * diminuto_store_find(diminuto_store_t ** rootp, diminuto_store_t * targetp, diminuto_store_comparator_t * comparefp)
{
    diminuto_store_t * candidatep;
    int rc;

    if (diminuto_store_isempty(rootp)) {
        return DIMINUTO_STORE_NULL;
    } else {
        candidatep = find_close(diminuto_store_downcast(*rootp), targetp, comparefp, &rc);
        return (rc == 0) ? candidatep : DIMINUTO_STORE_NULL;
    }

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
        diminuto_tree_log(diminuto_store_upcast(nodep));
        DIMINUTO_LOG_DEBUG("diminuto_store_t@%p[%zu]: { key=%p value=%p }\n", nodep, sizeof(*nodep), nodep->key, nodep->value);
    } else {
    	DIMINUTO_LOG_DEBUG("diminuto_store_t@%p[%zu]\n", nodep, sizeof(*nodep));
    }
}
