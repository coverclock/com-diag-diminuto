/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_STORE_
#define _H_COM_DIAG_DIMINUTO_STORE_

/**
 * @file
 *
 * Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Store feature uses a Red-Black tree, a kind of self-balancing binary
 * tree, to implement a key-value store in memory, a kind of associative
 * memory. The key and value can be anything that can be stored in, pointed
 * to by, or mapped from, a void pointer. The unit test uses character
 * strings for both, which is a typical application. This was obviously
 * inspired by the std::map<> container in the C++ Standard Template Library
 * (STL). From an implementation point of view, it is useful to think of the
 * Store structure as a class deriving from the Tree structure base class.
 */

#include "com/diag/diminuto/diminuto_tree.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_comparator.h"
#include <string.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This structure describes a node in the store. The tree is the exclusive
 * domain of this implementation; the key and value are for the use of the
 * application (or of yet another derived class) as it sees fit.
 */
typedef struct DiminutoStore {
    diminuto_tree_t tree;
    void *          key;
    void *          value;
} diminuto_store_t;

/**
 * Like the function strcmp(3), the comparator returns <0, 0, or >0 if the first
 * node is less than, equal to, or greater than, the second node (whatever that
 * means in the context of the application).
 */
typedef diminuto_tree_comparator_t diminuto_store_comparator_t;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * This is the value returned of a node does not exist in the store.
 */
#define DIMINUTO_STORE_NULL ((diminuto_store_t *)0)

/**
 * This is the value in the root pointer if the store is empty.
 */
#define DIMINUTO_STORE_EMPTY DIMINUTO_STORE_NULL

/*******************************************************************************
 * CASTS
 ******************************************************************************/

/**
 * Cast a pointer to a root store pointer to a pointer to the tree store
 * pointer. (This is part of the private API.)
 * @param pointer is a pointer to a root store pointer.
 * @return a poiinter to a root tree pointer.
 */
static inline diminuto_tree_t ** diminuto_store_rootcast(diminuto_store_t ** pointer)
{
    return (diminuto_tree_t **)pointer;
}

/*******************************************************************************
 * CONDITIONALS
 ******************************************************************************/

/**
 * Return true if a store node is missing (null).
 * @param nodep is a pointer to a node or null if missing.
 * @return true if the node is missing, false otherwise.
 */
static inline int diminuto_store_ismissing(diminuto_store_t * nodep)
{
    return (nodep == DIMINUTO_STORE_NULL);
}

/**
 * Return true if a store is empty.
 * @param rootp is a pointer to a pointer to the root of the store.
 * @return true if the store is empty, false otherwise.
 */
static inline int diminuto_store_isempty(diminuto_store_t ** rootp)
{
    return diminuto_tree_isempty(diminuto_store_rootcast(rootp));
}

/*******************************************************************************
 * MORE CASTS
 ******************************************************************************/

/**
 * Upcast a store node pointer to a tree node pointer. (This is part of the
 * private API).
 * @param pointer is a pointer to a store node.
 * @return a pointer to a tree node.
 */
static inline diminuto_tree_t * diminuto_store_upcast(diminuto_store_t * pointer)
{
    return diminuto_store_ismissing(pointer) ? DIMINUTO_TREE_NULL : &(pointer->tree);
}

/**
 * Downcast a tree node pointer to a store node pointer. (This is part of the
 * private API).
 * @param pointer is a pointer to a tree node.
 * @return a pointer to a store node.
 */
static inline diminuto_store_t * diminuto_store_downcast(diminuto_tree_t * pointer)
{
    return diminuto_tree_isleaf(pointer) ? DIMINUTO_STORE_NULL : diminuto_containerof(diminuto_store_t, tree, pointer);
}

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

/**
 * Compare two nodes by doing a string comparison of their key fields. This is
 * shown as an inline here as an illustrative example, but typically it should
 * be implemented outline since an inline will require that the code for this
 * function be generated in every application (as small as that code might
 * be).
 * @param thisp is a pointer to the first of two nodes being compared.
 * @param thatp is a pointer to the second of two nodes being compared.
 * @return <0, 0, or >0 if the first node is less than, equal to, or greater
 * than the second.
 */
static inline int diminuto_store_compare_strings(diminuto_tree_t * thisp, diminuto_tree_t * thatp)
{
    return strcmp((const char *)(diminuto_store_downcast(thisp)->key), (const char *)(diminuto_store_downcast(thatp)->key));
}

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_STORE_KEYVALUEINIT
 * Generate a storage initializer for a node, including its key pointer,
 * @a _KEYP_, and its value pointer, @a _VALUEP_.
 */
