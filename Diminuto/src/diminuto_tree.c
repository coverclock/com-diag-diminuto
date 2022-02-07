/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Tree feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Three feature.
 *
 * See the header file for references and acknowledgements.
 *
 * This code is complicated enough that I used gcov(1) to profile it to
 * insure that the unit test actually exercised all the nominal code paths.
 * It does. The paths that were not executed were the "this can't happen"
 * paths, those that return NULL (if it is likely to be an error in the
 * caller's use of the API), or that set the error bit in the node (which
 * would likely be a bug in my code). Note that the audit, that is called many
 * times in the unit test, checks for the error bit when it walks the tree.
 */

#include "com/diag/diminuto/diminuto_tree.h"
#include "com/diag/diminuto/diminuto_log.h"

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

static void rotate_left(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
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
    } else if (nodep == parentp->right) {
        parentp->right = rightp;
    } else {
        nodep->error = !0; /* Error: node not child of parent! */
    }

    nodep->parent = rightp;
}

static void rotate_right(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
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
    } else if (nodep == parentp->left) {
        parentp->left = leftp;
    } else {
        nodep->error = !0; /* Error: node not child of parent! */
    }

    nodep->parent = leftp;
}

static void insert_fixup(diminuto_tree_t * nodep, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * parentp;
    diminuto_tree_t * grandp;

    while ((!diminuto_tree_isleaf(parentp = nodep->parent)) && diminuto_tree_isred(parentp)) {

        grandp = parentp->parent;

        if (parentp == grandp->left) {
            diminuto_tree_t * unclep;

            unclep = grandp->right;
            if ((!diminuto_tree_isleaf(unclep)) && diminuto_tree_isred(unclep)) {
                unclep->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;
                grandp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = grandp;
                continue;
            }

            if (parentp->right == nodep) {
                diminuto_tree_t * tempp;
                rotate_left(parentp, rootp);
                tempp = parentp;
                parentp = nodep;
                nodep = tempp;
            }

            parentp->color = DIMINUTO_TREE_COLOR_BLACK;
            grandp->color = DIMINUTO_TREE_COLOR_RED;
            rotate_right(grandp, rootp);

        } else if (parentp == grandp->right) {
            diminuto_tree_t * unclep;

            unclep = grandp->left;
            if ((!diminuto_tree_isleaf(unclep)) && diminuto_tree_isred(unclep)) {
                unclep->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;
                grandp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = grandp;
                continue;
            }

            if (parentp->left == nodep) {
                diminuto_tree_t * tempp;
                rotate_right(parentp, rootp);
                tempp = parentp;
                parentp = nodep;
                nodep = tempp;
            }

            parentp->color = DIMINUTO_TREE_COLOR_BLACK;
            grandp->color = DIMINUTO_TREE_COLOR_RED;
            rotate_left(grandp, rootp);

        } else {

            nodep->error = !0; /* Error: parent is not child of grandparent! */

        }
    }

    (*rootp)->color = DIMINUTO_TREE_COLOR_BLACK;
}

