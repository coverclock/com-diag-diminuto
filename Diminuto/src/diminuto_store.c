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
            return find_close(diminuto_containerof(diminuto_store_t, tree, candidatep->tree.right), targetp, comparefp, rcp);
        }
    } else if (*rcp > 0) {
        if (diminuto_tree_isleaf(candidatep->tree.left)) {
            return candidatep;
        } else {
            return find_close(diminuto_containerof(diminuto_store_t, tree, candidatep->tree.left), targetp, comparefp, rcp);
        }
    } else {
        return candidatep;
    }
}

static diminuto_store_t * insert_or_replace(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp, int replace)
{
	diminuto_store_t * storep;
    diminuto_tree_t * treep;
    int rc;

    if (diminuto_tree_isempty(rootp)) {
        treep = diminuto_tree_insert_root(&(nodep->tree), rootp);
    } else {
        storep = find_close(diminuto_containerof(diminuto_store_t, tree, *rootp), nodep, comparefp, &rc);
        if (rc < 0) {
            treep = diminuto_tree_insert_right(&(nodep->tree), &(storep->tree));
        } else if (rc > 0) {
            treep = diminuto_tree_insert_left(&(nodep->tree), &(storep->tree));
        } else if (replace) {
            treep = diminuto_tree_replace(&(storep->tree), &(nodep->tree));
        } else {
            treep = DIMINUTO_TREE_NULL;
        }
    }

    return diminuto_tree_isleaf(treep) ? DIMINUTO_STORE_NULL : diminuto_containerof(diminuto_store_t, tree, treep);
}

/*******************************************************************************
 * PUBLIC ACCESSORS
 ******************************************************************************/

diminuto_store_t * diminuto_store_find(diminuto_tree_t ** rootp, diminuto_store_t * targetp, diminuto_store_comparator_t * comparefp)
{
    diminuto_store_t * candidatep;
    int rc;

    if (diminuto_tree_isempty(rootp)) {
        return DIMINUTO_STORE_NULL;
    } else {
    	candidatep = find_close(diminuto_containerof(diminuto_store_t, tree, *rootp), targetp, comparefp, &rc);
        if (rc != 0) {
            return DIMINUTO_STORE_NULL;
        } else {
        	return candidatep;
        }
    }
}

/*******************************************************************************
 * PUBLIC MUTATORS
 ******************************************************************************/

diminuto_store_t * diminuto_store_insert(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    return insert_or_replace(rootp, nodep, comparefp, 0);
}

diminuto_store_t * diminuto_store_replace(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    return insert_or_replace(rootp, nodep, comparefp, !0);
}

diminuto_store_t * diminuto_store_remove(diminuto_store_t * nodep)
{
    diminuto_tree_t * treep;
    treep = diminuto_tree_remove(&(nodep->tree));
    return diminuto_tree_isleaf(treep) ? DIMINUTO_STORE_NULL : diminuto_containerof(diminuto_store_t, tree, treep);
}

/*******************************************************************************
 * AUDITS
 ******************************************************************************/

void diminuto_store_log(diminuto_store_t * nodep)
{
    if (nodep) {
        diminuto_tree_log(&(nodep->tree));
        DIMINUTO_LOG_DEBUG("diminuto_store_t@%p[%zu]: { key=%p value=%p }\n", nodep, sizeof(*nodep), nodep->key, nodep->value);
    } else {
    	DIMINUTO_LOG_DEBUG("diminuto_store_t@%p[%zu]\n", nodep, sizeof(*nodep));
    }
}
