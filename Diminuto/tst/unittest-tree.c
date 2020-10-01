/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_tree.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_comparator.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if !defined(_GNU_SOURCE)
#   warning strfry(3) not available on this platform!
static inline char * strfry(char *string) {
    return string;
}
#endif

static const char SPACES[] = "                                                                                ";

static void dumpp(diminuto_tree_t * nodep)
{
    printf("data=%p\n", nodep->data);
}

static void dumps(diminuto_tree_t * nodep)
{
    printf("data=\"%s\"\n", (const char *)nodep->data);
}

typedef void (dumpf_t)(diminuto_tree_t * nodep);

static void dumpn(diminuto_tree_t * nodep, dumpf_t * dumpfp)
{
    printf("node=%p color=%s parent=%p left=%p right=%p root=%p error=%d ", (void *)nodep, diminuto_tree_isblack(nodep) ? "black" : diminuto_tree_isred(nodep) ? "red" : "invalid", (void *)diminuto_tree_parent(nodep), (void *)diminuto_tree_left(nodep), (void *)diminuto_tree_right(nodep), (void *)diminuto_tree_root(nodep), diminuto_tree_error(nodep));
    (*dumpfp)(nodep);
}

static void dumpr(int depth, int height, diminuto_tree_t * nodep, dumpf_t * dumpfp)
{
    if (diminuto_tree_isblack(nodep)) {
        ++height;
    }
    printf("dump: %sdepth=%d height=%d ", &(SPACES[(depth < sizeof(SPACES)) ? (sizeof(SPACES) - 1 - depth) : 0]), depth, height);
    dumpn(nodep, dumpfp);
    ++depth;
    if (!diminuto_tree_isleaf(nodep->left)) {
        dumpr(depth, height, nodep->left, dumpfp);
    }
    if (!diminuto_tree_isleaf(nodep->right)) {
        dumpr(depth, height, nodep->right, dumpfp);
    }
}

static void dump(int line, diminuto_tree_t ** rootp, dumpf_t * dumpfp)
{
    printf("dump: line=%d root=%p\n", line, (void *)rootp);
    if (!diminuto_tree_isempty(rootp)) {
        dumpr(1, 0, *rootp, dumpfp);
    }
}

static void traverser(diminuto_tree_t * nodep, dumpf_t * dumpfp)
{
    if (!diminuto_tree_isleaf(nodep->left)) {
        traverser(nodep->left, dumpfp);
    }
    printf("traverse: ");
    dumpn(nodep, dumpfp);
    if (!diminuto_tree_isleaf(nodep->right)) {
        traverser(nodep->right, dumpfp);
    }
}

static void traverse(int line, diminuto_tree_t ** rootp, dumpf_t * dumpfp)
{
    printf("traverse: line=%d root=%p\n", line, (void *)rootp);
    if (!diminuto_tree_isempty(rootp)) {
        traverser(*rootp, dumpfp);
    }
}

static void list(int line, diminuto_tree_t ** rootp, dumpf_t * dumpfp)
{
    diminuto_tree_t * nodep;
    printf("list: line=%d root=%p\n", line, (void *)rootp);
    for (nodep = diminuto_tree_first(rootp); nodep != DIMINUTO_TREE_NULL; nodep = diminuto_tree_next(nodep)) {
        printf("list: ");
        dumpn(nodep, dumpfp);
    }
}

static diminuto_tree_t *audit(int line, diminuto_tree_t ** rootp, dumpf_t * dumpfp)
{
    diminuto_tree_t * nodep;
    diminuto_tree_t * prevp;
    nodep = diminuto_tree_audit(rootp);
    if (nodep == DIMINUTO_TREE_NULL) {
        if (dumpfp == dumps) {
            for (prevp = DIMINUTO_TREE_NULL, nodep = diminuto_tree_first(rootp); nodep != DIMINUTO_TREE_NULL; prevp = nodep, nodep = diminuto_tree_next(nodep)) {
                if (prevp != DIMINUTO_TREE_NULL) {
                    if (!(strcmp((const char *)prevp->data, (const char *)nodep->data) < 0)) {
                        break;
                    }
                }
            }
        }
    }
    if (nodep != DIMINUTO_TREE_NULL) {
        list(line, rootp, dumpfp);
        dump(line, rootp, dumpfp);
    }
    return nodep;
}

