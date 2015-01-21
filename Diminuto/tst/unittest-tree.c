/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_tree.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_comparator.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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
    printf("node=%p color=%s parent=%p left=%p right=%p root=%p ", nodep, nodep->color ? "black": "red", nodep->parent, nodep->left, nodep->right, nodep->root);
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
    printf("dump: line=%d root=%p\n", line, rootp);
    if (!diminuto_tree_isempty(rootp)) {
        dumpr(1, 0, *rootp, dumpfp);
    }
}

static void list(int line, diminuto_tree_t ** rootp, dumpf_t * dumpfp)
{
    diminuto_tree_t * nodep;
    printf("list: line=%d root=%p\n", line, rootp);
    for (nodep = diminuto_tree_first(rootp); nodep != DIMINUTO_TREE_NULL; nodep = diminuto_tree_next(nodep)) {
        printf("list: ");
        dumpn(nodep, dumpfp);
    }
}

static diminuto_tree_t * audit(int line, diminuto_tree_t ** rootp, dumpf_t * dumpfp)
{
    diminuto_tree_t * nodep;
    nodep = diminuto_tree_audit(rootp);
    if (nodep != DIMINUTO_TREE_NULL) {
        list(line, rootp, dumpfp);
        dump(line, rootp, dumpfp);
    }
    return nodep;
}

static diminuto_tree_t * findr(diminuto_tree_t * nodep, void * key, diminuto_comparator_t * comparefp)
{
    int rc;
    if (diminuto_tree_isleaf(nodep)) {
        return DIMINUTO_TREE_NULL;
    } else {
        rc = (*comparefp)(key, nodep->data);
        if (rc < 0) {
            return findr(nodep->left, key, comparefp);
        } else if (rc > 0) {
            return findr(nodep->right, key, comparefp);
        } else {
            return nodep;
        }
    }
}

static diminuto_tree_t * find(diminuto_tree_t ** rootp, void * key, diminuto_comparator_t * comparefp)
{
    return findr(*rootp, key, comparefp);
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
        ASSERT(diminuto_tree_parent(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_left(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_right(&node) == DIMINUTO_TREE_NULL);
        ASSERT(diminuto_tree_root(&node) == DIMINUTO_TREE_ORPHAN);
        node.color = DIMINUTO_TREE_COLOR_BLACK;
        node.parent = (diminuto_tree_t *)0x11111111;
        node.left = (diminuto_tree_t *)0x22222222;
        node.right = (diminuto_tree_t *)0x33333333;
        node.root = (diminuto_tree_t **)0x44444444;
        ASSERT(!diminuto_tree_isred(&node));
        ASSERT(diminuto_tree_isblack(&node));
        ASSERT(diminuto_tree_parent(&node) == (diminuto_tree_t *)0x11111111);
        ASSERT(diminuto_tree_left(&node) == (diminuto_tree_t *)0x22222222);
        ASSERT(diminuto_tree_right(&node) == (diminuto_tree_t *)0x33333333);
        ASSERT(diminuto_tree_root(&node) == (diminuto_tree_t **)0x44444444);
        diminuto_tree_dataset(&node, (void *)0x55555555);
        ASSERT(diminuto_tree_data(&node) == (void *)0x55555555);
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
            diminuto_tree_init(nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == DIMINUTO_TREE_NULL);
            diminuto_tree_insert_right_or_root(nodep, parentp, &root);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == nodep);
            parentp = nodep;
            ASSERT(!diminuto_tree_isempty(&root));
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
            nextp = diminuto_tree_first(&root);
            for (jj = 0; jj <= ii; ++jj) {
                 ASSERT(nextp == &(ALPHABET[jj]));
                 nextp = diminuto_tree_next(nextp);
            }
            nextp = diminuto_tree_last(&root);
            ASSERT(nextp == nodep);
        }
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        for (ii = 0; ii < countof(ALPHABET); ++ii) {
            nodep = &(ALPHABET[ii]);
            nextp = &(ALFABIT[ii]);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == nodep);
            ASSERT(diminuto_tree_replace(nextp, nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_replace(nodep, nextp) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nextp->data, diminuto_compare_strings) == nextp);
        }
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        for (ii = 0; ii < countof(ALFABIT); ++ii) {
            nodep = &(ALFABIT[ii]);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == nodep);
            ASSERT(diminuto_tree_remove(nodep) == nodep);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == DIMINUTO_TREE_NULL);
        }
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
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
            diminuto_tree_init(nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == DIMINUTO_TREE_NULL);
            diminuto_tree_insert_left_or_root(nodep, parentp, &root);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == nodep);
            parentp = nodep;
            ASSERT(!diminuto_tree_isempty(&root));
            ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
            prevp = diminuto_tree_last(&root);
            for (jj = countof(ALPHABET) - 1; jj >= ii; --jj) {
                 ASSERT(prevp == &(ALPHABET[jj]));
                 prevp = diminuto_tree_prev(prevp);
            }
            prevp = diminuto_tree_first(&root);
            ASSERT(prevp == nodep);
        }
        list(__LINE__, &root, dumps);
        dump(__LINE__, &root, dumps);
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        for (ii = countof(ALPHABET) - 1; ii >= 0; --ii) {
            nodep = &(ALPHABET[ii]);
            prevp = &(ALFABIT[ii]);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == nodep);
            ASSERT(diminuto_tree_replace(prevp, nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_replace(nodep, prevp) == nodep);
            ASSERT(diminuto_tree_parent(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_left(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_right(nodep) == DIMINUTO_TREE_NULL);
            ASSERT(diminuto_tree_root(nodep) == DIMINUTO_TREE_ORPHAN);
            ASSERT(diminuto_tree_data(nodep) != (void *)0);
            ASSERT(find(&root, prevp->data, diminuto_compare_strings) == prevp);
        }
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
        for (ii = countof(ALFABIT) - 1; ii >= 0; --ii) {
            nodep = &(ALFABIT[ii]);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == nodep);
            ASSERT(diminuto_tree_remove(nodep) == nodep);
            ASSERT(find(&root, nodep->data, diminuto_compare_strings) == DIMINUTO_TREE_NULL);
        }
        ASSERT(audit(__LINE__, &root, dumps) == DIMINUTO_TREE_NULL);
    }

    EXIT();
}
