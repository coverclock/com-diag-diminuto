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
 * Hence, a single list node all by itself constitutes a circular
 * doubly-linked list. This is true whether the list node is newly
 * constructed, has never been inserted onto a list, or has been
 * removed from a list.
 *
 * The circularity is a simplification, not a complication, as
 * it eliminates all the special cases present with NULL-terminated
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
 * list node). There is never a NULL moment with the next, previous
 * or root pointers of a list node.
 *
 * Each list node also has a data pointer. The data pointer is a
 * void pointer. It is the only pointer in a list node that may be NULL.
 * The data pointer can be accessed and set by the application or optionally
 * when a list node is initialized.
 *
 * A list node can exist independently of the data object to which
 * its data pointer points (as the STL container classes do). But a
 * list node can also exist as an instance variable inside the data
 * object. This makes memory management of list nodes much simpler, and
 * also couples the destruction of the list node to the destruction of the
 * data object. Furthermore, a data object may contain more than
 * one list node instance variable, allowing it to be on more than one
 * list at a time.
 *
 * Caveats:
 *
 * Many (most) of these operations are implemented as macros which may
 * evaluate their argument(s) more than once.
 *
 * Inserting a list node onto a list when it is already on a list causes
 * the list node to be first removed from the list is is already on.
 *
 * Removing a list node from a list when it is not on a list has no effect.
 *
 * A node that is the root of its list cannot be removed from its list.
 * This is because removing such a node would orphan all of the root pointers
 * in all of the other nodes on the same list. Attemping to do so has no
 * effect.
 *
 * Doubly-linked lists can be used to implement stacks (Last-In, First-Out
 * or LIFO) or queues (First-In, First-Out or FIFO), in which case NULL
 * pointers are returned to indicate that the container is empty. These
 * NULL pointers are not stored in the container but generated by the
 * interface.
 */

struct DiminutoList;

typedef struct DiminutoList diminuto_list_t;

struct DiminutoList {
    diminuto_list_t * next;   /* Points to the next node on the list. */
    diminuto_list_t * prev;   /* Points to the previous node on the list. */
    diminuto_list_t * root;   /* Points to the root node of the list. */
    void * data;            /* Points to the payload of the node. */
};

/**
 * @def diminuto_list_next(_NODEP_)
 * Return the next node pointer on the node pointed to by @a _NODEP_.
 */
#define diminuto_list_next(_NODEP_) \
    ((_NODEP_)->next)

/**
 * @def diminuto_list_prev(_NODEP_)
 * Return a the previous node pointer on the node pointed to by @a _NODEP_.
 */
#define diminuto_list_prev(_NODEP_) \
    ((_NODEP_)->prev)

/**
 * @def diminuto_list_root(_NODEP_)
 * Return the root node pointer on the node pointed to by @a _NODEP_.
 */
#define diminuto_list_root(_NODEP_) \
    ((_NODEP_)->root)

/**
 * @def diminuto_list_data(_NODEP_)
 * Return the data pointer on the node pointed to by @a _NODEP_.
 */
#define diminuto_list_data(_NODEP_) \
    ((_NODEP_)->data)

/**
 * @def diminuto_list_dataset(_NODEP_, _DATAP_)
 * Set the data pointer on the node pointed to by @a _NODEP_ to @a _DATAP_.
 * Returns the data pointer.
 */
#define diminuto_list_dataset(_NODEP_, _DATAP_) \
    ((_NODEP_)->data = (void *)(_DATAP_))

/**
 * @def diminuto_list_isempty(_NODEP_)
 * Returns true if the node pointed to by @a _NODEP_ has no other
 * nodes linked to it.
 */
#define diminuto_list_isempty(_NODEP_) \
    ((_NODEP_) == diminuto_list_next(_NODEP_))

/**
 * @def diminuto_list_isroot(_NODEP_)
 * Returns true if the node pointed to by @a _NODEP_ is the root
 * of the list it is on.
 */
