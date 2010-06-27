/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>
#include <stdarg.h>
#include "diminuto_list.h"
#include "diminuto_countof.h"
#include "diminuto_unittest.h"

#define END ((diminuto_list *)0)

static diminuto_list head;
static diminuto_list node[3];

static void audit(const char * file, int line, diminuto_list * rootp, ...)
{
    diminuto_list * expected;
    diminuto_list * actual;
    va_list ap;

    printf("audit:%s@%d\n", file, line);
    actual = rootp;
    va_start(ap, rootp);
    while ((expected = va_arg(ap, diminuto_list *)) != END) {
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
}

static void initialize(void)
{
    int ii;

    diminuto_list_init(&head);
    ASSERT(diminuto_list_isempty(&head));
    ASSERT(diminuto_list_isroot(&head));
    ASSERT(diminuto_list_ismember(&head, &head));
    for (ii = 0; ii < countof(node); ++ii) {
        diminuto_list_init(&node[ii]);
        ASSERT(diminuto_list_isempty(&node[ii]));
        ASSERT(diminuto_list_isroot(&node[ii]));
        ASSERT(!diminuto_list_ismember(&head, &node[ii]));
        ASSERT(!diminuto_list_ismember(&node[ii], &head));
    }
}

int main(void)
{
    {
        /* Core Operations */

        initialize();
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        diminuto_list_insert(&head, &node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[0], &head, END);

        diminuto_list_insert(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        diminuto_list_insert(&head, &node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[2], &node[1], &node[0], &head, END);

        diminuto_list_insert(&head, &node[1]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[2], &node[0], &head, END);

        diminuto_list_remove(&node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        diminuto_list_remove(&node[2]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &node[0], &head, END);

        diminuto_list_remove(&node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &head, END);

        diminuto_list_remove(&node[0]);
        audit(__FILE__, __LINE__, &head, &head, &node[1], &head, END);

        diminuto_list_remove(&node[1]);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        diminuto_list_remove(&node[1]);
        audit(__FILE__, __LINE__, &head, &head, &head, END);

        diminuto_list_remove(&head);
        audit(__FILE__, __LINE__, &head, &head, &head, END);
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

    return 0;
}
