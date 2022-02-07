/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TREE_
#define _H_COM_DIAG_DIMINUTO_TREE_

/**
 * @file
 * @copyright Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a Red-Black tree.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Tree feature implements a Red-Black Tree. This is intended to be used
 * to build more complex data structures, such as those that have
 * associative-memory-like properties. See diminuto_store and its unit test
 * for a more complete solution.
 *
 * I looked at all of the references below, but the one I found the most useful
 * was the one from the U-Boot boot loader, which is a simplified version of the
 * implementation in the Linux kernel. This implementation is inspired by the
 * U-Boot code, even though it looks nothing like it. Since I wanted this to be
 * a kind of reference implementation for my own use, I didn't make it as time
 * or space efficient as the the implementations in U-Boot or the Linux Kernel.
 *
 * REFERENCES
 *
 * Andrea Arcangeli et al., U-Boot, include/linux/rbtree.h and lib/rbtree.c
 *
 * Andrea Arcangeli et al., Linux kernel, include/linux/rbtree.h and
 * lib/rbtree.c
 *
 * Wikipedia, "Red-black tree", http://en.wikipedia.org/wiki/Red�black_tree
 *
 * Thomas H. Cormen et al., Introduction to Algorithms, MIT Press, 1992,
 * pp. 263-280
 *
 * Julienne Walker, "Red Black Trees",
 * http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx
 *
 * Free Software Foundation et al., Standard Template Library,
 * include/bits/stl_tree.h
 */

#include <stdio.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This enumerates the two possible node colors: red or black. Every node is
 * either red or black; every leaf (that is, a null pointer to a non-existent
 * node) is black; and if a node is red, then both its children are black.
 */
typedef enum DiminutoTreeColor {
    DIMINUTO_TREE_COLOR_RED     = 0,
    DIMINUTO_TREE_COLOR_BLACK   = 1,
} diminuto_tree_color_t;

/**
 * This structure describes a node in the tree. Each node has a color (red or
 * black), a parent, a left and a right pointer (in which, if both are null, the
 * node is a leaf), a pointer back to the root of the three (so two nodes in
 * the same tree will always point to the same root), and a pointer to a
 * payload (which may or may not be used, or may containing something other
 * than a pointer). This structure has a lot of wasted space. The Linux
 * implementation (from which U-Boot borrows) uses a trick I've used myself in
 * the past: hide the color bit in the otherwise unused lowest order bit of one
 * of the pointers. The down side is that you now have an invalid pointer value
 * that you have to deal with. I chose not to do that for this code since my
 * goal here is to understand Red-Black trees. But if I were porting this
 * implementation to a commercial system, I might maybe change it to be more
 * space efficient.
 */
typedef struct DiminutoTree {
    struct DiminutoTree *  parent;
    struct DiminutoTree *  left;
    struct DiminutoTree *  right;
    struct DiminutoTree ** root;
    void *                 data;
    unsigned int           color : 1;
    unsigned int           error : 1;
} diminuto_tree_t;

/**
 * This type describes the prototype of a function that compares two nodes, and
 * returns a negative number of the first node is less than the second node,
 * zero of the two nodes are equal, and a positive number if the first node is
 * greater than the second (in a logical sense, it subtracts the second node
 * from the first node). What this comparison means is completely up to the
 * implementor. Typically, the node structures would actually be part of a
 * larger structure being used by the application, or would contain data
 * pointers to application specific data that is compared. N.B. The pointers
 * in the argument list should really be const, but that turns out to be
 * remarkably difficult to do in the code that uses Diminuto Tree: Diminuto
 * Store, and the projects Assay, Codex, and Hazer, at last count. It is
 * certainly my intent that the comparator functor not modify the nodes.
 * (The correct use of the const keyword in a declaration like TYPE **
 * is also a subject of additional research on my part.)
 */
typedef int (diminuto_tree_comparator_t)(diminuto_tree_t *, diminuto_tree_t *);

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_TREE_NULL
 * This is what a null pointer in the node looks like.
 */
#define DIMINUTO_TREE_NULL ((diminuto_tree_t *)0)

/**
 * @def DIMINUTO_TREE_EMPTY
 * If a tree is empty, this is the value its root will have.
 */
#define DIMINUTO_TREE_EMPTY DIMINUTO_TREE_NULL

