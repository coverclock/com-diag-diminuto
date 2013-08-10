/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_list.h"

diminuto_list_t * diminuto_list_remove(
    diminuto_list_t * nodep
) {
    if ((!diminuto_list_isroot(nodep)) && (!diminuto_list_isempty(nodep))) {
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
    if (!diminuto_list_isempty(nodep)) {
        diminuto_list_remove(nodep);
    }
    nodep->next = rootp->next;
    nodep->prev = rootp;
    nodep->root = rootp->root;
    rootp->next->prev = nodep;
    rootp->next = nodep;
    return nodep;
}

diminuto_list_t * diminuto_list_reroot(
    diminuto_list_t * nodep
) {
    diminuto_list_t * nextp = nodep;
    do {
        nextp->root = nodep;
    } while ((nextp = diminuto_list_next(nextp)) != nodep);
    return nodep;
}

diminuto_list_t * diminuto_list_replace(
	diminuto_list_t * oldp,
	diminuto_list_t * newp
) {
    diminuto_list_insert(oldp, newp);
    if (diminuto_list_isroot(oldp)) {
    	diminuto_list_reroot(newp);
    }
    return diminuto_list_remove(oldp);
}

diminuto_list_t * diminuto_list_apply(
	diminuto_list_functor_t * funcp,
    diminuto_list_t * nodep,
    void * contextp
) {
    int rc;
    while (!0) {
        rc = (*funcp)(diminuto_list_data(nodep), contextp);
        if (rc < 0) {
            nodep = diminuto_list_prev(nodep);
        } else if (rc > 0) {
            nodep = diminuto_list_next(nodep);
        } else {
            break;
        }
    }
    return nodep;
}

diminuto_list_t * diminuto_list_fini(
	diminuto_list_t * rootp
) {
    while (!diminuto_list_isempty(rootp)) {
    	diminuto_list_remove(diminuto_list_next(rootp));
    }
    return rootp;
}