static void remove_fixup(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** rootp)
{
    diminuto_tree_t * siblingp;

    while ((diminuto_tree_isleaf(nodep) || diminuto_tree_isblack(nodep)) && (nodep != *rootp)) {

        if (parentp->left == nodep) {

            siblingp = parentp->right;

            if (diminuto_tree_isred(siblingp)) {

                siblingp->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_RED;
                rotate_left(parentp, rootp);
                siblingp = parentp->right;

            }

            if ((diminuto_tree_isleaf(siblingp->left) || diminuto_tree_isblack(siblingp->left)) && (diminuto_tree_isleaf(siblingp->right) || diminuto_tree_isblack(siblingp->right))) {

                siblingp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = parentp;
                parentp = nodep->parent;

            } else {

                if (diminuto_tree_isleaf(siblingp->right) || diminuto_tree_isblack(siblingp->right)) {

                    if (!diminuto_tree_isleaf(siblingp->left)) {
                        siblingp->left->color = DIMINUTO_TREE_COLOR_BLACK;
                    }

                    siblingp->color = DIMINUTO_TREE_COLOR_RED;
                    rotate_right(siblingp, rootp);
                    siblingp = parentp->right;

                }

                siblingp->color = parentp->color;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;

                if (!diminuto_tree_isleaf(siblingp->right)) {
                    siblingp->right->color = DIMINUTO_TREE_COLOR_BLACK;
                }

                rotate_left(parentp, rootp);
                nodep = *rootp;
                break;

            }

        } else if (parentp->right == nodep) {

            siblingp = parentp->left;

            if (diminuto_tree_isred(siblingp)) {

                siblingp->color = DIMINUTO_TREE_COLOR_BLACK;
                parentp->color = DIMINUTO_TREE_COLOR_RED;
                rotate_right(parentp, rootp);
                siblingp = parentp->left;

            }

            if ((diminuto_tree_isleaf(siblingp->left) || diminuto_tree_isblack(siblingp->left)) && (diminuto_tree_isleaf(siblingp->right) || diminuto_tree_isblack(siblingp->right))) {

                siblingp->color = DIMINUTO_TREE_COLOR_RED;
                nodep = parentp;
                parentp = nodep->parent;

            } else {

                if (diminuto_tree_isleaf(siblingp->left) || diminuto_tree_isblack(siblingp->left)) {

                    if (!diminuto_tree_isleaf(siblingp->right)) {
                        siblingp->right->color = DIMINUTO_TREE_COLOR_BLACK;
                    }

                    siblingp->color = DIMINUTO_TREE_COLOR_RED;
                    rotate_left(siblingp, rootp);
                    siblingp = parentp->left;

                }

                siblingp->color = parentp->color;
                parentp->color = DIMINUTO_TREE_COLOR_BLACK;

                if (!diminuto_tree_isleaf(siblingp->left)) {
                    siblingp->left->color = DIMINUTO_TREE_COLOR_BLACK;
                }

                rotate_right(parentp, rootp);
                nodep = *rootp;
                break;

            }

        } else {

            nodep->error = !0; /* Error: node is not child of parent! */

        }
    }

    if (!diminuto_tree_isleaf(nodep)) {
        nodep->color = DIMINUTO_TREE_COLOR_BLACK;
    }
}

static void link_together(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp, diminuto_tree_t ** linkp)
{
    nodep->color = DIMINUTO_TREE_COLOR_RED;
    nodep->parent = parentp;
    nodep->left = DIMINUTO_TREE_NULL;
    nodep->right = DIMINUTO_TREE_NULL;
    nodep->root = rootp;
    *linkp = nodep;
}

