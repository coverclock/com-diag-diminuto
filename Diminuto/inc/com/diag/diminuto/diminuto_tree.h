/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TREE_
#define _H_COM_DIAG_DIMINUTO_TREE_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements a Red-Black Tree. I looked at all of the references below, but
 * the one I found the most useful was the one from the U-Boot boot loader,
 * which is a simplified version of the implementation in the Linux kernel.
 * Much of this code is derived at least indirectly (even though it may look
 * nothing like it) from the U-Boot code.
 *
 * REFERENCES
 *
 * Andrea Arcangeli et al., Linux kernel, <linux/rbtree.h> and <lib/rbtree.c>
 *
 * Andrea Arcangeli et al., U-Boot, <linux/rbtree.h> and <lib/rbtree.c>
 *
 * Free Software Foundation et al., Standard Template Library, <bits/stl_tree.h>
 *
 * Thomas H. Cormen et al., Introduction to Algorithms, MIT Press, 1992,
 * pp. 263-280
 *
 * Julienne Walker, "Red Black Trees",
 * http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx
 *
 * Wikipedia, "Red-black tree", http://en.wikipedia.org/wiki/RedÐblack_tree
 */

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef enum DiminutoTreeColor {
    DIMINUTO_TREE_COLOR_RED = 0,
    DIMINUTO_TREE_COLOR_BLACK = 1
} diminuto_tree_color_t;

typedef struct DiminutoTree {
    int                    color : 1;
    /*
     * Insert a lot of wasted space here. The Linux kernel implementation (from
     * which U-Boot borrows) uses a trick I've used myself in the past: hide
     * the color bit in the otherwise unused lowest order bit of one of the
     * pointers. The down side is that you now have an invalid pointer value
     * that you have to deal with. I chose not to do that for this code since my
     * goal here is to understand Red-Black trees. But if I were porting this
     * implementation to a commercial system, I'd probably change it to be more
     * space efficient.
     */
    struct DiminutoTree *  parent;
    struct DiminutoTree *  left;
    struct DiminutoTree *  right;
    struct DiminutoTree ** root;
    void *                 data;
} diminuto_tree_t;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * @def DIMINUTO_TREE_NULL
 * Defines the null pointer value used for those operations which may return it.
 * (If this were C++ I would have used a static const. But that turns out to
 * have remarkably inconvenient implications in C regarding const correctness.)
 */
#define DIMINUTO_TREE_NULL ((diminuto_tree_t *)0)

#define DIMINUTO_TREE_ORPHAN ((diminuto_tree_t **)0)

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_LIST_DATAINIT
 * Generate a storage initializer for the node @a _NODEP_ and data pointer
 * @a _DATAP_.
 */
#define DIMINUTO_TREE_DATAINIT(_NODEP_, _DATAP_) \
    { DIMINUTO_TREE_COLOR_RED, DIMINUTO_TREE_NULL, DIMINUTO_TREE_NULL, DIMINUTO_TREE_NULL, DIMINUTO_TREE_ORPHAN, (_DATAP_), }

/**
 * @def DIMINUTO_LIST_NULLINIT
 * Generate a storage initializer for the node @a _NODEP_.
 */
#define DIMINUTO_TREE_NULLINIT(_NODEP_) \
    DIMINUTO_TREE_DATAINIT(_NODEP_, (void *)0)

/*******************************************************************************
 * MUTATORS
 ******************************************************************************/

extern diminuto_tree_t * diminuto_tree_insert_left(diminuto_tree_t * nodep, diminuto_tree_t * parentp);

extern diminuto_tree_t * diminuto_tree_insert_right(diminuto_tree_t * nodep, diminuto_tree_t * parentp);

extern diminuto_tree_t * diminuto_tree_remove(diminuto_tree_t * nodep);

extern diminuto_tree_t * diminuto_tree_replace(diminuto_tree_t * oldp, diminuto_tree_t * newp);

/*******************************************************************************
 * TRAVERSERS
 ******************************************************************************/

extern diminuto_tree_t * diminuto_tree_next(const diminuto_tree_t * nodep);

extern diminuto_tree_t * diminuto_tree_prev(const diminuto_tree_t * nodep);

extern diminuto_tree_t * diminuto_tree_first(const diminuto_tree_t * nodep);

extern diminuto_tree_t * diminuto_tree_last(const diminuto_tree_t * nodep);

/*******************************************************************************
 * INITIALIZERS
 ******************************************************************************/

static inline diminuto_tree_t * diminuto_tree_init(diminuto_tree_t * nodep)
{
    nodep->color = DIMINUTO_TREE_COLOR_RED;
    nodep->parent = DIMINUTO_TREE_NULL;
    nodep->left = DIMINUTO_TREE_NULL;
    nodep->right = DIMINUTO_TREE_NULL;
    nodep->root = DIMINUTO_TREE_ORPHAN;
    nodep->data = (void *)0;
    return nodep;
}

static inline diminuto_tree_t * diminuto_tree_dataset(diminuto_tree_t * nodep, void * datap)
{
    nodep->data = datap;
    return nodep;
}

static inline diminuto_tree_t * diminuto_tree_datainit(diminuto_tree_t * nodep, void * datap)
{
    return diminuto_tree_dataset(diminuto_tree_init(nodep), datap);
}

static inline diminuto_tree_t * diminuto_tree_nullinit(diminuto_tree_t * nodep)
{
    return diminuto_tree_datainit(nodep, (void *)0);
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

static inline diminuto_tree_t ** diminuto_tree_root(const diminuto_tree_t * nodep) {
	return nodep->root;
}

static inline void * diminuto_tree_data(const diminuto_tree_t * nodep) {
	return nodep->data;
}

#endif
