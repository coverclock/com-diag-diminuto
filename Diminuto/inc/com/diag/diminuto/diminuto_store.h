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
 *
 * Uses a Red-Black tree, a kind of self-balancing binary tree, to implement
 * a key-value store in memory, a kind of associative memory. The key and value
 * can be anything that can be stored in, pointed to by, or mapped from a void
 * pointer. The unit test uses character strings for both, which is a typical
 * application.
 */

#include "com/diag/diminuto/diminuto_tree.h"
#include "com/diag/diminuto/diminuto_comparator.h"

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct DiminutoStore {
    diminuto_tree_t tree;       /* This is effectively the base class so must be first. */
    void *          key;
    void *          value;
} diminuto_store_t;

typedef int (diminuto_store_comparator_t)(const diminuto_store_t *, const diminuto_store_t *);

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

#define DIMINUTO_STORE_NULL ((diminuto_store_t *)0)

#define DIMINUTO_STORE_EMPTY DIMINUTO_STORE_NULL

/*******************************************************************************
 * CONDITIONALS
 ******************************************************************************/

static inline int diminuto_store_isnull(diminuto_store_t * nodep)
{
    return (nodep == DIMINUTO_STORE_NULL);
}

static inline int diminuto_store_isempty(diminuto_store_t ** rootp)
{
	return diminuto_tree_isempty((diminuto_tree_t **)rootp);
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

extern diminuto_store_t * diminuto_store_find(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

/*******************************************************************************
 * MUTATORS
 ******************************************************************************/

extern diminuto_store_t * diminuto_store_insert(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

extern diminuto_store_t * diminuto_store_replace(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

extern diminuto_store_t * diminuto_store_remove(diminuto_store_t * nodep);

/*******************************************************************************
 * ITERATORS
 ******************************************************************************/

static inline diminuto_store_t * diminuto_store_first(diminuto_store_t ** rootp)
{
    return (diminuto_store_t *)diminuto_tree_first((diminuto_tree_t **)rootp);
}

static inline diminuto_store_t * diminuto_store_last(diminuto_store_t ** rootp)
{
    return (diminuto_store_t *)diminuto_tree_last((diminuto_tree_t **)rootp);
}

static inline diminuto_store_t * diminuto_store_next(diminuto_store_t * nodep)
{
    return (diminuto_store_t *)diminuto_tree_next(&(nodep->tree));
}

static inline diminuto_store_t * diminuto_store_prev(diminuto_store_t * nodep)
{
    return (diminuto_store_t *)diminuto_tree_prev(&(nodep->tree));
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
    return diminuto_store_valueset(diminuto_store_keyset(nodep, key), value);
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
 * AUDITORS
 ******************************************************************************/

extern void diminuto_store_log(diminuto_store_t * nodep);

static inline diminuto_store_t *  diminuto_store_audit(diminuto_store_t ** rootp)
{
    return (diminuto_store_t *)diminuto_tree_audit((diminuto_tree_t **)rootp);
}

#endif
