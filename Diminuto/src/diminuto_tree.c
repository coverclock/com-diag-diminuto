/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_tree.h"

/*******************************************************************************
 * PRIVATE MUTATORS
 ******************************************************************************/

static void diminuto_tree_rotate_left(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * rightp;
    diminuto_tree_t * parentp;

    rightp = nodep->right;
    parentp = nodep->parent;

    nodep->right = rightp->left;
    if (nodep->right != DIMINUTO_TREE_NULL) {
        rightp->left->parent = nodep;
    }

    rightp->left = nodep;
    rightp->parent = parentp;

    if (parentp == DIMINUTO_TREE_NULL) {
        *rootp = rightp;
    } else if (nodep == parentp->left) {
        parentp->left = rightp;
    } else {
        parentp->right = rightp;
    }

    nodep->parent = rightp;
}

static void diminuto_tree_rotate_right(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * leftp;
    diminuto_tree_t * parentp;

    leftp = nodep->left;
    parentp = nodep->parent;

    nodep->left = leftp->right;
    if (nodep->left != DIMINUTO_TREE_NULL) {
        leftp->right->parent = nodep;
    }

    leftp->right = nodep;
    leftp->parent = parentp;

    if (parentp == DIMINUTO_TREE_NULL) {
        *rootp = leftp;
    } else if (nodep == parentp->right) {
        parentp->right = leftp;
    } else {
        parentp->left = leftp;
    }

    nodep->parent = leftp;
}

static void diminuto_tree_insert_rebalance(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * parentp;
    diminuto_tree_t * grandp;

    while (((parentp = nodep->parent) != DIMINUTO_TREE_NULL) && (parentp->color == DIMINUTO_TREE_COLOR_RED)) {

        grandp = parentp->parent;

        if (parentp == grandp->left) {
            diminuto_tree_t * unclep;

            unclep = grandp->right;
            if ((unclep != DIMINUTO_TREE_NULL) && (unclep->color == DIMINUTO_TREE_COLOR_RED)) {
                unclep->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;
                grandp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = grandp;
                continue;
            }

            if (parentp->right == nodep) {
                diminuto_tree_t * tempp;
                diminuto_tree_rotate_left(parentp, rootp);
                tempp = parentp;
                parentp = nodep;
                nodep = tempp;
            }

            parentp->color = DIMINUTO_TREE_COLOR_BLACK;
            grandp->color = DIMINUTO_TREE_COLOR_RED;
            diminuto_tree_rotate_right(grandp, rootp);

        } else if (parentp == grandp->right) {
            diminuto_tree_t * unclep;

            unclep = grandp->left;
            if ((unclep != DIMINUTO_TREE_NULL) && (unclep->color == DIMINUTO_TREE_COLOR_RED)) {
                unclep->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;
                grandp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = grandp;
                continue;
            }

            if (parentp->left == nodep) {
                diminuto_tree_t * tempp;
                diminuto_tree_rotate_right(parentp, rootp);
                tempp = parentp;
                parentp = nodep;
                nodep = tempp;
            }

            parentp->color = DIMINUTO_TREE_COLOR_BLACK;
            grandp->color = DIMINUTO_TREE_COLOR_RED;
            diminuto_tree_rotate_left(grandp, rootp);

        } else {

            /* Error: should never happen! */

        }
    }

    (*rootp)->color = DIMINUTO_TREE_COLOR_BLACK;
}