/**
 * @def DIMINUTO_TREE_ORPHAN
 * If a node is not on a tree, this is the value its root pointer will have.
 */
#define DIMINUTO_TREE_ORPHAN ((diminuto_tree_t **)0)

/**
 * @def DIMINUTO_TREE_DATAINIT
 * Generate a storage initializer for a node and its data pointer
 * @a _DATAP_.
 */
#define DIMINUTO_TREE_DATAINIT(_DATAP_) { DIMINUTO_TREE_NULL, DIMINUTO_TREE_NULL, DIMINUTO_TREE_NULL, DIMINUTO_TREE_ORPHAN, (_DATAP_), DIMINUTO_TREE_COLOR_RED, !!0, }

/**
 * @def DIMINUTO_TREE_NULLINIT
 * Generate a storage initializer for a node.
 */
#define DIMINUTO_TREE_NULLINIT DIMINUTO_TREE_DATAINIT((void *)0)

/*******************************************************************************
 * ITERATORS
 ******************************************************************************/

/**
 * Return the pointer to the first node on the tree (in terms of a depth first
 * left to right search), or null if the tree is empty.
 * @param rootp points to the root pointer of the tree.
 * @return a pointer to the first node on the tree or null if none.
 */
extern diminuto_tree_t * diminuto_tree_first(diminuto_tree_t ** rootp);

/**
 * Return the pointer to the last node on the tree (in terms of a depth first
 * left to right search), or null if the tree is empty.
 * @param rootp points to the root pointer of the tree.
 * @return a pointer to the last node on the tree or null if none.
 */
extern diminuto_tree_t * diminuto_tree_last(diminuto_tree_t ** rootp);

/**
 * Given a pointer to a node on the tree, return a pointer to the next node
 * on the tree (in terms of a depth first left to right search), or null if
 * the given node is the last node.
 * @param nodep points to a node on the tree.
 * @return a pointer to the next node on the tree or null if none.
 */
extern diminuto_tree_t * diminuto_tree_next(const diminuto_tree_t * nodep);

/**
 * Given a pointer to a node on the tree, return a pointer to the previous node
 * on the tree (in terms of a depth first left to right search), or null if
 * the given node is the first node.
 * @param nodep points to a node on the tree.
 * @return a pointer to the previous node on the tree or null if none.
 */
extern diminuto_tree_t * diminuto_tree_prev(const diminuto_tree_t * nodep);

/*******************************************************************************
 * MUTATORS
 ******************************************************************************/

/**
 * Insert an orphaned node either to the left of an existing node on the tree
 * (which will become the parent of the orphaned node) or as the root of the
 * tree if the tree is empty (in which case there is no parent node). There
 * must be no node to the left of the parent if the parent exists.
 * @param nodep points to the orphaned node to be inserted.
 * @param parentp points to the parent node on the tree or null if none.
 * @param rootp points to the root pointer of the tree.
 * @return a pointer to the newly inserted node or null if error.
 */
extern diminuto_tree_t * diminuto_tree_insert_left_or_root(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp);

/**
 * Insert an orphaned node either to the right of an existing node on the tree
 * (which will become the parent of the orphaned node) or as the root of the
 * tree if the tree is empty (in which case there is no parent node). There
 * must be no node to the right of the parent if the parent exists.
 * @param nodep points to the orphaned node to be inserted.
 * @param parentp points to  the parent node on the tree or null if none.
 * @param rootp points to the root pointer of the tree.
 * @return a pointer to the newly inserted node or null if error.
 */
extern diminuto_tree_t * diminuto_tree_insert_right_or_root(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp);

/**
 * Remove a node from the tree.
 * @param nodep points to the node to be removed.
 * @return a pointer to the newly orphaned node or null if error.
 */
extern diminuto_tree_t * diminuto_tree_remove(diminuto_tree_t * nodep);

/**
 * Replacing an existing (old) node on the tree with a orphaned (new) node.
 * @param oldp points to the existing node.
 * @param newp points to the orphaned node.
 * @return a pointer to the replaced node or null if error.
 */
extern diminuto_tree_t * diminuto_tree_replace(diminuto_tree_t * oldp, diminuto_tree_t * newp);

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return a pointer to the parent of a node on the tree.
 * @param nodep points to an existing node.
 * @return a pointer to the parent of the existing node.
 */
