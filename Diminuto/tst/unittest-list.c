/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the List feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the List feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_comparator.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static diminuto_list_t head;
static diminuto_list_t node[3];
static char * name[countof(node)] = { "node0", "node1", "node2" };
static diminuto_list_t * stack[64];

static void dump(const char * prefix, diminuto_list_t * nodep) {
    diminuto_list_t * nextp = nodep;
    int ii = 0;
    do {
        diminuto_list_log(nextp);
        nextp = nextp->next;
    } while (nextp != nodep);
}

static void audit(const char * file, int line, diminuto_list_t * rootp, ...)
{
    int ii;
    int jj;
    diminuto_list_t * expected;
    diminuto_list_t * actual;
    va_list ap;

    CHECKPOINT("audit:%s@%d\n", file, line);

    /* Forward */

    actual = rootp;
    ii = 0;
    va_start(ap, rootp);
    while ((expected = va_arg(ap, diminuto_list_t *)) != DIMINUTO_LIST_NULL) {
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

    ASSERT(diminuto_list_audit(rootp) == DIMINUTO_LIST_NULL);
}

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

    if (DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG)) {
        dump("head", &head);
    }
}

int main(void)
{
    SETLOGMASK();

    {
        /* Core Operations */

        char datum[] = "datum";

        TEST();

        ASSERT(DIMINUTO_LIST_NULL == (diminuto_list_t *)0);

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_insert(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &head, DIMINUTO_LIST_NULL);
        ASSERT(!diminuto_list_isempty(&head));

        diminuto_list_remove(&node[1]);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));

        diminuto_list_remove(&node[1]);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));

        diminuto_list_remove(&head);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));

        ASSERT(diminuto_list_init(&head) == &head);
        diminuto_list_dataset(&head, datum);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT((const char *)diminuto_list_data(&head) == datum);
        ASSERT(diminuto_list_fini(&head) == (diminuto_list_t *)0);

        ASSERT(diminuto_list_init(&head) == &head);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT((const char *)diminuto_list_data(&head) == datum);
        ASSERT(diminuto_list_fini(&head) == (diminuto_list_t *)0);

        ASSERT(diminuto_list_nullinit(&head) == &head);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT(diminuto_list_data(&head) == (void *)0);
        ASSERT(diminuto_list_fini(&head) == (diminuto_list_t *)0);

        ASSERT(diminuto_list_datainit(&head, datum) == &head);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_isempty(&head));
        ASSERT((const char *)diminuto_list_data(&head) == datum);
        ASSERT(diminuto_list_fini(&head) == (diminuto_list_t *)0);

        STATUS();
    }

    {
        /* Static Initializer */

        static const diminuto_list_t HEAD = DIMINUTO_LIST_INITIALIZER;
        char datum[] = "datum";
        char other[] = "other";

        TEST();

        ASSERT(sizeof(head) == sizeof(HEAD));
        memcpy(&head, &HEAD, sizeof(head));
        ASSERT(head.next == (diminuto_list_t *)0);
        ASSERT(head.prev == (diminuto_list_t *)0);
        ASSERT(head.root == (diminuto_list_t *)0);
        ASSERT(head.data == (void *)0);

        head.root = &head;
        diminuto_list_initif(&head);
        ASSERT(head.next == (diminuto_list_t *)0);
        ASSERT(head.prev == (diminuto_list_t *)0);
        ASSERT(head.root == &head);
        ASSERT(head.data == (void *)0);

        head.root = (diminuto_list_t *)0;
        diminuto_list_initif(&head);
        ASSERT(head.next == &head);
        ASSERT(head.prev == &head);
        ASSERT(head.root == &head);
        ASSERT(head.data == (void *)0);
        ASSERT(diminuto_list_isempty(&head));
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        diminuto_list_datasetif(&head, datum);
        ASSERT((const char *)diminuto_list_data(&head) == datum);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        diminuto_list_datasetif(&head, other);
        ASSERT((const char *)diminuto_list_data(&head) == datum);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        diminuto_list_dataset(&head, other);
        ASSERT((const char *)diminuto_list_data(&head) == other);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_fini(&head) == (diminuto_list_t *)0);

        STATUS();
    }

    {
        /* Stack Operations */

        TEST();

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_pop(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        diminuto_list_push(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);

        diminuto_list_push(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);

        diminuto_list_push(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1],  &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_pop(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_pop(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_pop(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_pop(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        STATUS();
    }

    {
        /* Queue Operations */

        TEST();

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_dequeue(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        diminuto_list_enqueue(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, DIMINUTO_LIST_NULL);

        diminuto_list_enqueue(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &head, DIMINUTO_LIST_NULL);

        diminuto_list_enqueue(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1],  &node[2], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_dequeue(&head) == &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_dequeue(&head) == &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_dequeue(&head) == &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_dequeue(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_head(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);
        ASSERT(diminuto_list_tail(&head) == DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head, &head, &head, DIMINUTO_LIST_NULL);

        STATUS();
    }

    {
        /* Data and Functors 1 */

        TEST();

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

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

        STATUS();
    }

    {
        /* Data and Functors 2 */

        int total;

        TEST();

        initialize();

        total = 0;
        ASSERT(diminuto_list_apply(&count, diminuto_list_next(&head), &total) == &head);
        ASSERT(total == 0);

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        total = 0;
        ASSERT(diminuto_list_apply(&count, diminuto_list_next(&head), &total) == &head);
        ASSERT(total == 3);

        STATUS();
    }

    {
        /* Reroot */

        diminuto_list_t temp;

        TEST();

        diminuto_list_init(&temp);

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        /* Example of moving all nodes from one root to another. */

        while (!diminuto_list_isempty(&head)) {
            diminuto_list_insert(&temp,
                diminuto_list_prev(&head)
            );
        }

        audit(__FILE__, __LINE__, &temp, &temp, &node[0], &node[1], &node[2], &temp, DIMINUTO_LIST_NULL);

        ASSERT(diminuto_list_reroot(&node[1]) == &node[1]);

        audit(__FILE__, __LINE__, &node[1], &node[1], &node[2], &temp, &node[0], &node[1], DIMINUTO_LIST_NULL);

        STATUS();
    }

    {
        /* Replace 1 */

        diminuto_list_t temp;

        TEST();

        diminuto_list_init(&temp);

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        diminuto_list_replace(&node[1], &temp);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &temp, &node[2], &head, DIMINUTO_LIST_NULL);

        STATUS();
    }

    {
        /* Replace 2 */

        diminuto_list_t temp;

        TEST();

        diminuto_list_init(&temp);

        initialize();

        diminuto_list_insert(
            diminuto_list_insert(
                diminuto_list_insert(&head, &node[0]),
            &node[1]),
        &node[2]);

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        /* Another example of moving all nodes from one root to another. */

        diminuto_list_replace(&head, &temp);

        audit(__FILE__, __LINE__, &temp, &temp, &node[0], &node[1], &node[2], &temp, DIMINUTO_LIST_NULL);

        STATUS();
    }

    {
        /* Iteration */

        int ii;
        diminuto_list_t * np;

        TEST();

        initialize();

        for (ii = 0; ii < countof(node); ++ii) {
            diminuto_list_insert(diminuto_list_prev(&head), &node[ii]);
        }

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        ii = 0;
        for (np = diminuto_list_next(&head); !diminuto_list_isroot(np); np = diminuto_list_next(np)) {
            ASSERT(np == &node[ii++]);
        }

        audit(__FILE__, __LINE__, &head, &head, &node[0], &node[1], &node[2], &head, DIMINUTO_LIST_NULL);

        STATUS();
    }

    {
        /* Cut/Splice */

        diminuto_list_t head1;
        diminuto_list_t list1[4];
        diminuto_list_t head2;
        diminuto_list_t list2[5];
        diminuto_list_t * np;

        TEST();

        diminuto_list_datainit(&head1, (void*)0x10);
        diminuto_list_enqueue(&head1, diminuto_list_datainit(&list1[0], (void *)0x11));
        diminuto_list_enqueue(&head1, diminuto_list_datainit(&list1[1], (void *)0x12));
        diminuto_list_enqueue(&head1, diminuto_list_datainit(&list1[2], (void *)0x13));
        diminuto_list_enqueue(&head1, diminuto_list_datainit(&list1[3], (void *)0x14));
        audit(__FILE__, __LINE__, &head1, &head1, &list1[0], &list1[1], &list1[2], &list1[3], &head1, DIMINUTO_LIST_NULL);

        diminuto_list_datainit(&head2, (void*)0x20);
        diminuto_list_enqueue(&head2, diminuto_list_datainit(&list2[0], (void *)0x21));
        diminuto_list_enqueue(&head2, diminuto_list_datainit(&list2[1], (void *)0x22));
        diminuto_list_enqueue(&head2, diminuto_list_datainit(&list2[2], (void *)0x23));
        diminuto_list_enqueue(&head2, diminuto_list_datainit(&list2[3], (void *)0x24));
        diminuto_list_enqueue(&head2, diminuto_list_datainit(&list2[4], (void *)0x25));
        audit(__FILE__, __LINE__, &head2, &head2, &list2[0], &list2[1], &list2[2], &list2[3], &list2[4], &head2, DIMINUTO_LIST_NULL);

        np = diminuto_list_cut(&list2[1], &list2[3]);
        ASSERT(np == &list2[1]);
        audit(__FILE__, __LINE__, &head2, &head2, &list2[0], &list2[4], &head2, DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &list2[1], &list2[1], &list2[2], &list2[3], &list2[1], DIMINUTO_LIST_NULL);

        np = diminuto_list_splice(&list1[1], &list2[2]);
        ASSERT(np == &list2[2]);
        audit(__FILE__, __LINE__, &head1, &head1, &list1[0], &list1[1], &list2[2], &list2[3], &list2[1], &list1[2], &list1[3], &head1, DIMINUTO_LIST_NULL);

        /* Different Lists */

        np = diminuto_list_cut(&list2[1], &list2[0]);
        ASSERT(np == &list2[1]);
        audit(__FILE__, __LINE__, &head2, &head2, &list2[0], &list2[4], &head2, DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &head1, &head1, &list1[0], &list1[1], &list2[2], &list2[3], &list2[1], &list1[2], &list1[3], &head1, DIMINUTO_LIST_NULL);

        /* Complete List */

        np = diminuto_list_cut(&head2, &list2[4]);
        ASSERT(np == &head2);
        audit(__FILE__, __LINE__, &head2, &head2, &list2[0], &list2[4], &head2, DIMINUTO_LIST_NULL);

        /* Single Node */

        np = diminuto_list_cut(&list2[0], &list2[0]);
        ASSERT(np == &list2[0]);
        audit(__FILE__, __LINE__, &head2, &head2, &list2[4], &head2, DIMINUTO_LIST_NULL);
        audit(__FILE__, __LINE__, &list2[0], &list2[0], &list2[0], DIMINUTO_LIST_NULL);

        np = diminuto_list_splice(&list1[1], &list2[0]);
        ASSERT(np == &list2[0]);
        audit(__FILE__, __LINE__, &head1, &head1, &list1[0], &list1[1], &list2[0], &list2[2], &list2[3], &list2[1], &list1[2], &list1[3], &head1, DIMINUTO_LIST_NULL);

        /* Already There */

        np = diminuto_list_splice(&list2[0], &list1[1]);
        ASSERT(np == &list1[1]);
        audit(__FILE__, __LINE__, &head1, &head1, &list1[0], &list1[1], &list2[0], &list2[2], &list2[3], &list2[1], &list1[2], &list1[3], &head1, DIMINUTO_LIST_NULL);

        STATUS();
   }

    EXIT();
}
