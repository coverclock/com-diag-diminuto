/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_store.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static void list(diminuto_tree_t ** rootp)
{
    diminuto_store_t * nodep;
    printf("list: root=%p\n", rootp);
    for (nodep = diminuto_store_first(rootp); !diminuto_store_isnull(nodep); nodep = diminuto_store_next(nodep)) {
        printf("list: %s=\"%s\"\n", (const char *)(nodep->key), (const char *)(nodep->value));
    }
}

static diminuto_store_t ALPHABET[] = {
    DIMINUTO_STORE_KEYVALUEINIT("ALFA", "1"),
    DIMINUTO_STORE_KEYVALUEINIT("BRAVO", "2"),
    DIMINUTO_STORE_KEYVALUEINIT("CHARLIE", "3"),
    DIMINUTO_STORE_KEYVALUEINIT("DELTA", "4"),
    DIMINUTO_STORE_KEYVALUEINIT("ECHO", "5"),
    DIMINUTO_STORE_KEYVALUEINIT("FOXTROT", "6"),
    DIMINUTO_STORE_KEYVALUEINIT("GOLF", "7"),
    DIMINUTO_STORE_KEYVALUEINIT("HOTEL", "8"),
    DIMINUTO_STORE_KEYVALUEINIT("INDIA", "9"),
    DIMINUTO_STORE_KEYVALUEINIT("JULIETT", "10"),
    DIMINUTO_STORE_KEYVALUEINIT("KILO", "11"),
    DIMINUTO_STORE_KEYVALUEINIT("LIMO", "12"),
    DIMINUTO_STORE_KEYVALUEINIT("MIKE", "13"),
    DIMINUTO_STORE_KEYVALUEINIT("NOVEMBER", "14"),
    DIMINUTO_STORE_KEYVALUEINIT("OSCAR", "15"),
    DIMINUTO_STORE_KEYVALUEINIT("PAPA", "16"),
    DIMINUTO_STORE_KEYVALUEINIT("QUEBEC", "17"),
    DIMINUTO_STORE_KEYVALUEINIT("ROMEO", "18"),
    DIMINUTO_STORE_KEYVALUEINIT("SIERRA", "19"),
    DIMINUTO_STORE_KEYVALUEINIT("TANGO", "20"),
    DIMINUTO_STORE_KEYVALUEINIT("UNIFORM", "21"),
    DIMINUTO_STORE_KEYVALUEINIT("VICTOR", "22"),
    DIMINUTO_STORE_KEYVALUEINIT("WHISKEY", "23"),
    DIMINUTO_STORE_KEYVALUEINIT("XRAY", "24"),
    DIMINUTO_STORE_KEYVALUEINIT("YANKEE", "25"),
    DIMINUTO_STORE_KEYVALUEINIT("ZULU", "26"),
};

int main(void)
{
	SETLOGMASK();

    {
        ASSERT(DIMINUTO_STORE_NULL == (diminuto_store_t *)0);
    }

    {
        diminuto_store_t alpha = DIMINUTO_STORE_KEYVALUEINIT("Aa", 1);
        diminuto_store_t beta = DIMINUTO_STORE_KEYVALUEINIT("Bbb", 2);
        diminuto_store_t gamma = DIMINUTO_STORE_KEYVALUEINIT("Cccc", 3);
        diminuto_store_t able = DIMINUTO_STORE_KEYVALUEINIT("Aa", 4);
        diminuto_store_t baker = DIMINUTO_STORE_KEYVALUEINIT("Bbb", 5);
        diminuto_store_t charlie = DIMINUTO_STORE_KEYVALUEINIT("Cccc", 6);
        ASSERT(diminuto_store_compare_strings(&alpha, &beta) < 0);
        ASSERT(diminuto_store_compare_strings(&beta, &gamma) < 0);
        ASSERT(diminuto_store_compare_strings(&alpha, &gamma) < 0);
        ASSERT(diminuto_store_compare_strings(&beta, &alpha) > 0);
        ASSERT(diminuto_store_compare_strings(&gamma, &beta) > 0);
        ASSERT(diminuto_store_compare_strings(&gamma, &alpha) > 0);
        ASSERT(diminuto_store_compare_strings(&alpha, &alpha) == 0);
        ASSERT(diminuto_store_compare_strings(&beta, &beta) == 0);
        ASSERT(diminuto_store_compare_strings(&gamma, &gamma) == 0);
        ASSERT(diminuto_store_compare_strings(&alpha, &able) == 0);
        ASSERT(diminuto_store_compare_strings(&beta, &baker) == 0);
        ASSERT(diminuto_store_compare_strings(&gamma, &charlie) == 0);
    }

    {
        diminuto_store_t node;
        ASSERT(diminuto_containerof(diminuto_store_t, tree, &node.tree) == &node);
   }

    {
        diminuto_tree_t * root = DIMINUTO_TREE_EMPTY;
        diminuto_store_t * nodep;
        diminuto_store_t * resultp;
        diminuto_store_t target;
        ssize_t ii;
        list(&root);
        ASSERT(diminuto_tree_isempty(&root));
        ASSERT(diminuto_store_first(&root) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_last(&root) == DIMINUTO_STORE_NULL);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[ii]);
            target.key = nodep->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
            EXPECT((resultp = diminuto_store_insert(&root, nodep, diminuto_store_compare_strings)) == nodep);
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
        }
        list(&root);
        ASSERT(diminuto_store_first(&root) == &ALPHABET[0]);
        ASSERT(diminuto_store_last(&root) == &ALPHABET[countof(ALPHABET) - 1]);
    }

    EXIT();
}
