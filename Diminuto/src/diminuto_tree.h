//* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TREE_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_TREE_PRIVATE_

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
 * PRIVATE
 ******************************************************************************/

static inline void diminuto_tree_link(diminuto_tree_t * nodep, diminuto_tree_t * parentp, diminuto_tree_t ** linkp)
{
	nodep->color = DIMINUTO_TREE_COLOR_RED;
    nodep->parent = parentp;
    nodep->left = DIMINUTO_TREE_NULL;
    nodep->left = DIMINUTO_TREE_NULL;
    *linkp = nodep;
}

#endif