static int comparator(diminuto_tree_t * nodep, diminuto_tree_t * targetp)
{
    return strcmp(nodep->data, targetp->data);
}

static diminuto_tree_t * find(diminuto_tree_t ** rootp, void * key)
{
    diminuto_tree_t * nodep;
    diminuto_tree_t target = DIMINUTO_TREE_NULLINIT;
    int rc = 0;
    target.data = key;
    nodep = diminuto_tree_search(*rootp, &target, &comparator, &rc);
    return (rc == 0) ? nodep : (diminuto_tree_t *)0;
}

static diminuto_tree_t ALPHABET[] = {
    DIMINUTO_TREE_DATAINIT("ALFA"),
    DIMINUTO_TREE_DATAINIT("BRAVO"),
    DIMINUTO_TREE_DATAINIT("CHARLIE"),
    DIMINUTO_TREE_DATAINIT("DELTA"),
    DIMINUTO_TREE_DATAINIT("ECHO"),
    DIMINUTO_TREE_DATAINIT("FOXTROT"),
    DIMINUTO_TREE_DATAINIT("GOLF"),
    DIMINUTO_TREE_DATAINIT("HOTEL"),
    DIMINUTO_TREE_DATAINIT("INDIA"),
    DIMINUTO_TREE_DATAINIT("JULIETT"),
    DIMINUTO_TREE_DATAINIT("KILO"),
    DIMINUTO_TREE_DATAINIT("LIMO"),
    DIMINUTO_TREE_DATAINIT("MIKE"),
    DIMINUTO_TREE_DATAINIT("NOVEMBER"),
    DIMINUTO_TREE_DATAINIT("OSCAR"),
    DIMINUTO_TREE_DATAINIT("PAPA"),
    DIMINUTO_TREE_DATAINIT("QUEBEC"),
    DIMINUTO_TREE_DATAINIT("ROMEO"),
    DIMINUTO_TREE_DATAINIT("SIERRA"),
    DIMINUTO_TREE_DATAINIT("TANGO"),
    DIMINUTO_TREE_DATAINIT("UNIFORM"),
    DIMINUTO_TREE_DATAINIT("VICTOR"),
    DIMINUTO_TREE_DATAINIT("WHISKEY"),
    DIMINUTO_TREE_DATAINIT("XRAY"),
    DIMINUTO_TREE_DATAINIT("YANKEE"),
    DIMINUTO_TREE_DATAINIT("ZULU"),
};

static diminuto_tree_t ALFABIT[] = {
    DIMINUTO_TREE_DATAINIT("ALFA"),
    DIMINUTO_TREE_DATAINIT("BRAVO"),
    DIMINUTO_TREE_DATAINIT("CHARLIE"),
    DIMINUTO_TREE_DATAINIT("DELTA"),
    DIMINUTO_TREE_DATAINIT("ECHO"),
    DIMINUTO_TREE_DATAINIT("FOXTROT"),
    DIMINUTO_TREE_DATAINIT("GOLF"),
    DIMINUTO_TREE_DATAINIT("HOTEL"),
    DIMINUTO_TREE_DATAINIT("INDIA"),
    DIMINUTO_TREE_DATAINIT("JULIETT"),
    DIMINUTO_TREE_DATAINIT("KILO"),
    DIMINUTO_TREE_DATAINIT("LIMO"),
    DIMINUTO_TREE_DATAINIT("MIKE"),
    DIMINUTO_TREE_DATAINIT("NOVEMBER"),
    DIMINUTO_TREE_DATAINIT("OSCAR"),
    DIMINUTO_TREE_DATAINIT("PAPA"),
    DIMINUTO_TREE_DATAINIT("QUEBEC"),
    DIMINUTO_TREE_DATAINIT("ROMEO"),
    DIMINUTO_TREE_DATAINIT("SIERRA"),
    DIMINUTO_TREE_DATAINIT("TANGO"),
    DIMINUTO_TREE_DATAINIT("UNIFORM"),
    DIMINUTO_TREE_DATAINIT("VICTOR"),
    DIMINUTO_TREE_DATAINIT("WHISKEY"),
    DIMINUTO_TREE_DATAINIT("XRAY"),
    DIMINUTO_TREE_DATAINIT("YANKEE"),
    DIMINUTO_TREE_DATAINIT("ZULU"),
};

