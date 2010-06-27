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

diminuto_list * diminuto_list_insert(diminuto_list * rootp, diminuto_list * nodep)
{
    if (!diminuto_list_isempty(nodep)) {
        diminuto_list_remove(nodep);
    }
    diminuto_list_next(nodep) = diminuto_list_next(rootp);
    diminuto_list_prev(nodep) = rootp;
    diminuto_list_root(nodep) = diminuto_list_root(rootp);
    diminuto_list_prev(diminuto_list_next(rootp)) = nodep;
    diminuto_list_next(rootp) = nodep;
    return nodep;
}

diminuto_list * diminuto_list_apply(diminuto_list_functor * funcp, diminuto_list * nodep, void * contextp)
{
    int rc;
    while (1) {
        rc = (*funcp)(nodep, contextp);
        if (rc < 0) {
            nodep = diminuto_list_prev(nodep);
        } else if (rc > 0) {
            nodep = diminuto_list_next(nodep);
        } else {
            return nodep;
        }
    }
}
