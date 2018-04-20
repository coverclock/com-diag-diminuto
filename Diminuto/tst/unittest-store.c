/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#define _GNU_SOURCE /* for strfry(3) */
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_store.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if !defined(COM_DIAG_DIMINUTO_PLATFORM_GLIBC)
#   warning platform may not provide strfry(3)!
static inline char * strfry(char *string) {
    return string;
}
#endif

static int compare_strings(diminuto_store_t * thisp, diminuto_store_t * thatp)
{
    return diminuto_store_compare_strings(diminuto_store_upcast(thisp), diminuto_store_upcast(thatp));
}

static void list(diminuto_store_t ** rootp)
{
    diminuto_store_t * nodep;
    printf("list: root=%p\n", rootp);
    for (nodep = diminuto_store_first(rootp); !diminuto_store_ismissing(nodep); nodep = diminuto_store_next(nodep)) {
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

static diminuto_store_t ALFABIT[] = {
    DIMINUTO_STORE_KEYVALUEINIT("ALFA", "one"),
    DIMINUTO_STORE_KEYVALUEINIT("BRAVO", "two"),
    DIMINUTO_STORE_KEYVALUEINIT("CHARLIE", "three"),
    DIMINUTO_STORE_KEYVALUEINIT("DELTA", "four"),
    DIMINUTO_STORE_KEYVALUEINIT("ECHO", "five"),
    DIMINUTO_STORE_KEYVALUEINIT("FOXTROT", "six"),
    DIMINUTO_STORE_KEYVALUEINIT("GOLF", "seven"),
    DIMINUTO_STORE_KEYVALUEINIT("HOTEL", "eight"),
    DIMINUTO_STORE_KEYVALUEINIT("INDIA", "nine"),
    DIMINUTO_STORE_KEYVALUEINIT("JULIETT", "ten"),
    DIMINUTO_STORE_KEYVALUEINIT("KILO", "eleven"),
    DIMINUTO_STORE_KEYVALUEINIT("LIMO", "twelve"),
    DIMINUTO_STORE_KEYVALUEINIT("MIKE", "thirteen"),
    DIMINUTO_STORE_KEYVALUEINIT("NOVEMBER", "fourteen"),
    DIMINUTO_STORE_KEYVALUEINIT("OSCAR", "fifteen"),
    DIMINUTO_STORE_KEYVALUEINIT("PAPA", "sixteen"),
    DIMINUTO_STORE_KEYVALUEINIT("QUEBEC", "seventeen"),
    DIMINUTO_STORE_KEYVALUEINIT("ROMEO", "eighteen"),
    DIMINUTO_STORE_KEYVALUEINIT("SIERRA", "nineteen"),
    DIMINUTO_STORE_KEYVALUEINIT("TANGO", "twenty"),
    DIMINUTO_STORE_KEYVALUEINIT("UNIFORM", "twenty-one"),
    DIMINUTO_STORE_KEYVALUEINIT("VICTOR", "twenty-two"),
    DIMINUTO_STORE_KEYVALUEINIT("WHISKEY", "twenty-three"),
    DIMINUTO_STORE_KEYVALUEINIT("XRAY", "twenty-four"),
    DIMINUTO_STORE_KEYVALUEINIT("YANKEE", "twenty-five"),
    DIMINUTO_STORE_KEYVALUEINIT("ZULU", "twenty-six"),
};

int main(void)
{
	SETLOGMASK();

    {
        ASSERT(DIMINUTO_STORE_NULL == (diminuto_store_t *)0);
        ASSERT(DIMINUTO_STORE_EMPTY == (diminuto_store_t *)0);
    }

    {
        diminuto_store_t first = DIMINUTO_STORE_NULLINIT;
        char KEY[] = "key";
        char VALUE[] = "value";
        ASSERT(diminuto_store_keyget(&first) == (void *)0);
        ASSERT(diminuto_store_valueget(&first) == (void *)0);
        diminuto_store_keyvalueinit(&first, KEY, VALUE);
        ASSERT(diminuto_store_keyget(&first) == KEY);
        ASSERT(diminuto_store_valueget(&first) == VALUE);
        diminuto_store_init(&first);
        ASSERT(diminuto_store_keyget(&first) == KEY);
        ASSERT(diminuto_store_valueget(&first) == VALUE);
        diminuto_store_nullinit(&first);
        ASSERT(diminuto_store_keyget(&first) == (void *)0);
        ASSERT(diminuto_store_valueget(&first) == (void *)0);
        diminuto_store_keyset(&first, KEY);
        ASSERT(diminuto_store_keyget(&first) == KEY);
        ASSERT(diminuto_store_valueget(&first) == (void *)0);
        diminuto_store_valueset(&first, VALUE);
        ASSERT(diminuto_store_keyget(&first) == KEY);
        ASSERT(diminuto_store_valueget(&first) == VALUE);
        diminuto_store_nullinit(&first);
        ASSERT(diminuto_store_keyget(&first) == (void *)0);
        ASSERT(diminuto_store_valueget(&first) == (void *)0);
        diminuto_store_keyvalueset(&first, KEY, VALUE);
        ASSERT(diminuto_store_keyget(&first) == KEY);
        ASSERT(diminuto_store_valueget(&first) == VALUE);
    }

    {
        diminuto_store_t alpha = DIMINUTO_STORE_KEYVALUEINIT("Aa", 1);
        diminuto_store_t beta = DIMINUTO_STORE_KEYVALUEINIT("Bbb", 2);
        diminuto_store_t gamma = DIMINUTO_STORE_KEYVALUEINIT("Cccc", 3);
        diminuto_store_t able = DIMINUTO_STORE_KEYVALUEINIT("Aa", 4);
        diminuto_store_t baker = DIMINUTO_STORE_KEYVALUEINIT("Bbb", 5);
        diminuto_store_t charlie = DIMINUTO_STORE_KEYVALUEINIT("Cccc", 6);
        ASSERT(compare_strings(&alpha, &beta) < 0);
        ASSERT(compare_strings(&beta, &gamma) < 0);
        ASSERT(compare_strings(&alpha, &gamma) < 0);
        ASSERT(compare_strings(&beta, &alpha) > 0);
        ASSERT(compare_strings(&gamma, &beta) > 0);
        ASSERT(compare_strings(&gamma, &alpha) > 0);
        ASSERT(compare_strings(&alpha, &alpha) == 0);
        ASSERT(compare_strings(&beta, &beta) == 0);
        ASSERT(compare_strings(&gamma, &gamma) == 0);
        ASSERT(compare_strings(&alpha, &able) == 0);
        ASSERT(compare_strings(&beta, &baker) == 0);
        ASSERT(compare_strings(&gamma, &charlie) == 0);
    }

    {
        diminuto_store_t node;
        ASSERT(diminuto_containerof(diminuto_store_t, tree, &node.tree) == &node);
   }

    {
        size_t ii;
        ASSERT(countof(ALPHABET) == countof(ALFABIT));
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            ASSERT(strcmp((const char *)ALPHABET[ii].key, (const char *)ALFABIT[ii].key) == 0);
        }
    }

    {
        diminuto_store_t * root = DIMINUTO_STORE_EMPTY;
        diminuto_store_t foo = DIMINUTO_STORE_KEYVALUEINIT("FOO", "0");
        diminuto_store_t bar = DIMINUTO_STORE_KEYVALUEINIT("BAR", "0");
        diminuto_store_t * nodep;
        ASSERT(diminuto_store_remove(DIMINUTO_STORE_NULL) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_remove(diminuto_store_find(&root, DIMINUTO_STORE_NULL, diminuto_store_compare_strings)) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_remove(diminuto_store_find(&root, &foo, diminuto_store_compare_strings)) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_find(&root, &foo, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_insert(&root, &foo, diminuto_store_compare_strings) == &foo);
        ASSERT(diminuto_store_find(&root, &foo, diminuto_store_compare_strings) == &foo);
        ASSERT(diminuto_store_remove(diminuto_store_find(&root, &bar, diminuto_store_compare_strings)) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_remove(nodep = diminuto_store_find(&root, &foo, diminuto_store_compare_strings)) == &foo);
        ASSERT(nodep == &foo);
        ASSERT(diminuto_tree_isorphan(diminuto_store_upcast(nodep)));
        ASSERT(diminuto_store_find(&root, &foo, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_remove(diminuto_store_find(&root, &foo, diminuto_store_compare_strings)) == DIMINUTO_STORE_NULL);
    }

    {
        char biasedindices[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        diminuto_store_t * root = DIMINUTO_STORE_EMPTY;
        diminuto_store_t * nodep;
        diminuto_store_t * otherp;
        diminuto_store_t target;
        ssize_t ii;
        list(&root);
        ASSERT(diminuto_store_audit(&root) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_isempty(&root));
        ASSERT(diminuto_store_first(&root) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_last(&root) == DIMINUTO_STORE_NULL);
        strfry(biasedindices);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[biasedindices[ii] - 'A']);
            target.key = nodep->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
            ASSERT(diminuto_store_insert(&root, nodep, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_audit(&root) == DIMINUTO_STORE_NULL);
        }
        list(&root);
        ASSERT(!diminuto_store_isempty(&root));
        ASSERT(diminuto_store_first(&root) == &ALPHABET[0]);
        ASSERT(diminuto_store_last(&root) == &ALPHABET[countof(ALPHABET) - 1]);
        strfry(biasedindices);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[biasedindices[ii] - 'A']);
            target.key = nodep->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_insert(&root, nodep, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_audit(&root) == DIMINUTO_STORE_NULL);
        }
        list(&root);
        ASSERT(!diminuto_store_isempty(&root));
        ASSERT(diminuto_store_first(&root) == &ALPHABET[0]);
        ASSERT(diminuto_store_last(&root) == &ALPHABET[countof(ALPHABET) - 1]);
        strfry(biasedindices);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[biasedindices[ii] - 'A']);
            target.key = nodep->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_insert(&root, nodep, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
        }
        list(&root);
        ASSERT(diminuto_store_audit(&root) == DIMINUTO_STORE_NULL);
        ASSERT(!diminuto_store_isempty(&root));
        ASSERT(diminuto_store_first(&root) == &ALPHABET[0]);
        ASSERT(diminuto_store_last(&root) == &ALPHABET[countof(ALPHABET) - 1]);
        strfry(biasedindices);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[biasedindices[ii] - 'A']);
            otherp = &(ALFABIT[biasedindices[ii] - 'A']);
            target.key = nodep->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_replace(&root, otherp, diminuto_store_compare_strings) == nodep);
            target.key = otherp->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == otherp);
            ASSERT(diminuto_store_audit(&root) == DIMINUTO_STORE_NULL);
        }
        list(&root);
        ASSERT(!diminuto_store_isempty(&root));
        ASSERT(diminuto_store_first(&root) == &ALFABIT[0]);
        ASSERT(diminuto_store_last(&root) == &ALFABIT[countof(ALFABIT) - 1]);
        strfry(biasedindices);
        for (ii = 0; ii < countof(ALFABIT); ++ii) {
            nodep = &(ALFABIT[biasedindices[ii] - 'A']);
            target.key = nodep->key;
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == nodep);
            ASSERT(diminuto_store_remove(nodep) == nodep);
            ASSERT(diminuto_store_find(&root, &target, diminuto_store_compare_strings) == DIMINUTO_STORE_NULL);
            ASSERT(diminuto_store_audit(&root) == DIMINUTO_STORE_NULL);
        }
        list(&root);
        ASSERT(diminuto_store_isempty(&root));
        ASSERT(diminuto_store_isempty(&root));
        ASSERT(diminuto_store_first(&root) == DIMINUTO_STORE_NULL);
        ASSERT(diminuto_store_last(&root) == DIMINUTO_STORE_NULL);
    }

    EXIT();
}