#define diminuto_list_isroot(_NODEP_) \
    ((_NODEP_) == diminuto_list_root(_NODEP_))

/**
 * @def diminuto_list_ismember(_ROOTP_, _NODEP_)
 * Returns true if the node pointed to by @a _NODEP_ is on a list
 * for which the node pointed to by @a _ROOTP_ is the root node.
 */
#define diminuto_list_ismember(_ROOTP_, _NODEP_) \
    ((_ROOTP_) == diminuto_list_root(_NODEP_))

/**
 * Initialize the specified node. The node is left initialized as an
 * empty list with itself as its root. The data pointer is not initialized.
 * @param nodep points to the node to be initialized.
 * @return a pointer to the initialized node.
 */
extern diminuto_list_t * diminuto_list_init(
    diminuto_list_t * nodep
);

/**
 * @def DIMINUTO_LIST_INIT
 * Generate a storage initializer for the node @a _NODEP_.
 */
#define DIMINUTO_LIST_INIT(_NODEP_) \
    { _NODEP_, _NODEP_, _NODEP_, (void *)0 }


/**
 * @def DIMINUTO_LIST_DATAINIT
 * Generate a storage initializer for the node @a _NODEP and data pointer
 * @a _DATAP_.
 */
#define DIMINUTO_LIST_DATAINIT(_NODEP_, _DATAP_) \
    { _NODEP_, _NODEP_, _NODEP_, _DATAP_ }

/**
 * @def diminuto_list_datainit(_NODEP_, _DATAP_)
 * Initialize the node pointed to by @a _NODEP_ and set its data pointer
 * to the address @a _DATAP_. Returns the pointer to the node.
 */
#define diminuto_list_datainit(_NODEP_, _DATAP_) \
    (diminuto_list_dataset(diminuto_list_init(_NODEP_), _DATAP_), _NODEP_)

/**
 * @def diminuto_list_nullinit(_NODEP_)
 * Initialize the node pointed to by @a _NODEP_ and set its data pointer
 * to NULL. Returns the pointer to the node.
 */
#define diminuto_list_nullinit(_NODEP_) \
    (diminuto_list_dataset(diminuto_list_init(_NODEP_), (void *)0), _NODEP_)

/**
 * Remove the specified node from the list it is on. The node is
 * reinitialized. The data pointer is left unaltered. If the node is
 * not on a list, nothing is done.
 * @param nodep points to the node to be removed.
 * @return a pointer to the removed node.
 */
extern diminuto_list_t * diminuto_list_remove(
    diminuto_list_t * nodep
);

/**
 * Insert the specified node into list just after the specified root.
 * If the node is already on a list, it is first removed from that
 * list before being inserted on the specified root.
 * @param rootp points to the root on which the node is inserted.
 * @param nodep points to the node to be inserted.
 * @return a pointer to the inserted node.
 */
extern diminuto_list_t * diminuto_list_insert(
    diminuto_list_t * rootp,
    diminuto_list_t * nodep
);

/**
 * Make the specified node the root of all the other nodes that are
 * on the list that the specified node is on.
 * @param nodep points to the node to become the root of the list.
 * @return a pointer to the new root node.
 */
extern diminuto_list_t * diminuto_list_reroot(
    diminuto_list_t * nodep
);

typedef int (diminuto_list_functor)(
    void * datap,
    void * contextp
);

/**
 * Apply the specified functor to successive nodes. If the functor
 * returns a positive number, the functor is then applied to the next
 * node. If the functor returns a negative number, the functor is then
 * applied to the previous node. If the functor returns zero, a pointer
 * to the current node is returned. A pointer to the the data of the current
 * node and a pointer to a caller supplied context pointer is passed
 * to the functor each time it is called. If the functor needs to treat
 * the root node specially, it must recognize it solely from its data
 * pointer.
 * @param funcp points to the functor.
 * @param nodep points to the initial node.
 * @param contextp points to the caller provided context or null.
 * @return a pointer to the node for which the functor returns zero.
 */
