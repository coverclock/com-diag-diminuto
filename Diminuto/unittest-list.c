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
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_comparator.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define END ((diminuto_list_t *)0)

static diminuto_list_t head;
static diminuto_list_t node[3];
static char * name[countof(node)] = { "node0", "node1", "node2" };

/*
 * Example FIND functor.
 */
static int find(void * datap, void * contextp)
{
    return datap ? diminuto_compare_strings(contextp, datap) : 0;
}

/*
 * Example COUNT functor.
 */
static int count(void * datap, void * contextp)
{
    return datap ? (++(*((int *)contextp)), 1) : 0;
}

static void initialize(void)
{
    int ii;
    int jj;

    diminuto_list_nullinit(&head);
    ASSERT(diminuto_list_isempty(&head));
    ASSERT(diminuto_list_isroot(&head));
    ASSERT(diminuto_list_ismember(&head, &head));
    ASSERT(diminuto_list_data(&head) == (void *)0);
    ASSERT(diminuto_list_dataif(&head) == (void *)0);
    for (ii = 0; ii < countof(node); ++ii) {
        diminuto_list_datainit(&node[ii], name[ii]);
        ASSERT(diminuto_list_isempty(&node[ii]));
        ASSERT(diminuto_list_isroot(&node[ii]));
        ASSERT(!diminuto_list_ismember(&head, &node[ii]));
        ASSERT(!diminuto_list_ismember(&node[ii], &head));
        ASSERT(diminuto_list_data(&node[ii]) == name[ii]);
        ASSERT(diminuto_list_dataif(&node[ii]) == name[ii]);
    }
    ASSERT(diminuto_list_dataif((diminuto_list_t *)0) == (void *)0);

    for (ii = 0; ii < countof(node); ++ii) {
    	for (jj = 0; jj < countof(node); ++jj) {
    		ASSERT(diminuto_list_aresiblings(&node[ii], &node[jj]) == (ii == jj));
    	}
    }
}

static void audit(const char * file, int line, diminuto_list_t * rootp, ...)
{
    diminuto_list_t * stack[countof(node) + 2];
    int ii;
    int jj;
    diminuto_list_t * expected;
    diminuto_list_t * actual;
    va_list ap;

#if 0
    printf("audit:%s@%d\n", file, line);
#endif

    /* Forward */

    actual = rootp;
    ii = 0;
    va_start(ap, rootp);
    while ((expected = va_arg(ap, diminuto_list_t *)) != END) {
        ASSERT(ii < countof(stack));
        stack[ii++] = expected;
        ASSERT(expected == actual);
        ASSERT(diminuto_list_isroot(rootp));
        ASSERT(diminuto_list_ismember(rootp, actual));
        if (rootp != actual) {
            ASSERT(!diminuto_list_isempty(rootp));
            ASSERT(!diminuto_list_isroot(actual));
            ASSERT(!diminuto_list_ismember(actual, rootp));
        }
        actual = diminuto_list_next(actual);
    }
    va_end(ap);

    /* Reverse */

    actual = rootp;
    while (ii > 0) {
        expected = stack[--ii];
        ASSERT(expected == actual);
        ASSERT(diminuto_list_isroot(rootp));
        ASSERT(diminuto_list_ismember(rootp, actual));
        if (rootp != actual) {
            ASSERT(!diminuto_list_isempty(rootp));
            ASSERT(!diminuto_list_isroot(actual));
            ASSERT(!diminuto_list_ismember(actual, rootp));
        }
        actual = diminuto_list_prev(actual);
    }

    /* Siblingship */

    actual = rootp;
    expected = rootp;
    do {
    	do {
    		ASSERT(diminuto_list_aresiblings(actual, expected));
    	} while ((actual = diminuto_list_next(actual)) != rootp);
    } while ((expected = diminuto_list_prev(expected)) != rootp);


    /* Audit */

    ASSERT(diminuto_list_audit(rootp) == (diminuto_list_t *)0);

}