#define DIMINUTO_STORE_KEYVALUEINIT(_KEYP_, _VALUEP_) \
    { DIMINUTO_TREE_NULLINIT, (void *)(_KEYP_), (void *)(_VALUEP_), }

/**
 * @def DIMINUTO_STORE_NULLINIT
 * Generate a storage initializer for a node using null for both the key and
 * value pointers.
 */
#define DIMINUTO_STORE_NULLINIT \
    DIMINUTO_STORE_KEYVALUEINIT((void *)0, (void*)0)

/*******************************************************************************
 * ACCESSORS
 ******************************************************************************/

/**
 * Find a node in the store. Return a pointer to the node, or null if the node
 * is missing. The store is searching by comparing a node on the tree with a
 * node provided by the caller using a comparator function. Only as much of the
 * provided node need be initialized as it used by the comparator function (for
 * example, only the key field if that is all the comparator looks at).
 * @param rootp is a pointer to the root pointer of the store.
 * @param nodep is a pointer to the provided node.
 * @param comparefp is a pointer to the comparator function.
 * @return a pointer to the matching node in the store or null if it is missing.
 */
extern diminuto_store_t * diminuto_store_find(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

/*******************************************************************************
 * MUTATORS
 ******************************************************************************/

/**
 * Insert a new node into the store. If an existing node matches the new node
 * (according to the comparator), the new node is not inserted and an error is
 * returned.
 * @param rootp is a pointer to the root pointer of the store.
 * @param nodep points to the new node to be inserted.
 * @param comparefp is a pointer to the comparator function.
 * @return a pointer to the newly inserted node, or null if a matching node exists.
 */
extern diminuto_store_t * diminuto_store_insert(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

/**
 * Insert a new node into the store. If an existing node matches the new node
 * (according to the comparator), the new node is removed and returned and the
 * new node takes its place. Either way, the new node is inserted into the
 * store.
 * @param rootp is a pointer to the root pointer of the store.
 * @param nodep points to the new node to be inserted.
 * @param comparefp is a pointer to the comparator function.
 * @return a pointer to the removed matching node, or null if there is no match.
 */
extern diminuto_store_t * diminuto_store_replace(diminuto_store_t ** rootp, diminuto_store_t * nodep, diminuto_store_comparator_t * comparefp);

/**
 * Remove a node from the store. The node is specified by pointer; you must
 * find the node first.
 * @param nodep is a pointer to a node in the store.
 * @return a pointer to the removed node.
 */
extern diminuto_store_t * diminuto_store_remove(diminuto_store_t * nodep);

/*******************************************************************************
 * ITERATORS
 ******************************************************************************/

/**
 * Return the pointer to the first node in the store (in terms of a depth first
 * left to right search), or null if the store is empty.
 * @param rootp is the pointer to the root pointer of the store.
 * @return a pointer to the first node in the store or null if none.
 */
static inline diminuto_store_t * diminuto_store_first(diminuto_store_t ** rootp)
{
    return diminuto_store_downcast(diminuto_tree_first(diminuto_store_rootcast(rootp)));
}

/**
 * Return the pointer to the last node in the store (in terms of a depth first
 * left to right search), or null if the store is empty.
 * @param rootp is the pointer to the root pointer of the store.
 * @return a pointer to the last node in the store or null if none.
 */
static inline diminuto_store_t * diminuto_store_last(diminuto_store_t ** rootp)
{
    return diminuto_store_downcast(diminuto_tree_last(diminuto_store_rootcast(rootp)));
}

/**
 * Given a pointer to a node in the store, return a pointer to the next node
 * in the store (in terms of a depth first left to right search), or null if
 * the given node is the last node.
 * @param nodep is a pointer to a node in the store.
 * @return a pointer to the next node in the store or null if none.
 */
static inline diminuto_store_t * diminuto_store_next(diminuto_store_t * nodep)
{
    return diminuto_store_downcast(diminuto_tree_next(diminuto_store_upcast(nodep)));
}

/**
 * Given a pointer to a node in the store, return a pointer to the previous node
 * in the store (in terms of a depth first left to right search), or null if
 * the given node is the first node.
 * @param nodep is a pointer to a node in the store.
 * @return a pointer to the previous node in the store or null if none.
 */
static inline diminuto_store_t * diminuto_store_prev(diminuto_store_t * nodep)
{
    return diminuto_store_downcast(diminuto_tree_prev(diminuto_store_upcast(nodep)));
}

/*******************************************************************************
 * SETTORS
 ******************************************************************************/

/**
 * Set the key in a node.
 * @param nodep is a pointer to the node.
 * @param keyp is the pointer to which to set the key.
 * @return a pointer to the node.
 */
static inline diminuto_store_t * diminuto_store_keyset(diminuto_store_t * nodep, void * keyp)
{
    nodep->key = keyp;
    return nodep;
}

/**
 * Set the value in a node.
 * @param nodep is a pointer to the node.
 * @param valuep is the pointer to which to set the value.
 * @return a pointer to the node.
 */
static inline diminuto_store_t * diminuto_store_valueset(diminuto_store_t * nodep, void * valuep)
{
    nodep->value = valuep;
    return nodep;
}

/**
 * Set the key and value in a node.
 * @param nodep is a pointer to the node.
 * @param keyp is the pointer to which to set the key.
 * @param valuep is the pointer to which to set the value.
 * @return a pointer to the node.
 */
static inline diminuto_store_t * diminuto_store_keyvalueset(diminuto_store_t * nodep, void * keyp, void * valuep)
{
    return diminuto_store_valueset(diminuto_store_keyset(nodep, keyp), valuep);
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return the key from a node.
 * @param nodep is a pointer to the node.
 * @return the key from the node.
 */
static inline void * diminuto_store_keyget(diminuto_store_t * nodep)
{
    return nodep->key;
}

/**
 * Return the value from a node.
 * @param nodep is a pointer to the node.
 * @return the value from the node.
 */
static inline void * diminuto_store_valueget(diminuto_store_t * nodep)
{
    return nodep->value;
}

/*******************************************************************************
 * INITIALIZERS
 ******************************************************************************/

/**
 * Initialize a node to its orphaned state. (If you do this to a node in the
 * store and then try to use the store, wackiness will ensue.) The key and
 * value fields are not modified.
 * @param nodep is a pointer to a node.
 * @return a pointer to the same node.
 */
static inline diminuto_store_t * diminuto_store_init(diminuto_store_t * nodep)
{
    diminuto_tree_datainit(diminuto_store_upcast(nodep), nodep);
    /* We don't initialize the key and value fields because they doesn't belong to us. */
    return nodep;
}

/**
 * Release any resources held by the store object.
 * @param nodep points to the object.
 * @return NULL if successful, a pointer to the object otherwise.
 */
static inline diminuto_store_t * diminuto_store_fini(diminuto_store_t * nodep) {
    return (diminuto_store_t *)0;
}

/**
 * Initialize a node to its orphaned state and initialize its key and value
 * pointers. (If you do this to a node on the store and then try to use
 * the store, wackiness will ensue.)
 * @param nodep is a pointer to a node.
 * @param keyp is the key to be stored in the node.
 * @param valuep is the value to be stored in the node.
 * @return a pointer to the same node.
 */
static inline diminuto_store_t * diminuto_store_keyvalueinit(diminuto_store_t * nodep, void * keyp, void * valuep)
{
    return diminuto_store_keyvalueset(diminuto_store_init(nodep), keyp, valuep);
}

/**
 * Initialize a node to its orphaned state and initialize its key and value
 * pointers to null. (If you do this to a node on the store and then try to use
 * the store, wackiness will ensue.)
 * @param nodep is a pointer to a node.
 * @return a pointer to the same node.
 */
static inline diminuto_store_t * diminuto_store_nullinit(diminuto_store_t * nodep)
{
    return diminuto_store_keyvalueinit(nodep, (void *)0, (void *)0);
}

/*******************************************************************************
 * AUDITORS
 ******************************************************************************/

/**
 * Display a node (which may or may not be on a tree or may be a leaf) to the
 * log.
 * @param nodep is a pointer to a node.
 */
extern void diminuto_store_log(diminuto_store_t * nodep);

/**
 * Audit a store. Applies the tree audit to the underlying tree structure.
 * @return a pointer to the first node at which an error was found or null.
 */
static inline diminuto_store_t * diminuto_store_audit(diminuto_store_t ** rootp)
{
    return diminuto_store_downcast(diminuto_tree_audit(diminuto_store_rootcast(rootp)));
}

#endif
