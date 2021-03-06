/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the List feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the List feature.
 */

#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_log.h"

/**
 * This is a private internal general reroot operator that reroots a range of
 * nodes to any arbitary root node. No checking is done to insure that the
 * range of nodes are actually on the same list, nor is there a check to insure
 * that the root node is on the list.
 * @param firstp points to the first node to rereoot.
 * @param lastp points to the last node to reroot.
 * @param rootp points to the root node.
 * @return the pointer to the root node.
 */
static diminuto_list_t * reroot(
    diminuto_list_t * firstp,
    diminuto_list_t * lastp,
    diminuto_list_t * rootp
) {
    diminuto_list_t * nextp = firstp;
    do {
        nextp->root = rootp;
        if (nextp == lastp) {
            break;
        }
        nextp = nextp->next;
    } while (nextp != firstp);
    return rootp;
}

diminuto_list_t * diminuto_list_reroot(
    diminuto_list_t * nodep
) {
    return reroot(nodep, nodep->prev, nodep);
}

diminuto_list_t * diminuto_list_remove(
    diminuto_list_t * nodep
) {
    if (nodep != nodep->next) {
        if (nodep == nodep->root) {
            reroot(nodep->next, nodep->prev, nodep->next);
        }
        nodep->next->prev = nodep->prev;
        nodep->prev->next = nodep->next;
        diminuto_list_init(nodep);
    }
    return nodep;
}

diminuto_list_t * diminuto_list_insert(
    diminuto_list_t * rootp,
    diminuto_list_t * nodep
) {
    diminuto_list_remove(nodep);
    nodep->next = rootp->next;
    nodep->prev = rootp;
    rootp->next->prev = nodep;
    rootp->next = nodep;
    nodep->root = rootp->root;
    return nodep;
}

diminuto_list_t * diminuto_list_cut(
    diminuto_list_t * firstp,
    diminuto_list_t * lastp
) {
    if (firstp->root != lastp->root) {
        /* Do nothing. */
    } else if (firstp->prev == lastp) {
        /* Do nothing. */
    } else if (firstp == lastp) {
        diminuto_list_remove(firstp);
    } else {
        diminuto_list_t * nodep = firstp->prev;
        do {
            nodep = nodep->next;
            if (nodep == firstp->root) {
                reroot(lastp->next, firstp->prev, lastp->next);
                break;
            }
        } while (nodep != lastp);
        firstp->prev->next = lastp->next;
        lastp->next->prev = firstp->prev;
        firstp->prev = lastp;
        lastp->next = firstp;
        reroot(firstp, lastp, firstp);
    }
    return firstp;
}

diminuto_list_t * diminuto_list_splice(
    diminuto_list_t * top,
    diminuto_list_t * fromp
) {
    if (fromp->root == top->root) {
        /* Do nothing. */
    } else if (fromp == fromp->next) {
        diminuto_list_insert(top, fromp);
    } else {
        reroot(fromp, fromp->prev, top->root);
        fromp->prev->next = top->next;
        top->next->prev = fromp->prev;
        top->next = fromp;
        fromp->prev = top;
    }
    return fromp;
}

diminuto_list_t * diminuto_list_replace(
    diminuto_list_t * oldp,
    diminuto_list_t * newp
) {
    diminuto_list_insert(oldp, newp);
    return diminuto_list_remove(oldp);
}

diminuto_list_t * diminuto_list_apply(
    diminuto_list_functor_t * funcp,
    diminuto_list_t * nodep,
    void * contextp
) {
    int rc;
    while (!0) {
        rc = (*funcp)(nodep->data, contextp);
        if (rc < 0) {
            nodep = nodep->prev;
        } else if (rc > 0) {
            nodep = nodep->next;
        } else {
            break;
        }
    }
    return nodep;
}

void diminuto_list_log(
    const diminuto_list_t * nodep
) {
    if (nodep) {
        DIMINUTO_LOG_DEBUG("diminuto_list_t@%p[%zu]: { next=%p prev=%p root=%p data=%p }\n", nodep, sizeof(*nodep), nodep->next, nodep->prev, nodep->root, nodep->data);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_list_t@%p[%zu]\n", nodep, sizeof(*nodep));
    }
}

const diminuto_list_t * diminuto_list_audit(
    const diminuto_list_t * nodep
) {
    const diminuto_list_t * rootp = diminuto_list_root(nodep);
    const diminuto_list_t * nextp = nodep;
    const diminuto_list_t * prevp = nodep;
    while (!0) {
        if ((nextp->root != rootp) || (nextp->next->prev != nextp) || (nextp->prev->next != nextp)) {
            DIMINUTO_LOG_DEBUG("%s@%d: diminuto_list_audit FAILED!\n", __FILE__, __LINE__);
            diminuto_list_log(nextp);
            nodep = nextp;
            break;
        } else if ((prevp->root != rootp) || (prevp->prev->next != prevp) || (prevp->next->prev != prevp)) {
            DIMINUTO_LOG_DEBUG("%s@%d: diminuto_list_audit FAILED!\n", __FILE__, __LINE__);
            diminuto_list_log(prevp);
            nodep = prevp;
            break;
        } else if ((nextp = nextp->next) == (prevp = prevp->prev)) {
            nodep = DIMINUTO_LIST_NULL;
            break;
        } else {
            continue;
        }
    }
    return nodep;
}