int main(void)
{
    SETLOGMASK();

    {
        ASSERT(DIMINUTO_TREE_NULL == (diminuto_tree_t *)0);
        ASSERT(DIMINUTO_TREE_ORPHAN == (diminuto_tree_t **)0);
        ASSERT((DIMINUTO_TREE_COLOR_RED == 0) || (DIMINUTO_TREE_COLOR_RED == 1));
        ASSERT((DIMINUTO_TREE_COLOR_BLACK == 0) || (DIMINUTO_TREE_COLOR_BLACK == 1));
        ASSERT(DIMINUTO_TREE_COLOR_RED != DIMINUTO_TREE_COLOR_BLACK);
    }

    {
        diminuto_tree_t node = DIMINUTO_TREE_NULLINIT;
        ASSERT(node.color == DIMINUTO_TREE_COLOR_RED);
        ASSERT(!node.error);
        ASSERT(node.parent == DIMINUTO_TREE_NULL);
        ASSERT(node.left == DIMINUTO_TREE_NULL);
        ASSERT(node.right == DIMINUTO_TREE_NULL);
        ASSERT(node.root == DIMINUTO_TREE_ORPHAN);
        ASSERT(node.data == (void *)0);
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&node) == (void *)0);
        ASSERT(diminuto_tree_isred(&node));
        ASSERT(!diminuto_tree_isblack(&node));
    }

    {
        diminuto_tree_t node = DIMINUTO_TREE_DATAINIT((void *)0x55555555);
        ASSERT(node.color == DIMINUTO_TREE_COLOR_RED);
        ASSERT(!node.error);
        ASSERT(node.parent == DIMINUTO_TREE_NULL);
        ASSERT(node.left == DIMINUTO_TREE_NULL);
        ASSERT(node.right == DIMINUTO_TREE_NULL);
        ASSERT(node.root == DIMINUTO_TREE_ORPHAN);
        ASSERT(node.data == (void *)0x55555555);
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&node) == (void *)0x55555555);
        ASSERT(diminuto_tree_isred(&node));
        ASSERT(!diminuto_tree_isblack(&node));
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(!diminuto_tree_isred(&node));
    }

    {
        diminuto_tree_t node;
        node.data = (void *)0x55555555;
        diminuto_tree_init(&node);
        ASSERT(node.color == DIMINUTO_TREE_COLOR_RED);
        ASSERT(!node.error);
        ASSERT(node.parent == DIMINUTO_TREE_NULL);
        ASSERT(node.left == DIMINUTO_TREE_NULL);
        ASSERT(node.right == DIMINUTO_TREE_NULL);
        ASSERT(node.root == DIMINUTO_TREE_ORPHAN);
        ASSERT(node.data == (void *)0x55555555);
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&node) == (void *)0x55555555);
        ASSERT(diminuto_tree_isred(&node));
        ASSERT(!diminuto_tree_isblack(&node));
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(!diminuto_tree_isred(&node));
    }

    {
        diminuto_tree_t node;
        node.data = (void *)0x55555555;
        diminuto_tree_datainit(&node, (void *)0xAAAAAAAA);
        ASSERT(node.color == DIMINUTO_TREE_COLOR_RED);
        ASSERT(!node.error);
        ASSERT(node.parent == DIMINUTO_TREE_NULL);
        ASSERT(node.left == DIMINUTO_TREE_NULL);
        ASSERT(node.right == DIMINUTO_TREE_NULL);
        ASSERT(node.root == DIMINUTO_TREE_ORPHAN);
        ASSERT(node.data == (void *)0xAAAAAAAA);
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&node) == (void *)0xAAAAAAAA);
        ASSERT(diminuto_tree_isred(&node));
        ASSERT(!diminuto_tree_isblack(&node));
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(!diminuto_tree_isred(&node));
    }

    {
        diminuto_tree_t node;
        diminuto_tree_nullinit(&node);
        ASSERT(node.color == DIMINUTO_TREE_COLOR_RED);
        ASSERT(!node.error);
        ASSERT(node.parent == DIMINUTO_TREE_NULL);
        ASSERT(node.left == DIMINUTO_TREE_NULL);
        ASSERT(node.right == DIMINUTO_TREE_NULL);
        ASSERT(node.root == DIMINUTO_TREE_ORPHAN);
        ASSERT(node.data == (void *)0);
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&node) == (void *)0);
        ASSERT(diminuto_tree_isred(&node));
        ASSERT(!diminuto_tree_isblack(&node));
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(!diminuto_tree_isred(&node));
    }

    {
        diminuto_tree_t node;
        diminuto_tree_init(&node);
        ASSERT(diminuto_tree_isred(&node));
        ASSERT(!diminuto_tree_isblack(&node));
        ASSERT(!diminuto_tree_error(&node));
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        ASSERT(!diminuto_tree_set(&node));
        ASSERT(diminuto_tree_set(&node));
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        node.parent = (diminuto_tree_t *)0x11111111;
        node.left = (diminuto_tree_t *)0x22222222;
        node.right = (diminuto_tree_t *)0x33333333;
        node.root = (diminuto_tree_t **)0x44444444;
        ASSERT(!diminuto_tree_isred(&node));
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(diminuto_tree_error(&node));
        ASSERT(diminuto_tree_parent(&node) == (diminuto_tree_t *)0x11111111);
        ASSERT(diminuto_tree_left(&node) == (diminuto_tree_t *)0x22222222);
        ASSERT(diminuto_tree_right(&node) == (diminuto_tree_t *)0x33333333);
        ASSERT(diminuto_tree_root(&node) == (diminuto_tree_t **)0x44444444);
        ASSERT(diminuto_tree_dataset(&node, (void *)0x55555555) == &node);
        ASSERT(diminuto_tree_data(&node) == (void *)0x55555555);
        ASSERT(diminuto_tree_clear(&node));
        ASSERT(!diminuto_tree_error(&node));
        ASSERT(!diminuto_tree_isred(&node));
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(!diminuto_tree_clear(&node));
     }

    {
        diminuto_tree_t * root = DIMINUTO_TREE_EMPTY;
        diminuto_tree_t * spare = DIMINUTO_TREE_EMPTY;
        /*
         * Using ((void*)0) as a data value yields "(nil)" for the %p format
         * which confuses me.
         */
        diminuto_tree_t node[3] = { DIMINUTO_TREE_DATAINIT((void *)1),  DIMINUTO_TREE_DATAINIT((void *)2),  DIMINUTO_TREE_DATAINIT((void *)3), };
        diminuto_tree_t dummy =  DIMINUTO_TREE_DATAINIT((void *)4);
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_isleaf(DIMINUTO_TREE_NULL));
        ASSERT(!diminuto_tree_isleaf(&(node[0])));
        ASSERT(!diminuto_tree_isleaf(&(node[1])));
        ASSERT(!diminuto_tree_isleaf(&(node[2])));
        ASSERT(diminuto_tree_isorphan(&(node[0])));
        ASSERT(diminuto_tree_isorphan(&(node[1])));
        ASSERT(diminuto_tree_isorphan(&(node[2])));
        ASSERT(diminuto_tree_parent(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[0])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&(node[0])) == (void *)1);
        ASSERT(diminuto_tree_parent(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[1])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&(node[1])) == (void *)2);
        ASSERT(diminuto_tree_parent(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[2])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&(node[2])) == (void *)3);
        ASSERT(diminuto_tree_first(&root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_last(&root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_remove(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_remove(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_remove(&(node[2])) == DIMINUTO_TREE_NULL);
        /**/
        ASSERT(diminuto_tree_insert_root(&(node[1]), &root) == &(node[1]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_isorphan(&(node[0])));
        ASSERT(!diminuto_tree_isorphan(&(node[1])));
        ASSERT(diminuto_tree_isorphan(&(node[2])));
        ASSERT(diminuto_tree_parent(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[0])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&(node[0])) == (void *)1);
        ASSERT(diminuto_tree_parent(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[1])) == &root);
        ASSERT(diminuto_tree_data(&(node[1])) == (void *)2);
        ASSERT(diminuto_tree_parent(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[2])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&(node[2])) == (void *)3);
        ASSERT(diminuto_tree_first(&root) == &(node[1]));
        ASSERT(diminuto_tree_last(&root) == &(node[1]));
        ASSERT(diminuto_tree_next(&(node[1])) == DIMINUTO_TREE_NULL);
        /**/
        ASSERT(diminuto_tree_insert_left(&(node[0]), &(node[1])) == &(node[0]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(!diminuto_tree_isorphan(&(node[0])));
        ASSERT(!diminuto_tree_isorphan(&(node[1])));
        ASSERT(diminuto_tree_isorphan(&(node[2])));
        ASSERT(diminuto_tree_parent(&(node[0])) == &(node[1]));
        ASSERT(diminuto_tree_left(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[0])) == &root);
        ASSERT(diminuto_tree_data(&(node[0])) == (void *)1);
        ASSERT(diminuto_tree_parent(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[1])) == &(node[0]));
        ASSERT(diminuto_tree_right(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[1])) == &root);
        ASSERT(diminuto_tree_data(&(node[1])) == (void *)2);
        ASSERT(diminuto_tree_parent(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[2])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_data(&(node[2])) == (void *)3);
        ASSERT(diminuto_tree_first(&root) == &(node[0]));
        ASSERT(diminuto_tree_last(&root) == &(node[1]));
        ASSERT(diminuto_tree_next(&(node[0])) == &(node[1]));
        ASSERT(diminuto_tree_next(&(node[1])) == DIMINUTO_TREE_NULL);
        /**/
        ASSERT(diminuto_tree_insert_right(&(node[2]), &(node[1])) == &(node[2]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(!diminuto_tree_isorphan(&(node[0])));
        ASSERT(!diminuto_tree_isorphan(&(node[1])));
        ASSERT(!diminuto_tree_isorphan(&(node[2])));
        ASSERT(diminuto_tree_parent(&(node[0])) == &(node[1]));
        ASSERT(diminuto_tree_left(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[0])) == &root);
        ASSERT(diminuto_tree_data(&(node[0])) == (void *)1);
        ASSERT(diminuto_tree_parent(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[1])) == &(node[0]));
        ASSERT(diminuto_tree_right(&(node[1])) == &(node[2]));
        ASSERT(diminuto_tree_root(&(node[1])) == &root);
        ASSERT(diminuto_tree_data(&(node[1])) == (void *)2);
        ASSERT(diminuto_tree_parent(&(node[2])) == &(node[1]));
        ASSERT(diminuto_tree_left(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[2])) == &root);
        ASSERT(diminuto_tree_data(&(node[2])) == (void *)3);
        ASSERT(diminuto_tree_first(&root) == &(node[0]));
        ASSERT(diminuto_tree_next(&(node[0])) == &(node[1]));
        ASSERT(diminuto_tree_next(&(node[1])) == &(node[2]));
        ASSERT(diminuto_tree_next(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_last(&root) == &(node[2]));
        /**/
        ASSERT(diminuto_tree_remove(&(node[0])) == &(node[0]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_root(&(node[0])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_replace(&node[0], &node[0]) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_replace(&node[2], &node[2]) == DIMINUTO_TREE_NULL);
        /**/
        ASSERT(diminuto_tree_replace(&node[2], &node[0]) == &(node[2]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_root(&(node[2])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_root(&(node[0])) == &root);
        /**/
        ASSERT(diminuto_tree_replace(&(node[1]), &(node[2])) == &(node[1]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[1])) == DIMINUTO_TREE_ORPHAN);
        ASSERT(diminuto_tree_root(&(node[2])) == &root);
        ASSERT(diminuto_tree_insert_root(&(node[1]), &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_right(&(node[1]), &(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_left(&(node[0]), &(node[2])) == DIMINUTO_TREE_NULL);
        /**/
        ASSERT(diminuto_tree_insert_left(&(node[1]), &(node[2])) == &(node[1]));
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_parent(&(node[0])) == &(node[2]));
        ASSERT(diminuto_tree_left(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[0])) == &root);
        ASSERT(diminuto_tree_data(&(node[0])) == (void *)1);
        ASSERT(diminuto_tree_parent(&(node[1])) == &(node[2]));
        ASSERT(diminuto_tree_left(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&(node[1])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&(node[1])) == &root);
        ASSERT(diminuto_tree_data(&(node[1])) == (void *)2);
        ASSERT(diminuto_tree_parent(&(node[2])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&(node[2])) == &(node[1]));
        ASSERT(diminuto_tree_right(&(node[2])) == &(node[0]));
        ASSERT(diminuto_tree_root(&(node[2])) == &root);
        ASSERT(diminuto_tree_data(&(node[2])) == (void *)3);
        ASSERT(diminuto_tree_first(&root) == &(node[1]));
        ASSERT(diminuto_tree_next(&(node[1])) == &(node[2]));
        ASSERT(diminuto_tree_next(&(node[2])) == &(node[0]));
        ASSERT(diminuto_tree_next(&(node[0])) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_last(&root) == &(node[0]));
        /**/
        ASSERT(root != DIMINUTO_TREE_EMPTY);
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_remove(&(node[0])) == &(node[0]));
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_remove(&(node[1])) == &(node[1]));
        ASSERT(!diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_remove(&(node[2])) == &(node[2]));
        ASSERT(diminuto_tree_isempty(&root));
        ASSERT(root == DIMINUTO_TREE_EMPTY);
        ASSERT(audit(__LINE__, &root, dumpp) == DIMINUTO_TREE_NULL);
    }

    {
        diminuto_tree_t * root = DIMINUTO_TREE_EMPTY;
        diminuto_tree_t node1 = DIMINUTO_TREE_NULLINIT;
        diminuto_tree_t node2 = DIMINUTO_TREE_NULLINIT;
        diminuto_tree_t node3 = DIMINUTO_TREE_NULLINIT;
        diminuto_tree_t node4 = DIMINUTO_TREE_NULLINIT;
        ASSERT(diminuto_tree_remove(DIMINUTO_TREE_NULL) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_remove(&node1) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_left_or_root(&node1, DIMINUTO_TREE_NULL, &root) == &node1);
        ASSERT(diminuto_tree_replace(&node2, &node2) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_replace(&node2, &node1) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_replace(&node1, &node1) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_left_or_root(&node1, DIMINUTO_TREE_NULL, &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_right_or_root(&node1, DIMINUTO_TREE_NULL, &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_left_or_root(&node2, DIMINUTO_TREE_NULL, &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_right_or_root(&node2, DIMINUTO_TREE_NULL, &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_left_or_root(&node2, &node1, &root) == &node2);
        ASSERT(diminuto_tree_insert_right_or_root(&node3, &node1, &root) == &node3);
        ASSERT(diminuto_tree_insert_left_or_root(&node4, &node1, &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_insert_right_or_root(&node4, &node1, &root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_next(&node4) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_prev(&node4) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node4) == DIMINUTO_TREE_ORPHAN);
    }

    {
        size_t ii;
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            ASSERT(diminuto_tree_parent(&(ALPHABET[ii])) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(&(ALPHABET[ii])) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(&(ALPHABET[ii])) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(&(ALPHABET[ii])) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(&(ALPHABET[ii])) != (void *)0);
        }
    }

    {
        size_t ii;
        for (ii = 0; ii < countof(ALFABIT); ++ii) {
            ASSERT(diminuto_tree_parent(&(ALFABIT[ii])) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(&(ALFABIT[ii])) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(&(ALFABIT[ii])) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(&(ALFABIT[ii])) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(&(ALFABIT[ii])) != (void *)0);
        }
    }

    {
        size_t ii;
        ASSERT(countof(ALPHABET) == countof(ALFABIT));
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            ASSERT(strcmp((const char *)ALPHABET[ii].data, (const char *)ALFABIT[ii].data) == 0);
        }
    }

    {
        diminuto_tree_t node = DIMINUTO_TREE_NULLINIT;
        diminuto_tree_log(DIMINUTO_TREE_NULL);
        diminuto_tree_log(&node);
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        node.error = !0;
        node.parent = (diminuto_tree_t *)0x11111111;
        node.left = (diminuto_tree_t *)0x22222222;
        node.right = (diminuto_tree_t *)0x33333333;
        node.root = (diminuto_tree_t **)0x44444444;
        node.data = (diminuto_tree_t *)0x55555555;
        diminuto_tree_log(&node);
    }

    {
        diminuto_tree_t * root = DIMINUTO_TREE_EMPTY;
        diminuto_tree_t * parentp = DIMINUTO_TREE_NULL;
        diminuto_tree_t * nodep;
        diminuto_tree_t * nextp;
        ssize_t ii;
        ssize_t jj;
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_first(&root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_last(&root) == DIMINUTO_TREE_NULL);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[ii]);
            ASSERT(diminuto_tree_init(nodep) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nodep->data) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_insert_right_or_root(nodep, parentp, &root) == nodep);
            ASSERT(find(&root, nodep->data) == nodep);
            parentp = nodep;
            ASSERT(!diminuto_tree_isempty(&root));
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
            nextp = diminuto_tree_first(&root);
            for (jj = 0; jj <= ii; ++jj) {
                 ASSERT(nextp == &(ALPHABET[jj]));
                 nextp = diminuto_tree_next(nextp);
            }
            ASSERT((nextp = diminuto_tree_last(&root)) == nodep);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        list(__LINE__, &root, dumps);
        dump(__LINE__, &root, dumps);
        traverse(__LINE__, &root, dumps);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[ii]);
            nextp = &(ALFABIT[ii]);
            ASSERT(find(&root, nextp->data) == nodep);
            ASSERT(diminuto_tree_replace(nextp, nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_replace(nodep, nextp) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nextp->data) == nextp);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        for (ii = 0; ii < countof(ALFABIT); ++ii) {
            nodep = &(ALFABIT[ii]);
            ASSERT(find(&root, nodep->data) == nodep);
            ASSERT(diminuto_tree_remove(nodep) == nodep);
            ASSERT(find(&root, nodep->data) == DIMINUTO_TREE_NULL);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
    }

    {
        diminuto_tree_t * root = DIMINUTO_TREE_EMPTY;
        diminuto_tree_t * parentp = DIMINUTO_TREE_NULL;
        diminuto_tree_t * nodep;
        diminuto_tree_t * prevp;
        ssize_t ii;
        ssize_t jj;
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_first(&root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_last(&root) == DIMINUTO_TREE_NULL);
        for (ii = countof(ALPHABET) - 1; ii >= 0; --ii) {
            nodep = &(ALPHABET[ii]);
            ASSERT(diminuto_tree_init(nodep) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nodep->data) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_insert_left_or_root(nodep, parentp, &root) == nodep);
            ASSERT(find(&root, nodep->data) == nodep);
            parentp = nodep;
            ASSERT(!diminuto_tree_isempty(&root));
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
            prevp = diminuto_tree_last(&root);
            for (jj = countof(ALPHABET) - 1; jj >= ii; --jj) {
                 ASSERT(prevp == &(ALPHABET[jj]));
                 prevp = diminuto_tree_prev(prevp);
            }
            ASSERT((prevp = diminuto_tree_first(&root)) == nodep);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        list(__LINE__, &root, dumps);
        dump(__LINE__, &root, dumps);
        traverse(__LINE__, &root, dumps);
        for (ii = countof(ALPHABET) - 1; ii >= 0; --ii) {
            nodep = &(ALPHABET[ii]);
            prevp = &(ALFABIT[ii]);
            ASSERT(find(&root, prevp->data) == nodep);
            ASSERT(diminuto_tree_replace(prevp, nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_replace(nodep, prevp) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, prevp->data) == prevp);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        for (ii = countof(ALFABIT) - 1; ii >= 0; --ii) {
            nodep = &(ALFABIT[ii]);
            ASSERT(find(&root, nodep->data) == nodep);
            ASSERT(diminuto_tree_remove(nodep) == nodep);
            ASSERT(find(&root, nodep->data) == DIMINUTO_TREE_NULL);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
    }

    {
        char biasedindices[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        diminuto_tree_t * root = DIMINUTO_TREE_EMPTY;
        diminuto_tree_t * nodep;
        diminuto_tree_t * nextp;
        diminuto_tree_t * gonep;
        ssize_t ii;
        int rc;
        printf("biasedindices[%zu]=\"%s\"\n", sizeof(biasedindices), biasedindices);
        strfry(biasedindices);
        printf("biasedindices[%zu]=\"%s\"\n", sizeof(biasedindices), biasedindices);
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_isempty(&root));
        ASSERT(diminuto_tree_first(&root) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_last(&root) == DIMINUTO_TREE_NULL);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[biasedindices[ii] - 'A']);
            ASSERT(diminuto_tree_init(nodep) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nodep->data) == DIMINUTO_TREE_NULL);
            gonep = diminuto_tree_search_insert(&root, nodep, comparator);
            ASSERT(gonep == nodep);
            ASSERT(find(&root, nodep->data) == nodep);
            ASSERT(!diminuto_tree_isempty(&root));
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        list(__LINE__, &root, dumps);
        dump(__LINE__, &root, dumps);
        traverse(__LINE__, &root, dumps);
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        strfry(biasedindices);
        printf("biasedindices[%zu]=\"%s\"\n", sizeof(biasedindices), biasedindices);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[biasedindices[ii] - 'A']);
            nextp = &(ALFABIT[biasedindices[ii] - 'A']);
            gonep = diminuto_tree_search_replace(&root, nextp, comparator);
            ASSERT(gonep == nodep);
            ASSERT(find(&root, nextp->data) == nextp);
            ASSERT(diminuto_tree_parent(gonep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(gonep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(gonep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(gonep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(gonep) != (void *)0);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        list(__LINE__, &root, dumps);
        dump(__LINE__, &root, dumps);
        traverse(__LINE__, &root, dumps);
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        strfry(biasedindices);
        printf("biasedindices[%zu]=\"%s\"\n", sizeof(biasedindices), biasedindices);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALFABIT[biasedindices[ii] - 'A']);
            nextp = &(ALPHABET[biasedindices[ii] - 'A']);
            gonep = diminuto_tree_search_insert(&root, nextp, comparator);
            ASSERT(gonep == (diminuto_tree_t *)0);
            ASSERT(find(&root, nodep->data) == nodep);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
        list(__LINE__, &root, dumps);
        dump(__LINE__, &root, dumps);
        traverse(__LINE__, &root, dumps);
        strfry(biasedindices);
        printf("biasedindices[%zu]=\"%s\"\n", sizeof(biasedindices), biasedindices);
        for (ii = 0; ii < countof(ALFABIT); ++ii) {
            nodep = &(ALFABIT[biasedindices[ii] - 'A']);
            ASSERT((nextp = find(&root, nodep->data)) == nodep);
            ASSERT((gonep = diminuto_tree_remove(nodep)) == nodep);
            ASSERT((nextp = find(&root, nodep->data)) == DIMINUTO_TREE_NULL);
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        }
    }

    EXIT();
}
