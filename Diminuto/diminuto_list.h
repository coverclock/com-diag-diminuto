/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LIST_
#define _H_COM_DIAG_DIMINUTO_LIST_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implements an object which can be inserted into and removed from a
 * circular doubly-linked list, in which insertions and
 * removals can be done in O(1) time, the head of the list can be
 * accessed in O(1) time, and a payload object can be on multiple lists
 * simultaneously.
 *
 * Each list node has a pointer to the next node on the list, and
 * another pointer to the previous list node on the list. For a
 * list node which is not on a list, both the next and previous
 * pointers of the list node point to the list node object itself.
 * Hence, a single list link all by itself constitutes a circular
 * doubly-linked list. This is true whether the list node is newly
 * constructed, has never been inserted onto a list, or has been
 * removed from a list.
 *
 * The circularity is a simplification, not a complication, as
 * it eliminates all the special cases present with null-terminated
 * doubly-linked lists. The next and previous pointers allow any list
 * node to be inserted into or removed from any arbitrary position in
 * the list in O(1) time.
 *
 * Each list node also has a pointer to its root. When a list node
 * is inserted onto a list, it is always inserted after an existing
 * list node that is already on the list. When this is done, the
 * inserted list node inherits the root pointer of the list node
 * that is already on the list. For a list node which is not on a
 * list, the root pointer of the list node points to the list node
 * object itself. This is true whether the list node is newly constructed,
 * has never been inserted onto a list, or has been removed from a list.
 *
 * Using a list node as the head or root of the list, as additional
 * list nodes are inserted onto the list, the initial root pointer
 * is propagated into each inserted list node. The root can be
 * accessed from any list node on the list. The root pointer allows
 * the list membership of any arbirary list node to be determined in O(1)
 * time.
 *
 * Accessing the next, previous or root pointers of a list node
 * always returns a valid pointer to a list node (possibly the same
 * list node). There is never a null moment with the next, previous
 * or root pointers of a list node.
 *
 * Each list node also has a payload pointer. The payload pointer is a
 * void* pointer. It is the only pointer in a list node
 * that is initialized to null when a list node is constructed. The
 * payload pointer can be accessed and set by the application.
 * No other list node operation references the payload pointer.
 *
 * A list node can exist independently of the payload object to which
 * its payload pointer points (as the STL container classes do). But a
 * list node can also exist as an instance variable inside the payload
 * object. This makes memory management of list nodes much simpler, and
 * also couples the destruction of the list node to the destruction of the
 * payload object. Furthermore, a payload object may contain more than
 * one list node instance variable, allowing it to be on more than one
 * list at a time. This class can also be used via inheritance rather
 * than by composition, but this is strongly discouraged.
 *
 * Inserting a list node onto a list when it is already on a list causes
 * the list node to be first removed from the list is is already on.
 *
 * Removing a list node from a list when it is not on a list has no effect.
 */

struct diminuto_list;

typedef struct diminuto_list diminuto_list;

struct diminuto_list {
    diminuto_list * next;   /* Points to the next node on the list. */
    diminuto_list * prev;   /* Points to the previous node on the list. */
    diminuto_list * root;   /* Points to the root node of the list. */
    void * data;            /* Points to the payload of the node. */
};

#define diminuto_list_next(_NODEP_) \
    ((_NODEP_)->next)

#define diminuto_list_prev(_NODEP_) \
    ((_NODEP_)->prev)

#define diminuto_list_root(_NODEP_) \
    ((_NODEP_)->root)

#define diminuto_list_data(_NODEP_) \
    ((_NODEP_)->data)

#define diminuto_list_isempty(_NODEP_) \
    ((_NODEP_) == (_NODEP_)->next)

#define diminuto_list_isroot(_NODEP_) \
    ((_NODEP_) == (_NODEP_)->root)

#define diminuto_list_ismember(_ROOTP_, _NODEP_) \
    ((_ROOTP_) == (_NODEP_)->root)

extern diminuto_list * diminuto_list_init(
    diminuto_list * nodep
);

extern diminuto_list * diminuto_list_remove(
    diminuto_list * nodep
);

extern diminuto_list * diminuto_list_insert(
    diminuto_list * rootp,
    diminuto_list * nodep
);

typedef int (*diminuto_list_functor)(
    diminuto_list * nodep,
    void * contextp
);

extern diminuto_list * diminuto_list_apply(
    diminuto_list_functor * funcp,
    diminuto_list * nodep,
    void * contextp
);

#define diminuto_list_head(_ROOTP_) \
    (diminuto_list_isempty(_ROOTP_) ? (diminuto_list *)0 : \
        diminuto_list_next(_ROOTP_))

#define diminuto_list_tail(_ROOTP_) \
    (diminuto_list_isempty(_ROOTP_) ? (diminuto_list *)0 : \
        diminuto_list_prev(_ROOTP_))

#define diminuto_list_push(_ROOTP_, _NODEP_) \
    diminuto_list_insert(_ROOTP_, _NODEP_)

#define diminuto_list_pop(_ROOTP_) \
    (diminuto_list_isempty(_ROOTP_) ? (diminuto_list *)0 : \
        diminuto_list_remove(diminuto_list_next(_ROOTP_)))

#define diminuto_list_enqueue(_ROOTP_, _NODEP_) \
    diminuto_list_insert(diminuto_list_prev(_ROOTP_), _NODEP_)

#define diminuto_list_dequeue(_ROOTP_) \
    (diminuto_list_isempty(_ROOTP_) ? (diminuto_list *)0 : \
        diminuto_list_remove(diminuto_list_next(_ROOTP_)))

#endif
