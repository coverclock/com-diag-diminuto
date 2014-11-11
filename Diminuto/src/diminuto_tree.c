/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_tree.h"

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

static void diminuto_tree_insert_color(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
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

            /* Do nothing: should never happen. */

        }
    }

    (*rootp)->color = DIMINUTO_TREE_COLOR_BLACK;
}

static void diminuto_tree_remove_color(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** rootp)
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

            /* Do nothing: should never happen. */

        }
    }

    if (nodep != DIMINUTO_TREE_NULL) {
        nodep->color = DIMINUTO_TREE_COLOR_BLACK;
    }
}

/*******************************************************************************
 * PUBLIC MUTATORS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_insert(diminuto_tree_t * nodep)
{
    return nodep;
}

diminuto_tree_t * diminuto_tree_remove(diminuto_tree_t * nodep)
{
    return nodep;
}

diminuto_tree_t * diminuto_tree_replace(diminuto_tree_t * wasp, diminuto_tree_t * nowp)
{
    return nowp;
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