static void diminuto_tree_remove_rebalance(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * siblingp;

    while (((nodep == DIMINUTO_TREE_NULL) || (nodep->color == DIMINUTO_TREE_COLOR_BLACK)) && (nodep != *rootp)) {

        if (parentp->left == nodep) {

            siblingp = parentp->right;

            if (siblingp->color == DIMINUTO_TREE_COLOR_RED) {

                siblingp->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_RED;
                diminuto_tree_rotate_left(parentp, rootp);
                siblingp = parentp->right;

            }

            if (((siblingp->left == DIMINUTO_TREE_NULL) || (siblingp->left->color == DIMINUTO_TREE_COLOR_BLACK)) && ((siblingp->right == DIMINUTO_TREE_NULL) || (siblingp->right->color == DIMINUTO_TREE_COLOR_BLACK))) {

                siblingp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = parentp;
                parentp = nodep->parent;

            } else {

                if ((siblingp->right == DIMINUTO_TREE_NULL) || (siblingp->right->color == DIMINUTO_TREE_COLOR_RED)) {

                    if (siblingp->left != DIMINUTO_TREE_NULL) {
                        siblingp->left->color = DIMINUTO_TREE_COLOR_BLACK;
                    }

                    siblingp->color = DIMINUTO_TREE_COLOR_RED;
                    diminuto_tree_rotate_right(siblingp, rootp);
                    siblingp = parentp->right;

                }

                siblingp->color = parentp->color;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;

                if (siblingp->right != DIMINUTO_TREE_NULL) {
                    siblingp->right->color = DIMINUTO_TREE_COLOR_BLACK;
                }

                diminuto_tree_rotate_left(parentp, rootp);
                nodep = *rootp;
                break;

            }

        } else if (parentp->right == nodep) {

            siblingp = parentp->left;

            if (siblingp->color == DIMINUTO_TREE_COLOR_RED) {

                siblingp->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_RED;
                diminuto_tree_rotate_right(parentp, rootp);
                siblingp = parentp->left;

            }

            if (((siblingp->left == DIMINUTO_TREE_NULL) || (siblingp->left->color == DIMINUTO_TREE_COLOR_BLACK)) && ((siblingp->right == DIMINUTO_TREE_NULL) || (siblingp->right->color == DIMINUTO_TREE_COLOR_BLACK))) {

                siblingp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = parentp;
                parentp = nodep->parent;

            } else {

                if ((siblingp->left == DIMINUTO_TREE_NULL) || (siblingp->left->color == DIMINUTO_TREE_COLOR_BLACK)) {

                    if (siblingp->right != DIMINUTO_TREE_NULL) {
                        siblingp->right->color = DIMINUTO_TREE_COLOR_BLACK;
                    }

                    siblingp->color = DIMINUTO_TREE_COLOR_RED;
                    diminuto_tree_rotate_left(siblingp, rootp);
                    siblingp = parentp->left;

                }

                siblingp->color = parentp->color;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;

                if (siblingp->left != DIMINUTO_TREE_NULL) {
                    siblingp->left->color = DIMINUTO_TREE_COLOR_BLACK;
                }

                diminuto_tree_rotate_right(parentp, rootp);
                nodep = *rootp;
                break;

            }

        } else {

            /* Error: should never happen! */

        }
    }

    if (nodep != DIMINUTO_TREE_NULL) {
        nodep->color = DIMINUTO_TREE_COLOR_BLACK;
    }
}

static inline void diminuto_tree_link(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** linkp)
{
    nodep->color = DIMINUTO_TREE_COLOR_RED;
    nodep->parent = parentp;
    nodep->left = DIMINUTO_TREE_NULL;
    nodep->right = DIMINUTO_TREE_NULL;
    nodep->root = parentp->root;
    *linkp = nodep;
}

