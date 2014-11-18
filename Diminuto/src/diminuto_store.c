/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_store.h"
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

static diminuto_store_t * diminuto_store_find_close(diminuto_store_t * candidatep, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp, int * rcp)
{
    *rcp = (*comparefp)(candidatep, nodep);
    if (*rcp < 0) {
        if (diminuto_tree_isleaf(candidatep->tree.left)) {
            return candidatep;
        } else {
            return diminuto_store_find_close(diminuto_containerof(diminuto_store_t, tree, candidatep->tree.left), nodep, comparefp, rcp);
        }
    } else if (*rcp > 0) {
        if (diminuto_tree_isleaf(candidatep->tree.right)) {
            return candidatep;
        } else {
            return diminuto_store_find_close(diminuto_containerof(diminuto_store_t, tree, candidatep->tree.right), nodep, comparefp, rcp);
        }
    } else {
        return candidatep;
    }
}

static diminuto_store_t * diminuto_store_find_and_insert_or_replace(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp, int replace)
{
    diminuto_store_t * resultp = DIMINUTO_STORE_NULL;
    diminuto_store_t * candidatep;
    int rc;

    if (diminuto_tree_isempty(rootp)) {
        diminuto_tree_insert_root(&(nodep->tree), rootp);
    } else {
        candidatep = diminuto_store_find_close(diminuto_containerof(diminuto_store_t, tree, *rootp), nodep, comparefp, &rc);
        if (rc < 0) {
            diminuto_tree_insert_right(&(nodep->tree), &(candidatep->tree));
        } else if (rc > 0) {
            diminuto_tree_insert_left(&(nodep->tree), &(candidatep->tree));
        } else {
            if (replace) {
                diminuto_tree_replace(&(candidatep->tree), &(nodep->tree));
            }
            resultp = candidatep;
        }
    }

    return resultp;
}

/*******************************************************************************
 * PUBLIC ACCESSORS
 ******************************************************************************/

diminuto_store_t * diminuto_store_find(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    diminuto_store_t * candidatep;
    int rc;

    if (diminuto_tree_isempty(rootp)) {
        return DIMINUTO_STORE_NULL;
    } else {
    	candidatep = diminuto_store_find_close(diminuto_containerof(diminuto_store_t, tree, *rootp), nodep, comparefp, &rc);
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

diminuto_store_t * diminuto_store_find_and_insert(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    return diminuto_store_find_and_insert_or_replace(rootp, nodep, comparefp, 0);
}

diminuto_store_t * diminuto_store_find_and_replace(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp)
{
    return diminuto_store_find_and_insert_or_replace(rootp, nodep, comparefp, !0);
}
