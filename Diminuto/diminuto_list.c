/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_list.h"

diminuto_list * diminuto_list_init(diminuto_list * nodep)
{
    nodep->next = nodep;
    nodep->prev = nodep;
    nodep->root = nodep;
    nodep->data = (void *)0;
    return nodep;
}

diminuto_list * diminuto_list_remove(diminuto_list * nodep)
{
    if (!diminuto_list_isempty(nodep)) {
        diminuto_list_prev(diminuto_list_next(nodep)) = diminuto_list_prev(nodep);
        diminuto_list_next(diminuto_list_prev(nodep)) = diminuto_list_next(nodep);
        diminuto_list_init(nodep);
    }
    return nodep;
}

diminuto_list * diminuto_list_insert(diminuto_list * listp, diminuto_list * nodep)
{
    if (!diminuto_list_isempty(nodep)) {
        diminuto_list_remove(nodep);
    }
    diminuto_list_next(nodep) = diminuto_list_next(listp);
    diminuto_list_prev(nodep) = listp;
    diminuto_list_root(nodep) = diminuto_list_root(listp);
    diminuto_list_prev(diminuto_list_next(listp)) = nodep;
    diminuto_list_next(listp) = nodep;
    return nodep;
}

diminuto_list * diminuto_list_apply(diminuto_list * listp, diminuto_list_functor * funcp, void * voidp)
{
    int rc;
    diminuto_list * herep = listp;
    while (1) {
        rc = (*funcp)(listp, herep, voidp);
        if (rc < 0) {
            herep = diminuto_list_prev(herep);
        } else if (rc > 0) {
            herep = diminuto_list_next(herep);
        } else {
            break;
        }
    }
    return herep;
}
