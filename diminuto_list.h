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
 *  Implements an object which can be inserted into and removed from a
 *  circular doubly-linked list, or list, in which insertions and
 *  removals can be done in O(1) time, the head of the list can be
 *  accessed in O(1) time, and a payload object can be on multiple lists
 *  simultaneously.
 *
 *  Each list node has a pointer to the next node on the list, and
 *  another pointer to the previous list node on the list. For a
 *  list node which is not on a list, both the next and previous
 *  pointers of the list node point to the list node object itself.
 *  Hence, a single list link all by itself constitutes a circular
 *  doubly-linked list. This is true whether the list node is newly
 *  constructed, has never been inserted onto a list, or has been
 *  removed from a list.
 *
 *  The circularity is a simplification, not a complication, as
 *  it eliminates all the special cases present with null-terminated
 *  doubly-linked lists. The next and previous pointers allow any list
 *  node to be inserted into or removed from any arbitrary position in
 *  the list in O(1) time.
 *
 *  Each list node also has a pointer to its root. When a list node
 *  is inserted onto a list, it is always inserted after an existing
 *  list node that is already on the list. When this is done, the
 *  inserted list node inherits the root pointer of the list node
 *  that is already on the list. For a list node which is not on a
 *  list, the root pointer of the list node points to the list node
 *  object itself. This is true whether the list node is newly constructed,
 *  has never been inserted onto a list, or has been removed from a list.
 *
 *  Using a list node as the head or root of the list, as additional
 *  list nodes are inserted onto the list, the initial root pointer
 *  is propagated into each inserted list node. The root can be
 *  accessed from any list node on the list. The root pointer allows
 *  the list membership of any arbirary list node to be determined in O(1)
 *  time.
 *
 *  Accessing the next, previous or root pointers of a list node
 *  always returns a valid pointer to a list node (possibly the same
 *  list node). There is never a null moment with the next, previous
 *  or root pointers of a list node.
 *
 *  Each list node also has a payload pointer. The payload pointer is a
 *  void* pointer. It is the only pointer in a list node
 *  that is initialized to null when a list node is constructed. The
 *  payload pointer can be accessed and set by the application.
 *  No other list node operation references the payload pointer.
 *
 *  A list node can exist independently of the payload object to which
 *  its payload pointer points (as the STL container classes do). But a
 *  list node can also exist as an instance variable inside the payload
 *  object. This makes memory management of list nodes much simpler, and
 *  also couples the destruction of the list node to the destruction of the
 *  payload object. Furthermore, a payload object may contain more than
 *  one list node instance variable, allowing it to be on more than one
 *  list at a time. This class can also be used via inheritance rather
 *  than by composition, but this is strongly discouraged.
 *
 *  Upon destruction, a list node is removed from any list it may
 *  be on. This prevents any dangling pointers from hanging around
 *  inside list nodes on a list. The exception to this is the
 *  destruction of the list node that is the root of a list.
 *  Destroying a list node that is the root of a list will leave
 *  an intact list in which each list node has valid next and
 *  previous pointers, but with a root pointer pointing to the
 *  destroyed list node.
 *
 *  It is an error to try to insert a list node that is already on a
 *  list. It is an error to try to remove a list node that is not on
 *  a list. The caller should check the return code on insert and remove
 *  operations to detect these errors.
 *
 *  This class contains no virtual functions so that a virtual table pointer
 *  is not allocated as part of the object. This doesn't sound so important,
 *  but in an application in which thousands of linked objects may be
 *  allocated, a single four-byte pointer makes a difference. Links already
 *  have enough overhead that for some applications their use will be
 *  problematic.
 *
 *  This class is not thread-safe. Serialization is the responsibility
 *  of the caller. Adding a thread safe wrapper using the appropriate
 *  native synchronization mechanism of the underlying platform (and as
 *  an aside, giving the payload pointer a specific type definition), seems
 *  like a good idea.
 *
 *  Thanks to the author(s) of the Linux kernel in which the doubly-linked
 *  list code inspired this design.
 *
 *  @see    T. Aivanzian, <I>Linux Kernel 2.4 Internals</I>,
 *          August 2001, p. 19-21
 */

struct diminuto_list;

typedef struct diminuto_list diminuto_list;

struct diminuto_list {
    diminuto_list * next;
    diminuto_list * prev;
    diminuto_list * root;
    void * data;
};

typedef int (*diminuto_list_functor)(
    diminuto_list * listp,
    diminuto_list * herep,
    void * contextp
);

#define diminuto_list_next(_LISTP_) \
    ((_LISTP_)->next)

#define diminuto_list_prev(_LISTP_) \
    ((_LISTP_)->prev)

#define diminuto_list_root(_LISTP_) \
    ((_LISTP_)->root)

#define diminuto_list_data(_LISTP_) \
    ((_LISTP_)->data)

#define diminuto_list_isempty(_LISTP_) \
    (((_LISTP_) == (_LISTP_)->next) && ((_LISTP_) == (_LISTP_)->prev))

#define diminuto_list_isroot(_LISTP_, _ROOTP_) \
    ((_ROOTP_) == (_LISTP_)->root)

extern diminuto_list * diminuto_list_init(diminuto_list * nodep);

extern diminuto_list * diminuto_list_remove(diminuto_list * nodep);

extern diminuto_list * diminuto_list_insert(diminuto_list * listp, diminuto_list * nodep);

extern diminuto_list * diminuto_list_apply(diminuto_list * listp, diminuto_list_functor * funcp, void * contextp);

#define diminuto_list_peek(_LISTP_) \
    diminuto_list_next(_LISTP_)

#define diminuto_list_push(_LISTP_, _NODEP_) \
    diminuto_list_insert(_LISTP_, _NODEP_)

#define diminuto_list_pop(_LISTP_) \
    diminuto_list_remove(diminuto_list_next(_LISTP_))

#define diminuto_list_get(_LISTP_) \
    diminuto_list_remove(diminuto_list_next(_LISTP_))

#define diminuto_list_put(_LISTP_, _NODEP_) \
    diminuto_list_insert(diminuto_list_prev(_LISTP_), _NODEP_)

#endif