static inline diminuto_tree_t * diminuto_tree_parent(const diminuto_tree_t * nodep)
{
    return nodep->parent;
}

/**
 * Return a pointer to the left child of a node on the tree.
 * @param nodep points to an existing node.
 * @return a pointer to the left child of the existing node.
 */
static inline diminuto_tree_t * diminuto_tree_left(const diminuto_tree_t * nodep)
{
    return nodep->left;
}

/**
 * Return a pointer to the right child of a node on the tree.
 * @param nodep points to an existing node.
 * @return a pointer to the right child of the existing node.
 */
static inline diminuto_tree_t * diminuto_tree_right(const diminuto_tree_t * nodep)
{
    return nodep->right;
}

/**
 * Return a pointer to the root pointer of the tree on which an existing node
 * resides.
 * @param nodep points to an existing node.
 * @return a pointer to the root pointer of the tree of the existing node.
 */
static inline diminuto_tree_t ** diminuto_tree_root(const diminuto_tree_t * nodep)
{
    return nodep->root;
}

/**
 * Return a pointer to the data (payload) of a node on the tree.
 * @param nodep points to an existing node.
 * @return a pointer to the data (payload) of the existing node.
 */
static inline void * diminuto_tree_data(const diminuto_tree_t * nodep)
{
    return nodep->data;
}

/**
 * Return the error bit for the node. (This is not part of the public API
 * and is only exposed for unit testing).
 * @param nodep points to an existing node.
 * @return the error bit.
 */
static inline int diminuto_tree_error(const diminuto_tree_t * nodep)
{
    return nodep->error;
}

/*******************************************************************************
 * CONDITIONALS
 ******************************************************************************/

/**
 * Return true of a tree is empty.
 * @param rootp points to the root pointer of the tree.
 * @return true if the tree is empty.
 */
static inline int diminuto_tree_isempty(diminuto_tree_t ** rootp)
{
    return (*rootp == DIMINUTO_TREE_EMPTY);
}

/**
 * Return true of a node is a leaf (that is, its pointer is null).
 * @param nodep points a node, which may be null.
 * @return true if the node is a leaf.
 */
static inline int diminuto_tree_isleaf(const diminuto_tree_t * nodep)
{
    return (nodep == DIMINUTO_TREE_NULL);
}

/**
 * Return true if a node is an orphan (that is, not presently on a tree).
 * @param nodep points to a node.
 * @return true if the node is an orphan.
 */
