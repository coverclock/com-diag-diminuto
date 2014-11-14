/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 *
 * See the header file for references and acknowledgements.
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
    if (!diminuto_tree_isleaf(nodep->right)) {
        rightp->left->parent = nodep;
    }

    rightp->left = nodep;
    rightp->parent = parentp;

    if (diminuto_tree_isleaf(parentp)) {
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
    if (!diminuto_tree_isleaf(nodep->left)) {
        leftp->right->parent = nodep;
    }

    leftp->right = nodep;
    leftp->parent = parentp;

    if (diminuto_tree_isleaf(parentp)) {
        *rootp = leftp;
    } else if (nodep == parentp->right) {
        parentp->right = leftp;
    } else {
        parentp->left = leftp;
    }

    nodep->parent = leftp;
}

static void diminuto_tree_insert(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * parentp;
    diminuto_tree_t * grandp;

    while ((!diminuto_tree_isleaf(parentp = nodep->parent)) && (parentp->color == DIMINUTO_TREE_COLOR_RED)) {

        grandp = parentp->parent;

        if (parentp == grandp->left) {
            diminuto_tree_t * unclep;

            unclep = grandp->right;
            if ((!diminuto_tree_isleaf(unclep)) && (unclep->color == DIMINUTO_TREE_COLOR_RED)) {
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
            if ((!diminuto_tree_isleaf(unclep)) && (unclep->color == DIMINUTO_TREE_COLOR_RED)) {
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

static void diminuto_tree_remove_fixup(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * siblingp;

    while ((diminuto_tree_isleaf(nodep) || (nodep->color == DIMINUTO_TREE_COLOR_BLACK)) && (nodep != *rootp)) {

        if (parentp->left == nodep) {

            siblingp = parentp->right;

            if (siblingp->color == DIMINUTO_TREE_COLOR_RED) {

                siblingp->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_RED;
                diminuto_tree_rotate_left(parentp, rootp);
                siblingp = parentp->right;

            }

            if ((diminuto_tree_isleaf(siblingp->left) || (siblingp->left->color == DIMINUTO_TREE_COLOR_BLACK)) && (diminuto_tree_isleaf(siblingp->right) || (siblingp->right->color == DIMINUTO_TREE_COLOR_BLACK))) {

                siblingp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = parentp;
                parentp = nodep->parent;

            } else {

                if (diminuto_tree_isleaf(siblingp->right) || (siblingp->right->color == DIMINUTO_TREE_COLOR_RED)) {

                    if (!diminuto_tree_isleaf(siblingp->left)) {
                        siblingp->left->color = DIMINUTO_TREE_COLOR_BLACK;
                    }

                    siblingp->color = DIMINUTO_TREE_COLOR_RED;
                    diminuto_tree_rotate_right(siblingp, rootp);
                    siblingp = parentp->right;

                }

                siblingp->color = parentp->color;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;

                if (!diminuto_tree_isleaf(siblingp->right)) {
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

            if ((diminuto_tree_isleaf(siblingp->left) || (siblingp->left->color == DIMINUTO_TREE_COLOR_BLACK)) && (diminuto_tree_isleaf(siblingp->right) || (siblingp->right->color == DIMINUTO_TREE_COLOR_BLACK))) {

                siblingp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = parentp;
                parentp = nodep->parent;

            } else {

                if (diminuto_tree_isleaf(siblingp->left) || (siblingp->left->color == DIMINUTO_TREE_COLOR_BLACK)) {

                    if (!diminuto_tree_isleaf(siblingp->right)) {
                        siblingp->right->color = DIMINUTO_TREE_COLOR_BLACK;
                    }

                    siblingp->color = DIMINUTO_TREE_COLOR_RED;
                    diminuto_tree_rotate_left(siblingp, rootp);
                    siblingp = parentp->left;

                }

                siblingp->color = parentp->color;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;

                if (!diminuto_tree_isleaf(siblingp->left)) {
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

static void diminuto_tree_link(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp, diminuto_tree_t ** linkp)
{
    nodep->color = DIMINUTO_TREE_COLOR_RED;
    nodep->parent = parentp;
    nodep->left = DIMINUTO_TREE_NULL;
    nodep->right = DIMINUTO_TREE_NULL;
    nodep->root = rootp;
    *linkp = nodep;
}

/*******************************************************************************
 * PUBLIC MUTATORS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_insert_left_or_root(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp)
{
    if (nodep->root != DIMINUTO_TREE_ORPHAN) {
       nodep = DIMINUTO_TREE_NULL; /* Error: already on a tree! */
    } else if (diminuto_tree_isleaf(parentp)) {
        if (*rootp != DIMINUTO_TREE_NULL) {
            nodep = DIMINUTO_TREE_NULL; /* Error: root already occupied! */
        } else {
            diminuto_tree_link(nodep, parentp, rootp, rootp);
            diminuto_tree_insert(nodep, rootp);
        }
    } else {
        if (!diminuto_tree_isleaf(parentp->left)) {
            nodep = DIMINUTO_TREE_NULL; /* Error: left already occupied! */
        } else {
            diminuto_tree_link(nodep, parentp, parentp->root, &(parentp->left));
            diminuto_tree_insert(nodep, nodep->root);
        }
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_insert_right_or_root(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp)
{
    if (nodep->root != DIMINUTO_TREE_ORPHAN) {
        nodep = DIMINUTO_TREE_NULL; /* Error: already on a tree! */
    } else if (diminuto_tree_isleaf(parentp)) {
        if (*rootp != DIMINUTO_TREE_NULL) {
            nodep = DIMINUTO_TREE_NULL; /* Error: root already occupied! */
        } else {
            diminuto_tree_link(nodep, parentp, rootp, rootp);
            diminuto_tree_insert(nodep, rootp);
        }
    } else {
        if (!diminuto_tree_isleaf(parentp->right)) {
            nodep = DIMINUTO_TREE_NULL; /* Error: right already occupied! */
        } else {
            diminuto_tree_link(nodep, parentp, parentp->root, &(parentp->right));
            diminuto_tree_insert(nodep, nodep->root);
        }
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

            if (diminuto_tree_isleaf(nodep->left)) {
                childp = nodep->right;
            } else if (diminuto_tree_isleaf(nodep->right)) {
                childp = nodep->left;
            } else {
                diminuto_tree_t * oldp;
                diminuto_tree_t * leftp;

                oldp = nodep;

                nodep = nodep->right;
                while (!diminuto_tree_isleaf(leftp = nodep->left)) {
                    nodep = leftp;
                }
                childp = nodep->right;
                parentp = nodep->parent;
                color = nodep->color;

                if (!diminuto_tree_isleaf(childp)) {
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

                if (diminuto_tree_isleaf(oldp->parent)) {
                    *rootp = nodep;
                } else if (oldp->parent->left == oldp) {
                    oldp->parent->left = nodep;
                } else if (oldp->parent->left == oldp) {
                    oldp->parent->right = nodep;
                } else {
                    /* Error: should never happen! */
                }

                oldp->left->parent = nodep;
                if (!diminuto_tree_isleaf(oldp->right)) {
                    oldp->right->parent = nodep;
                }

                break;
            }

            parentp = nodep->parent;
            color = nodep->color;

            if (!diminuto_tree_isleaf(childp)) {
                childp->parent = parentp;
            }

            if (diminuto_tree_isleaf(parentp)) {
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
            diminuto_tree_remove_fixup(childp, parentp, rootp);
        }

        nodep->root = DIMINUTO_TREE_ORPHAN;

    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_replace(diminuto_tree_t * oldp, diminuto_tree_t * newp) {
    if (oldp->root == DIMINUTO_TREE_ORPHAN) {
        oldp = DIMINUTO_TREE_NULL; /* Error: old node not on a tree! */
    } else if (newp->root != DIMINUTO_TREE_ORPHAN) {
        oldp = DIMINUTO_TREE_NULL; /* Error: new node already on a tree! */
    } else {
        diminuto_tree_t * parentp;

        parentp = oldp->parent;
        if (diminuto_tree_isleaf(parentp)) {
            *(oldp->root) = newp;
        } else if (oldp == parentp->left) {
            parentp->left = newp;
        } else if (oldp == parentp->right) {
            parentp->right = newp;
        } else {
            /* Error: should never happen! */
        }

        if (!diminuto_tree_isleaf(oldp->left)) {
            oldp->left->parent = newp;
        }
        if (!diminuto_tree_isleaf(oldp->right)) {
            oldp->right->parent = newp;
        }

        /*
         * We can't do a structure copy here because that would copy the
         * data pointer that pointers to the payload. We can't just swap the
         * data pointers because the structure may itself be embedded inside
         * the payload or otherwise owned by the caller.
         */

        newp->color = oldp->color;
        newp->parent = oldp->parent;
        newp->left = oldp->left;
        newp->right = oldp->right;
        newp->root = oldp->root;

        oldp->root = DIMINUTO_TREE_ORPHAN;

    }

    return oldp;
}

/*******************************************************************************
 * PUBLIC ACCESSORS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_next(diminuto_tree_t * nodep)
{
    diminuto_tree_t * parentp;

    if (nodep->root == DIMINUTO_TREE_ORPHAN) {
        parentp = DIMINUTO_TREE_NULL; /* Error: not on a tree! */
    } else if (!diminuto_tree_isleaf(nodep->right)) {
        nodep = nodep->right;
        while (!diminuto_tree_isleaf(nodep->left)) {
            nodep = nodep->left;
        }
        parentp = (diminuto_tree_t *)nodep;
    } else {
        while ((!diminuto_tree_isleaf(parentp = nodep->parent)) && (nodep == parentp->right)) {
            nodep = parentp;
        }
    }

    return parentp;
}

diminuto_tree_t * diminuto_tree_prev(diminuto_tree_t * nodep)
{
    diminuto_tree_t * parentp;

    if (nodep->root == DIMINUTO_TREE_ORPHAN) {
        parentp = DIMINUTO_TREE_NULL; /* Error: not on a tree! */
    } else if (!diminuto_tree_isleaf(nodep->left)) {
        nodep = nodep->left;
        while (!diminuto_tree_isleaf(nodep->right)) {
            nodep = nodep->right;
        }
        parentp = (diminuto_tree_t *)nodep;
    } else {
        while ((!diminuto_tree_isleaf(parentp = nodep->parent)) && (nodep == parentp->left)) {
            nodep = parentp;
        }
    }

    return parentp;
}

diminuto_tree_t * diminuto_tree_first(diminuto_tree_t ** rootp)
{
    diminuto_tree_t * nodep;

    if (!diminuto_tree_isleaf(nodep = (diminuto_tree_t *)*rootp)) {
        while (!diminuto_tree_isleaf(nodep->left)) {
            nodep = nodep->left;
        }
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_last(diminuto_tree_t ** rootp)
{
    diminuto_tree_t * nodep;

    if (!diminuto_tree_isleaf(nodep = (diminuto_tree_t *)*rootp)) {
        while (!diminuto_tree_isleaf(nodep->right)) {
            nodep = nodep->right;
        }
    }

    return nodep;
}

