/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_STORE_
#define _H_COM_DIAG_DIMINUTO_STORE_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_tree.h"
#include "com/diag/diminuto/diminuto_comparator.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include <stdio.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct DiminutoStore {
    diminuto_tree_t tree;
    void *          key;
    void *          value;
} diminuto_store_t;

typedef int (diminuto_store_comparator_t)(const diminuto_store_t *, const diminuto_store_t *);

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

#define DIMINUTO_STORE_NULL ((diminuto_store_t *)0)

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

static inline int diminuto_store_isnull(diminuto_store_t * nodep)
{
    return (nodep == DIMINUTO_STORE_NULL);
}

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_STORE_KEYVALUEINIT
 * Generate a storage initializer for a node and its data pointer
 * @a _DATAP_.
 */
#define DIMINUTO_STORE_KEYVALUEINIT(_KEYP_, _VALUEP_) \
    { DIMINUTO_TREE_NULLINIT, (void *)(_KEYP_), (void *)(_VALUEP_), }

/**
 * @def DIMINUTO_STORE_NULLINIT
 * Generate a storage initializer for a node.
 */
#define DIMINUTO_STORE_NULLINIT \
    DIMINUTO_STORE_KEYVALUEINIT((void *)0, (void*)0)

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

extern int diminuto_store_compare_strings(const diminuto_store_t * thisp, const diminuto_store_t * thatp);

/*******************************************************************************
 * ACCESSORS
 ******************************************************************************/

extern diminuto_store_t * diminuto_store_find(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

/*******************************************************************************
 * MUTATORS
 ******************************************************************************/

extern diminuto_store_t * diminuto_store_insert(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

extern diminuto_store_t * diminuto_store_replace(diminuto_tree_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

/*******************************************************************************
 * ITERATORS
 ******************************************************************************/

static inline diminuto_store_t * diminuto_store_first(diminuto_tree_t ** rootp)
{
    diminuto_tree_t * treep;
    treep = diminuto_tree_first(rootp);
    return (treep != DIMINUTO_TREE_NULL) ? diminuto_containerof(diminuto_store_t, tree, treep) : DIMINUTO_STORE_NULL;
}

static inline diminuto_store_t * diminuto_store_last(diminuto_tree_t ** rootp)
{
    diminuto_tree_t * treep;
    treep = diminuto_tree_last(rootp);
    return (treep != DIMINUTO_TREE_NULL) ? diminuto_containerof(diminuto_store_t, tree, treep) : DIMINUTO_STORE_NULL;
}

static inline diminuto_store_t * diminuto_store_next(diminuto_store_t * nodep)
{
    diminuto_tree_t * treep;
    treep = diminuto_tree_next(&(nodep->tree));
    return (treep != DIMINUTO_TREE_NULL) ? diminuto_containerof(diminuto_store_t, tree, treep) : DIMINUTO_STORE_NULL;
}

static inline diminuto_store_t * diminuto_store_prev(diminuto_store_t * nodep)
{
    diminuto_tree_t * treep;
    treep = diminuto_tree_prev(&(nodep->tree));
    return (treep != DIMINUTO_TREE_NULL) ? diminuto_containerof(diminuto_store_t, tree, treep) : DIMINUTO_STORE_NULL;
}

/*******************************************************************************
 * SETTORS
 ******************************************************************************/

static inline diminuto_store_t * diminuto_store_keyset(diminuto_store_t * nodep, void * key)
{
    nodep->key = key;
    return nodep;
}

static inline diminuto_store_t * diminuto_store_valueset(diminuto_store_t * nodep, void * value)
{
    nodep->value = value;
    return nodep;
}

static inline diminuto_store_t * diminuto_store_keyvalueset(diminuto_store_t * nodep, void * key, void * value)
{
    nodep->key = key;
    nodep->value = value;
    return nodep;
}

/*******************************************************************************
 * INITIALIZERS
 ******************************************************************************/

static inline diminuto_store_t * diminuto_store_init(diminuto_store_t * nodep)
{
    diminuto_tree_datainit(&(nodep->tree), nodep);
    /* We don't initialize the key and value fields because they doesn't belong to us. */
    return nodep;
}

static inline diminuto_store_t * diminuto_store_datainit(diminuto_store_t * nodep, void * keyp, void * valuep)
{
    return diminuto_store_keyvalueset(diminuto_store_init(nodep), keyp, valuep);
}

static inline diminuto_store_t * diminuto_store_nullinit(diminuto_store_t * nodep)
{
    return diminuto_store_datainit(nodep, (void *)0, (void *)0);
}

/*******************************************************************************
 * AUDITS
 ******************************************************************************/

extern void diminuto_store_print(FILE * fp, diminuto_store_t * nodep);

#endif