int main(void)
{
    {
        /* Core Operations */

        char datum[] = "datum";

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &node[0], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &head, END);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[1]);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));

        diminuto_list_remove(&node[1]);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));

        diminuto_list_remove(&head);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));

        ASSERT(diminuto_list_init(&head) == &head);
        diminuto_list_dataset(&head, datum);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT((const char *)diminuto_list_data(&head) == datum);

        ASSERT(diminuto_list_init(&head) == &head);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT((const char *)diminuto_list_data(&head) == datum);

        ASSERT(diminuto_list_nullinit(&head) == &head);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT(diminuto_list_data(&head) == (void *)0);

        ASSERT(diminuto_list_datainit(&head, datum) == &head);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT((const char *)diminuto_list_data(&head) == datum);
    }
    {
        /* Stack Operations */

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_pop(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        diminuto_list_push(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        diminuto_list_push(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        diminuto_list_push(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1],  &node[0], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, END);

        ASSERT(diminuto_list_pop(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        ASSERT(diminuto_list_pop(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        ASSERT(diminuto_list_pop(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_pop(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
    }

    {
        /* Queue Operations */

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_dequeue(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        diminuto_list_enqueue(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        diminuto_list_enqueue(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &head, END);

        diminuto_list_enqueue(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1],  &node[2], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        ASSERT(diminuto_list_dequeue(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &head, END);

        ASSERT(diminuto_list_dequeue(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &head, END);

        ASSERT(diminuto_list_head(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &head, END);
        ASSERT(diminuto_list_tail(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &head, END);

        ASSERT(diminuto_list_dequeue(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_dequeue(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        ASSERT(diminuto_list_head(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
        ASSERT(diminuto_list_tail(&head) == END);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
    }

    {
        /* Data and Functors 1 */

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        ASSERT(diminuto_list_apply(&find, diminuto_list_next(&head), "node0") == &node[0]);
        ASSERT(diminuto_list_apply(&find, diminuto_list_next(&head), "node1") == &node[1]);
        ASSERT(diminuto_list_apply(&find, diminuto_list_next(&head), "node2") == &node[2]);

        ASSERT(diminuto_list_apply(&find, diminuto_list_prev(&head), "node0") == &node[0]);
        ASSERT(diminuto_list_apply(&find, diminuto_list_prev(&head), "node1") == &node[1]);
        ASSERT(diminuto_list_apply(&find, diminuto_list_prev(&head), "node2") == &node[2]);

        ASSERT(diminuto_list_apply(&find, diminuto_list_next(&head), "node3") == &head);
        ASSERT(diminuto_list_apply(&find, diminuto_list_prev(&head), "node3") == &head);

        /* Example if removing all nodes on a list. */

        while (!diminuto_list_isempty(&head)) {
            diminuto_list_remove(diminuto_list_next(&head));
        }
    }

    {
        /* Data and Functors 2 */

        int total;

        initialize();

        total = 0;
        ASSERT(diminuto_list_apply(&count, diminuto_list_next(&head), &total) == &head);
        ASSERT(total == 0);

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        total = 0;
        ASSERT(diminuto_list_apply(&count, diminuto_list_next(&head), &total) == &head);
        ASSERT(total == 3);

    }

    {
        /* Reroot */

        diminuto_list_t temp;

        diminuto_list_init(&temp);

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        /* Example of moving all nodes from one root to another. */

        while (!diminuto_list_isempty(&head)) {
            diminuto_list_insert(&temp,
                diminuto_list_prev(&head)
            );
        }

        audit(__FILE__, __LINE__, &temp, &temp, &node[0], &node[1], &node[2], &temp, END);

        ASSERT(diminuto_list_reroot(&node[1]) == &node[1]);

        audit(__FILE__, __LINE__, &node[1], &node[1], &node[2], &temp, &node[0], &node[1], END);

    }

    {
        /* Replace 1 */

        diminuto_list_t temp;

        diminuto_list_init(&temp);

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        diminuto_list_replace(&node[1], &temp);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &temp, &node[2], &head, END);
    }

    {
        /* Replace 2 */

        diminuto_list_t temp;

        diminuto_list_init(&temp);

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        /* Another example of moving all nodes from one root to another. */

        diminuto_list_replace(&head, &temp);

        audit(__FILE__, __LINE__, &temp, &temp, &node[0], &node[1], &node[2], &temp, END);
    }

    {
        int ii;
        diminuto_list_t * np;

        /* Iteration */

        initialize();

        for (ii = 0; ii < countof(node); ++ii) {
            diminuto_list_insert(diminuto_list_prev(&head), &node[ii]);
        }

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);

        ii = 0;
        for (np = diminuto_list_next(&head); !diminuto_list_isroot(np); np = diminuto_list_next(np)) {
            ASSERT(np == &node[ii++]);
        }

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, END);
    }

    return 0;
}