/*******************************************************************************
 * PUBLIC MUTATORS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_insert_left(diminuto_tree_t * nodep, diminuto_tree_t * parentp)
{
    if (nodep->root != DIMINUTO_TREE_ORPHAN) {
        nodep = DIMINUTO_TREE_NULL; /* Error: already on a tree! */
    } else if (parentp->left != DIMINUTO_TREE_NULL) {
        nodep = DIMINUTO_TREE_NULL; /* Error: parent already occupied! */
    } else {
        diminuto_tree_link(nodep, parentp, &(parentp->left));
        diminuto_tree_insert_rebalance(nodep, nodep->root);
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_insert_right(diminuto_tree_t * nodep, diminuto_tree_t * parentp)
{
    if (nodep->root != DIMINUTO_TREE_ORPHAN) {
        nodep = DIMINUTO_TREE_NULL; /* Error: already on a tree! */
    } else if (parentp->right != DIMINUTO_TREE_NULL) {
        nodep = DIMINUTO_TREE_NULL; /* Error: parent already occupied! */
    } else {
        diminuto_tree_link(nodep, parentp, &(parentp->right));
        diminuto_tree_insert_rebalance(nodep, nodep->root);
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_remove(diminuto_tree_t * nodep)
{
    diminuto_tree_t * childp;
    diminuto_tree_t * parentp;
    diminuto_tree_t ** rootp;
    int color;

    if (nodep->root == DIMINUTO_TREE_ORPHAN) {
        nodep = DIMINUTO_TREE_NULL; /* Error: not on a tree! */
    } else {

        rootp = nodep->root;

        do {

            if (nodep->left == DIMINUTO_TREE_NULL) {
                childp = nodep->right;
            } else if (nodep->right == DIMINUTO_TREE_NULL) {
                childp = nodep->left;
            } else {
                diminuto_tree_t * oldp;
                diminuto_tree_t * leftp;

                oldp = nodep;

                nodep = nodep->right;
                while ((leftp = nodep->left) != DIMINUTO_TREE_NULL) {
                    nodep = leftp;
                }
                childp = nodep->right;
                parentp = nodep->parent;
                color = nodep->color;

                if (childp != DIMINUTO_TREE_NULL) {
                    childp->parent = parentp;
                }
                if (parentp == oldp) {
                    parentp->right = childp;
                    parentp = nodep;
                } else {
                    parentp->left = childp;
                }

                nodep->color = oldp->color;
                nodep->parent = oldp->parent;
                nodep->left = oldp->left;
                nodep->right = oldp->right;
                /* Presumably root is already set appropriately. */

                if (oldp->parent == DIMINUTO_TREE_NULL) {
                    *rootp = nodep;
                } else if (oldp->parent->left == oldp) {
                    oldp->parent->left = nodep;
                } else if (oldp->parent->left == oldp) {
                    oldp->parent->right = nodep;
                } else {
                    /* Error: should never happen! */
                }

                oldp->left->parent = nodep;
                if (oldp->right != DIMINUTO_TREE_NULL) {
                    oldp->right->parent = nodep;
                }

                break;
            }

            parentp = nodep->parent;
            color = nodep->color;

            if (childp != DIMINUTO_TREE_NULL) {
                childp->parent = parentp;
            }

            if (parentp == DIMINUTO_TREE_NULL) {
                *rootp = childp;
            } else if (parentp->left == nodep) {
                parentp->left = childp;
            } else if (parentp->right == nodep) {
                parentp->right = childp;
            } else {
                /* Error: should never happen! */
            }

        } while (0);

        if (color == DIMINUTO_TREE_COLOR_BLACK) {
            diminuto_tree_remove_rebalance(childp, parentp, rootp);
        }

        nodep->root = DIMINUTO_TREE_ORPHAN;

    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_replace(diminuto_tree_t * oldp, diminuto_tree_t * newp) {
    if (oldp->root == DIMINUTO_TREE_ORPHAN) {
        newp = DIMINUTO_TREE_NULL; /* Error: old node not on a tree! */
    } else if (newp->root != DIMINUTO_TREE_ORPHAN) {
        newp = DIMINUTO_TREE_NULL; /* Error: new node already on a tree! */
    } else {
        diminuto_tree_t * parentp;

        parentp = oldp->parent;
        if (parentp == DIMINUTO_TREE_NULL) {
            *(oldp->root) = newp;
        } else if (oldp == parentp->left) {
            parentp->left = newp;
        } else if (oldp == parentp->right) {
            parentp->right = newp;
        } else {
            /* Error: should never happen! */
        }

        if (oldp->left != DIMINUTO_TREE_NULL) {
            oldp->left->parent = newp;
        }
        if (oldp->right != DIMINUTO_TREE_NULL) {
            oldp->right->parent = newp;
        }

        *newp = *oldp; /* Structure copy. */

        oldp->root = DIMINUTO_TREE_ORPHAN;

    }

    return newp;
}

/*******************************************************************************
 * PUBLIC TRAVERSERS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_next(const diminuto_tree_t * nodep)
{
    return 0;
}

diminuto_tree_t * diminuto_tree_prev(const diminuto_tree_t * nodep)
{
    return 0;
}

diminuto_tree_t * diminuto_tree_first(const diminuto_tree_t * nodep)
{
    return 0;
}

diminuto_tree_t * diminuto_tree_last(const diminuto_tree_t * nodep)
{
    return 0;
}