/*******************************************************************************
 * MUTATORS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_insert_left_or_root(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp)
{
    if (!diminuto_tree_isorphan(nodep)) {
        nodep = DIMINUTO_TREE_NULL; /* Error: already on a tree! */
    } else if (diminuto_tree_isleaf(parentp)) {
        if (!diminuto_tree_isempty(rootp)) {
            nodep = DIMINUTO_TREE_NULL; /* Error: root already occupied! */
        } else {
            link_together(nodep, parentp, rootp, rootp);
            insert_fixup(nodep, rootp);
        }
    } else {
        if (!diminuto_tree_isleaf(parentp->left)) {
            nodep = DIMINUTO_TREE_NULL; /* Error: left already occupied! */
        } else {
            link_together(nodep, parentp, parentp->root, &(parentp->left));
            insert_fixup(nodep, nodep->root);
        }
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_insert_right_or_root(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t **rootp)
{
    if (!diminuto_tree_isorphan(nodep)) {
        nodep = DIMINUTO_TREE_NULL; /* Error: already on a tree! */
    } else if (diminuto_tree_isleaf(parentp)) {
        if (!diminuto_tree_isempty(rootp)) {
            nodep = DIMINUTO_TREE_NULL; /* Error: root already occupied! */
        } else {
            link_together(nodep, parentp, rootp, rootp);
            insert_fixup(nodep, rootp);
        }
    } else {
        if (!diminuto_tree_isleaf(parentp->right)) {
            nodep = DIMINUTO_TREE_NULL; /* Error: right already occupied! */
        } else {
            link_together(nodep, parentp, parentp->root, &(parentp->right));
            insert_fixup(nodep, nodep->root);
        }
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_remove(diminuto_tree_t * nodep)
{
    diminuto_tree_t * gonep = DIMINUTO_TREE_NULL;
    diminuto_tree_t * childp;
    diminuto_tree_t * parentp;
    diminuto_tree_t ** rootp;
    int color;

    if (diminuto_tree_isleaf(nodep)) {
        /* Error: is a leaf! */
    } else if (diminuto_tree_isorphan(nodep)) {
        /* Error: not on a tree! */
    } else {

        gonep = nodep;
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
                nodep->root = oldp->root; /* Although presumably root is already set appropriately. */

                if (diminuto_tree_isleaf(oldp->parent)) {
                    *rootp = nodep;
                } else if (oldp->parent->left == oldp) {
                    oldp->parent->left = nodep;
                } else if (oldp->parent->right == oldp) {
                    oldp->parent->right = nodep;
                } else {
                    nodep->error = !0; /* Error: node is neither leaf nor parent! */
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
                nodep->error = !0; /* Error: node is neither leaf nor parent! */
            }

        } while (0);

        if (color == DIMINUTO_TREE_COLOR_BLACK) {
            remove_fixup(childp, parentp, rootp);
        }

        /*
         * It's important to reinitialize the node we just removed so that
         * we don't leave any dangling pointers. If the caller performs a
         * next() or parent() gettor against the removed node, we don't want
         * it to lead to the tree from which it was removed.
         */

        diminuto_tree_init(gonep);

    }

    return gonep;
}

diminuto_tree_t * diminuto_tree_replace(diminuto_tree_t * oldp, diminuto_tree_t * newp)
{
    if (diminuto_tree_isorphan(oldp)) {
        oldp = DIMINUTO_TREE_NULL; /* Error: old node not on a tree! */
    } else if (!diminuto_tree_isorphan(newp)) {
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
            oldp->error = !0; /* Error: node is neither leaf nor parent! */
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

        /*
         * It's important to reinitialize the node we just replaced so that
         * we don't leave any dangling pointers. If the caller performs a
         * next() or parent() gettor against the removed node, we don't want
         * it to lead to the tree from which it was removed.
         */

        diminuto_tree_init(oldp);

    }

    return oldp;
}

/*******************************************************************************
 * ITERATORS
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_next(const diminuto_tree_t * nodep)
{
    diminuto_tree_t * parentp;

    if (diminuto_tree_isorphan(nodep)) {
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

diminuto_tree_t * diminuto_tree_prev(const diminuto_tree_t * nodep)
{
    diminuto_tree_t * parentp;

    if (diminuto_tree_isorphan(nodep)) {
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

    if (!diminuto_tree_isleaf(nodep = *rootp)) {
        while (!diminuto_tree_isleaf(nodep->left)) {
            nodep = nodep->left;
        }
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_last(diminuto_tree_t ** rootp)
{
    diminuto_tree_t * nodep;

    if (!diminuto_tree_isleaf(nodep = *rootp)) {
        while (!diminuto_tree_isleaf(nodep->right)) {
            nodep = nodep->right;
        }
    }

    return nodep;
}

/*******************************************************************************
 * SEARCH
 ******************************************************************************/

diminuto_tree_t * diminuto_tree_search(diminuto_tree_t * nodep, diminuto_tree_t * targetp, diminuto_tree_comparator_t * comparatorp, int * rcp)
{
    if (diminuto_tree_isleaf(nodep)) {
        /* Do nothing. */
    } else {
        *rcp = (*comparatorp)(nodep, targetp);
        if (*rcp < 0) {
            if (!diminuto_tree_isleaf(nodep->right)) {
                nodep = diminuto_tree_search(nodep->right, targetp, comparatorp, rcp);
            }
        } else if (*rcp > 0) {
            if (!diminuto_tree_isleaf(nodep->left)) {
                nodep = diminuto_tree_search(nodep->left, targetp, comparatorp, rcp);
            }
        } else {
            /* Do nothing. */
        }
    }

    return nodep;
}

diminuto_tree_t * diminuto_tree_search_insert_or_replace(diminuto_tree_t ** rootp, diminuto_tree_t * targetp, diminuto_tree_comparator_t * comparatorp, int replace)
{
    diminuto_tree_t * nodep;
    diminuto_tree_t * parentp;
    int rc;

    parentp = diminuto_tree_search(*rootp, targetp, comparatorp, &rc);
    if (parentp == DIMINUTO_TREE_NULL) {
        nodep = diminuto_tree_insert_root(targetp, rootp);
     } else if (rc < 0) {
        nodep = diminuto_tree_insert_right(targetp, parentp);
    } else if (rc > 0) {
        nodep = diminuto_tree_insert_left(targetp, parentp);
    } else if (replace) {
        nodep = diminuto_tree_replace(parentp, targetp);
    } else {
        nodep = DIMINUTO_TREE_NULL;
    }

    return nodep;

}

/*******************************************************************************
 * AUDITS
 ******************************************************************************/

void diminuto_tree_log(const diminuto_tree_t * nodep)
{
    if (nodep) {
        DIMINUTO_LOG_DEBUG("diminuto_tree_t@%p[%zu]: { color=%s parent=%p left=%p right=%p root=%p data=%p error=%d }\n", nodep, sizeof(*nodep), diminuto_tree_isblack(nodep) ? "black" : diminuto_tree_isred(nodep) ? "red" : "invalid", diminuto_tree_parent(nodep), diminuto_tree_left(nodep), diminuto_tree_right(nodep), diminuto_tree_root(nodep), diminuto_tree_data(nodep), diminuto_tree_error(nodep));
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_tree_t@%p[%zu]\n", nodep, sizeof(*nodep));
    }
}

static diminuto_tree_t * audit(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** rootp, int height, int * heightp)
{
    diminuto_tree_t * result = DIMINUTO_TREE_NULL;
    int line = -1;
    do {
        if (!diminuto_tree_isleaf(nodep)) {
            if (diminuto_tree_root(nodep) != rootp) {
                result = nodep;
                line = __LINE__;
                break;
            }
            if (diminuto_tree_parent(nodep) != parentp) {
                result = nodep;
                line = __LINE__;
                break;
            }
            if (*rootp == nodep) {
                if (!diminuto_tree_isleaf(parentp)) {
                    result = nodep;
                    line = __LINE__;
                    break;
                }
            }
            if (diminuto_tree_isleaf(parentp)) {
                if (*rootp != nodep) {
                    result = nodep;
                    line = __LINE__;
                    break;
                }
                if (!diminuto_tree_isblack(nodep)) {
                    result = nodep;
                    line = __LINE__;
                    break;
                }
            } else {
                if (diminuto_tree_left(parentp) == nodep) {
                    /* Do nothing. */
                } else if (diminuto_tree_right(parentp) == nodep) {
                    /* Do nothing. */
                } else {
                    result = nodep;
                    line = __LINE__;
                    break;
                }
            }
            if (diminuto_tree_error(nodep)) {
                result = nodep;
                line = __LINE__;
                break;
            }
            if (!diminuto_tree_isred(nodep)) {
                if (!diminuto_tree_isblack(nodep)) {
                    result = nodep;
                    line = __LINE__;
                    break;
                }
            }
            if (diminuto_tree_isblack(nodep)) {
                ++height;
            }
            if (!diminuto_tree_isleaf(diminuto_tree_left(nodep))) {
                if (diminuto_tree_isred(nodep)) {
                    if (!diminuto_tree_isblack(diminuto_tree_left(nodep))) {
                        result = nodep;
                        line = __LINE__;
                        break;
                    }
                }
                result = audit(diminuto_tree_left(nodep), nodep, rootp, height, heightp);
                if (result != DIMINUTO_TREE_NULL) {
                    break;
                }
            }
            if (!diminuto_tree_isleaf(diminuto_tree_right(nodep))) {
                if (diminuto_tree_isred(nodep)) {
                    if (!diminuto_tree_isblack(diminuto_tree_right(nodep))) {
                        result = nodep;
                        line = __LINE__;
                        break;
                    }
                }
                result = audit(diminuto_tree_right(nodep), nodep, rootp, height, heightp);
                if (result != DIMINUTO_TREE_NULL) {
                    break;
                }
            }
            if ((!diminuto_tree_isleaf(diminuto_tree_left(nodep))) || (!diminuto_tree_isleaf(diminuto_tree_right(nodep)))) {
                /* Do nothing: not leaf. */
            } else if (*heightp < 0) {
                *heightp = height;
            } else if (*heightp == height) {
                /* Do nothing: nominal. */
            } else {
                result = nodep;
                line = __LINE__;
                break;
            }
        }
    } while (0);
    if (result == DIMINUTO_TREE_NULL) {
        /* Do nothing: no error found. */
    } else if (line < 0) {
        /* Do nothing: error message already logged. */
    } else {
        DIMINUTO_LOG_DEBUG("%s@%d: diminuto_tree_audit FAILED!\n", __FILE__, line);
        diminuto_tree_log(result);
    }
    return result;
}

diminuto_tree_t *  diminuto_tree_audit(diminuto_tree_t ** rootp)
{
    int height = -1;
    return audit(*rootp, DIMINUTO_TREE_NULL, rootp, 0, &height);
}