extern diminuto_list_t * diminuto_list_apply(
    diminuto_list_functor * funcp,
    diminuto_list_t * nodep,
    void * contextp
);

/**
 * @def diminuto_list_dataif(_NODEP_)
 * Return the data pointer on the node pointer to by @a _NODEP_ if
 * @a _NODEP_ is non-NULL, otherwise return NULL.
 */
#define diminuto_list_dataif(_NODEP_) \
    (((_NODEP_) != (diminuto_list_t *)0) ? ((_NODEP_)->data) : (void *)0)

/**
 * @def diminuto_list_head(_ROOTP_)
 * Return a pointer to the first node on the list that is the root
 * of the node pointed to by @a _ROOTP_, or NULL if the list is empty.
 * The list is not modified.
 */
#define diminuto_list_head(_ROOTP_) \
    (diminuto_list_isempty(diminuto_list_root(_ROOTP_)) \
        ? (diminuto_list_t *)0 \
        : diminuto_list_next(diminuto_list_root(_ROOTP_)))

/**
 * @def diminuto_list_tail(_ROOTP_)
 * Return a pointer to the last node on the list that is the root
 * of the node pointed to by @a _ROOTP_, or NULL if the list is empty.
 * The list is not modified.
 */
#define diminuto_list_tail(_ROOTP_) \
    (diminuto_list_isempty(diminuto_list_root(_ROOTP_)) \
        ? (diminuto_list_t *)0 \
        : diminuto_list_prev(diminuto_list_root(_ROOTP_)))

/**
 * @def diminuto_list_push(_ROOTP_, _NODEP_)
 * Push the node pointed to by @a _NODEP_ on to the top of the stack that
 * is the root of the node pointed to by @a _ROOTP_. Return _NODEP_.
 */
#define diminuto_list_push(_ROOTP_, _NODEP_) \
    diminuto_list_insert(diminuto_list_root(_ROOTP_), _NODEP_)

/**
 * @def diminuto_list_pop(_ROOTP_)
 * Pop the node on the top of the stack that is the root of the node pointed
 * to by @a _ROOTP_ and return a pointer to it, or NULL 
 */
#define diminuto_list_pop(_ROOTP_) \
    (diminuto_list_isempty(diminuto_list_root(_ROOTP_)) \
        ? (diminuto_list_t *)0 \
        : diminuto_list_remove(diminuto_list_next(diminuto_list_root(_ROOTP_))))

/**
 * @def diminuto_list_enqueue(_ROOTP_, _NODEP_)
 * Enqueue the node pointed to by @a _NODEP_ on to the end of the queue that
 * is the root of the node pointed to by @a _ROOTP_. Return _NODEP_.
 */
#define diminuto_list_enqueue(_ROOTP_, _NODEP_) \
    diminuto_list_insert(diminuto_list_prev(diminuto_list_root(_ROOTP_)), _NODEP_)

/**
 * @def diminuto_list_dequeue(_ROOTP_)
 * Dequeue the node from the beginning of the queue that is the root of the
 * node pointed to by @a _ROOTP_ and return a pointer to it, or NULL 
 */
#define diminuto_list_dequeue(_ROOTP_) \
    (diminuto_list_isempty(diminuto_list_root(_ROOTP_)) \
        ? (diminuto_list_t *)0 \
        : diminuto_list_remove(diminuto_list_next(diminuto_list_root(_ROOTP_))))

/**
 * @def diminuto_list_replace(_OLDP_, _NEWP_)
 * Replace the node @a _OLDP_ with the node @a _NEWP_. If @a _OLDP_ is the
 * root of its list, @a _NEWP_ is now the root of its list.
 */
#define diminuto_list_replace(_OLDP_, _NEWP_) \
    (diminuto_list_insert(_OLDP_, _NEWP_), \
        (diminuto_list_isroot(_OLDP_) \
            ? diminuto_list_reroot(_NEWP_) \
            : _NEWP_), \
        diminuto_list_remove(_OLDP_))

#endif