static inline int diminuto_tree_isorphan(const diminuto_tree_t * nodep)
{
    return (diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
}

/**
 * Return true if a node is colored red. (This is not part of the public API
 * and is only exposed for unit testing).
 * @param nodep points to a node on the tree.
 * @return true if the node is colored red.
 */
static inline int diminuto_tree_isred(const diminuto_tree_t * nodep)
{
    return (nodep->color == DIMINUTO_TREE_COLOR_RED);
}

/**
 * Return true if a node is colored black. (This is not part of the public API
 * and is only exposed for unit testing).
 * @param nodep points to a node on the tree.
 * @return true if the node is colored red.
 */
static inline int diminuto_tree_isblack(const diminuto_tree_t * nodep)
{
    return (nodep->color == DIMINUTO_TREE_COLOR_BLACK);
}

/*******************************************************************************
 * SETTORS
 ******************************************************************************/

/**
 * Set the data (payload) pointer in a node. The node may be on a tree or may
 * be orphaned.
 * @param nodep points to a node.
 * @param datap is the payload to be stored in the node.
 */
static inline diminuto_tree_t * diminuto_tree_dataset(diminuto_tree_t * nodep, void * datap)
{
    nodep->data = datap;
    return nodep;
}

/**
 * Clear the error bit for the node. (This is not part of the public API
 * and is only exposed for unit testing).
 * @param nodep points to an existing node.
 * @return the prior value of the error bit.
 */
static inline int diminuto_tree_clear(diminuto_tree_t * nodep)
{
    int result;
    result = nodep->error;
    nodep->error = 0;
    return result;
}

/**
 * Set the error bit for the node. (This is not part of the public API
 * and is only exposed for unit testing).
 * @param nodep points to an existing node.
 * @return the prior value of the error bit.
 */
static inline int diminuto_tree_set(diminuto_tree_t * nodep)
{
    int result;
    result = nodep->error;
    nodep->error = !0;
    return result;
}

/*******************************************************************************
 * INITIALIZERS
 ******************************************************************************/

/**
 * Initialize a node to its orphaned state. (If you do this to a node on the
 * tree and then try to use the tree, wackiness will ensue.) The data (payload)
 * field is not modified.
 * @param nodep points to a node.
 * @return a pointer to the same node.
 */
static inline diminuto_tree_t * diminuto_tree_init(diminuto_tree_t * nodep)
{
    nodep->parent = DIMINUTO_TREE_NULL;
    nodep->left = DIMINUTO_TREE_NULL;
    nodep->right = DIMINUTO_TREE_NULL;
    nodep->root = DIMINUTO_TREE_ORPHAN;
    /* We don't initialize the data field because it doesn't belong to us. */
    nodep->color = DIMINUTO_TREE_COLOR_RED;
    nodep->error = 0;
    return nodep;
}

/**
 * Release any resources held by the tree object.
 * @param nodep points to the object.
 * @return NULL if successful, a pointer to the object otherwise.
 */
static inline diminuto_tree_t * diminuto_tree_fini(diminuto_tree_t * nodep) {
    return (diminuto_tree_t *)0;
}

/**
 * Initialize a node to its orphaned state and initialize its data (payload)
 * pointer. (If you do this to a node on the tree and then try to use the tree,
 * wackiness will ensue.)
 * @param nodep points to a node.
 * @param datap is the payload to be stored in the node.
 * @return a pointer to the same node.
 */
static inline diminuto_tree_t * diminuto_tree_datainit(diminuto_tree_t * nodep, void * datap)
{
    return diminuto_tree_dataset(diminuto_tree_init(nodep), datap);
}

/**
 * Initialize a node to its orphaned state and initialize its data (payload)
 * pointer to null. (If you do this to a node on the tree and then try to use
 * the tree, wackiness will ensue.)
 * @param nodep points to a node.
 * @return a pointer to the same node.
 */
static inline diminuto_tree_t * diminuto_tree_nullinit(diminuto_tree_t * nodep)
{
    return diminuto_tree_datainit(nodep, (void *)0);
}

/*******************************************************************************
 * SEARCH
 ******************************************************************************/

/**
 * Search the tree starting at the specified node (typically the root of the
 * tree) for a target. The target would typically not be a node on the tree
 * but a node the caller wishes to insert. The caller provides a comparison
 * function which the search uses to compare a node on the tree it is currently
 * examining with the target node; what this function actually compares is up to
 * the application. If the starting node of the search is a leaf (null), null is
 * returned; if the starting node was the root, then the tree is empty. If a
 * non-null pointer is returned, then the comparison return code must be
 * examined. If the return code is zero, then an exact match for the target node
 * was found. If the return code is negative, then the returned node is less
 * than the target node. If the return code is positive, then the returned node
 * is greater than the target node.
 * @param nodep points to a node on a tree at which to start the search.
 * @param targetp points to an orphaned node for which is searched.
 * @param comparatorp points to a comparator function.
 * @param rcp points to an variable into which the return code is stored.
 * @return a pointer to the nearest node or null if none.
 */
extern diminuto_tree_t * diminuto_tree_search(diminuto_tree_t * nodep, diminuto_tree_t * targetp, diminuto_tree_comparator_t * comparatorp, int * rcp);

/**
 * Search the tree starting at the root for a target node that is not on
 * the three. The caller provides a comparison function which the search uses to
 * compare a node on the tree it is currently examining with the target node;
 * what this function actually compares is up to the application. If an exact
 * match for the target node is found, than the returned node is replaced by
 * the target node is the replace flag is true. If an exact match is not found,
 * than the target node is inserted onto the tree appropriately. This function
 * returns null if an error occurred, a pointer to the target node if it was
 * inserted, or a pointer to an orphaned node if it was replaced.
 * @param rootp points to the root pointer of the tree.
 * @param targetp points to the target node to be inserted.
 * @param comparatorp points to a comparator function.
 * @param replace if true causes a matching node (if any) to be replaced.
 * @return a pointer to the target node, the replaced node, or null if an error.
 */
extern diminuto_tree_t * diminuto_tree_search_insert_or_replace(diminuto_tree_t ** rootp, diminuto_tree_t * targetp, diminuto_tree_comparator_t * comparatorp, int replace);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/**
 * Insert an orphaned node onto the an empty tree.
 * @param nodep points to an orphaned node.
 * @param rootp points to the root pointer of the empty tree.
 * @return a pointer to the newly inserted node or null if error.
 */
static inline diminuto_tree_t * diminuto_tree_insert_root(diminuto_tree_t * nodep, diminuto_tree_t **rootp)
{
    return diminuto_tree_insert_left_or_root(nodep, DIMINUTO_TREE_NULL, rootp);
}

/**
 * Insert an orphaned node to the left of a parent node on the tree. The parent
 * must have no child to its left.
 * @param nodep points to an orphaned node.
 * @param parentp points to the parent node on the tree.
 * @return a pointer to the newly inserted node. or null if error
 */
static inline diminuto_tree_t * diminuto_tree_insert_left(diminuto_tree_t * nodep, diminuto_tree_t * parentp)
{
    return diminuto_tree_insert_left_or_root(nodep, parentp, diminuto_tree_root(parentp));
}

/**
 * Insert an orphaned node to the right of a parent node on the tree. The parent
 * must have no child to its right.
 * @param nodep points to an orphaned node.
 * @param parentp points to the parent node on the tree.
 * @return a pointer to the newly inserted node. or null if error
 */
static inline diminuto_tree_t * diminuto_tree_insert_right(diminuto_tree_t * nodep, diminuto_tree_t * parentp)
{
    return diminuto_tree_insert_right_or_root(nodep, parentp, diminuto_tree_root(parentp));
}

/**
 * Search the tree starting at the root for a target node that is not on
 * the three. The caller provides a comparison function which the search uses to
 * compare a node on the tree it is currently examining with the target node;
 * what this function actually compares is up to the application. The target
 * node is inserted provided an exact match is not found.
 * @param rootp points to the root pointer of the tree.
 * @param targetp points to the target node to be inserted.
 * @param comparatorp points to a comparator function.
 * @return a pointer to the target node, or null if an error.
 */
static inline diminuto_tree_t * diminuto_tree_search_insert(diminuto_tree_t ** rootp, diminuto_tree_t * targetp, diminuto_tree_comparator_t * comparatorp)
{
    return diminuto_tree_search_insert_or_replace(rootp, targetp, comparatorp, 0);
}

/**
 * Search the tree starting at the root for a target node that is not on
 * the three. The caller provides a comparison function which the search uses to
 * compare a node on the tree it is currently examining with the target node;
 * what this function actually compares is up to the application. The target
 * node is inserted, or replaces an existing node if an exact match is found.
 * @param rootp points to the root pointer of the tree.
 * @param targetp points to the target node to be inserted or replacing.
 * @param comparatorp points to a comparator function.
 * @return a pointer to the target node, the replaced node, or null if an error.
 */
static inline diminuto_tree_t * diminuto_tree_search_replace(diminuto_tree_t ** rootp, diminuto_tree_t * targetp, diminuto_tree_comparator_t * comparatorp)
{
    return diminuto_tree_search_insert_or_replace(rootp, targetp, comparatorp, !0);
}

/*******************************************************************************
 * AUDITORS
 ******************************************************************************/

/**
 * Display a node (which may or may not be on a tree or may be a leaf) to the
 * log.
 * @param nodep points to a node.
 */
extern void diminuto_tree_log(const diminuto_tree_t * nodep);

/**
 * Audit a tree. The audit recursively walks the entire tree depth first, left
 * to right, checking all of the pointers, all of the colors, and also checks
 * the balance of the tree. It does not examine the data (payload) pointer.
 * It verifies the following five red-black properties as cited in
 * [Cormen et al., 3rd ed., p. 308]: "1. Every node is either red or black.
 * 2. The root is black. 3. Every leaf (NIL) is black. 4. If a node is red,
 * then both it's children are black. 5. For each node, all simple paths from
 * the node to descendant leaves contain the same number of black nodes."
 * @param rootp points to a pointer to the root of the tree.
 * @return a pointer to the first node at which an error was found or null.
 */
extern diminuto_tree_t *  diminuto_tree_audit(diminuto_tree_t ** rootp);

#endif
