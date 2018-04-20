/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_tree.h"

struct Alpha {
	char * key;
	char * value;
	diminuto_list_t list;
};

struct Beta {
	char * key;
	char * value;
	diminuto_tree_t tree;
};

int main(void)
{
	struct Alpha alpha;
	diminuto_list_t * listp;
	struct Beta beta;
	diminuto_tree_t * treep;
	/**/
	listp = &(alpha.list);
    ASSERT(diminuto_containerof(struct Alpha, list, listp) == &alpha);
    ASSERT(containerof(struct Alpha, list, listp) == &alpha);
    /**/
    treep = &(beta.tree);
    ASSERT(diminuto_containerof(struct Beta, tree, treep) == &beta);
    ASSERT(containerof(struct Beta, tree, treep) == &beta);
    /**/
    EXIT();
}
