/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LIST_
#define _H_COM_DIAG_DIMINUTO_LIST_

/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
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
 * the list membership of any arbitrary list node to be determined, or the
 * root of any list determined, in O(1) time. This allows the list to be
 * manipulated as a queue (FIFO) or a stack (LIFO), implementing traditional
 * operations like head, tail, push, pop, enqueue, dequeue, using any node that
 * is on the list.
 *
 * Accessing the next, previous or root pointers of a list node
 * always returns a valid pointer to a list node (possibly the same
 * list node). There is never a null moment with a list.
 *
 * Each list node also has a data pointer. The data pointer is a
 * void pointer. It is the only pointer in a list node that may be null. The
 * data pointer can be accessed and set by the application or optionally
 * when a list node is initialized.
 *
 * A list node can exist independently of the data object to which
 * its data pointer points (as the STL container classes do). But a
 * list node can also exist as an instance variable inside the data
 * object, such that the payload pointer of the list node points to the
 * beginning of the data object. This makes memory management of list nodes
 * much simpler, and also couples the destruction of the list node to the
 * destruction of the data object. Furthermore, a data object may contain more
 * than one list node instance variable, allowing it to be on more than one
 * list at a time. The data object destructor can trivially remove the data
 * object from any lists it is on in O(1) time.
 *
 * Inserting a list node onto a list when it is already on a list causes
 * the list node to be first removed from the list is is already on.
 *
 * Removing a list node from a list when it is not on a list has no effect.
 *
 * A node that is the root of its list cannot be removed from its list.
 * This is because removing such a node would orphan all of the root pointers
 * in all of the other nodes on the same list. Attempting to do so has no
 * effect.
 *
 * Lists can be used to implement stacks (LIFO) or queues (FIFO), in which case
 * null pointers are returned to indicate that the container is empty. These
 * null pointers are not stored in the container itself but synthesized by the
 * API when appropriate.
 */

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This is what a Diminuto doubly-linked-list data structure looks like: a
 * pointer to the next node, a pointer to the previous node, a pointer to the
 * root of the list, and a pointer to the payload (which could be anything, even
 * a non-pointer, as long as it fits in sizeof(void *). The size and alignment
 * of this structure was deliberately designed to fit in a typical cache line.
 */
typedef struct DiminutoList {
	struct DiminutoList * next;	/* Points to the next node on the list. */
	struct DiminutoList * prev;	/* Points to the previous node on the list. */
	struct DiminutoList * root;	/* Points to the root node of the list. */
    void                * data;	/* Points to the payload of the node. */
} diminuto_list_t;

/**
 * A list functor is used to walk the list. The functor is first applied to
 * the whatever node the application specifies. If the functor returns a
 * number less than zero, it is then applied to the previous node. If the
 * functor returns a number greater than zero, it is then applied to the next
 * node. This proceeds until the functor returns zero.
 */
typedef int (diminuto_list_functor_t)(
    void * datap,
    void * contextp
);

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_LIST_DATAINIT
 * Generate a storage initializer for the node @a _NODEP_ and data pointer
 * @a _DATAP_.
 */
#define DIMINUTO_LIST_DATAINIT(_NODEP_, _DATAP_) \
    { _NODEP_, _NODEP_, _NODEP_, _DATAP_ }

/**
 * @def DIMINUTO_LIST_NULLINIT
 * Generate a storage initializer for the node @a _NODEP_.
 */
#define DIMINUTO_LIST_NULLINIT(_NODEP_) \
    DIMINUTO_LIST_DATAINIT(_NODEP_, (void *)0)

/*******************************************************************************
 * PRIMITIVE OPERATIONS
 ******************************************************************************/

/**
 * Remove the specified node from the list it is on. The node is
 * reinitialized. The data pointer is left unaltered. If the node is the root
 * of the list, the list is rerooted to the next node. If the node is
 * not on a list, this operation has no effect.
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
    diminuto_list_t * queuep,
    diminuto_list_t * nodep
);

/**
 * Make the specified node the root of all the other nodes that are
 * on the list that the node is on.
 * @param nodep points to the node to become the root of the list.
 * @return a pointer to the new root node.
 */
extern diminuto_list_t * diminuto_list_reroot(
    diminuto_list_t * nodep
);

/**
 * Replace an old node with a new node on the same relative position on the
 * list. If the old node was the root of the list, the new node becomes the
 * root of the list.
 * @param oldp points to the old node.
 * @param newp points to the new node.
 * return a pointer to the old node.
 */
extern diminuto_list_t * diminuto_list_replace(
	diminuto_list_t * oldp,
	diminuto_list_t * newp
);

extern diminuto_list_t * diminuto_list_cut(
    diminuto_list_t * firstp,
    diminuto_list_t * secondp
);

extern diminuto_list_t * diminuto_list_splice(
    diminuto_list_t * top,
    diminuto_list_t * fromp
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
 * pointer. A common approach is to set the data pointer of the root node
 * to null.
 * @param funcp points to the functor.
 * @param nodep points to the initial node.
 * @param contextp points to the caller provided context or null.
 * @return a pointer to the node for which the functor returns zero.
 */
extern diminuto_list_t * diminuto_list_apply(
	diminuto_list_functor_t * funcp,
    diminuto_list_t * nodep,
    void * contextp
);

/**
 * Audit a list. Return a pointer to the first node on the list that appears
 * to be incorrect, or NULL if the list appears correct. Next, previous, and
 * root pointers are checked. The list is traversed bi-directionally and only
 * stops when an error is found or the traversals meet back at the original
 * node.
 * @param nodep points to a node on the list.
 * @return a pointer to the first incorrect node or null if none.
 */
extern diminuto_list_t * diminuto_list_audit(
	const diminuto_list_t * nodep
);

/*******************************************************************************
 * SETTORS
 ******************************************************************************/

/**
 * Initialize the specified node. The node is left initialized as an
 * empty list with itself as its root. The data pointer is not initialized.
 * @param nodep points to the node to be initialized.
 * @return a pointer to the initialized node.
 */
static inline diminuto_list_t * diminuto_list_init(
    diminuto_list_t * nodep
) {
    nodep->next = nodep;
    nodep->prev = nodep;
    nodep->root = nodep;
    return nodep;
}

/**
 * Set the data pointer to the data and return a pointer to the data.
 * @param nodep points to a node.
 * @param datap points to the data.
 * @return a pointer to the data.
 */
static inline void * diminuto_list_dataset(
	diminuto_list_t * nodep,
	void * datap
) {
	return nodep->data = datap;
}

/**
 * Initialize a node and set its data pointer.
 * @param nodep points to a node.
 * @param datap points to the data.
 * @return a pointer to the node.
 */
static inline diminuto_list_t * diminuto_list_datainit(
	diminuto_list_t * nodep,
	void * datap
)  {
	diminuto_list_dataset(diminuto_list_init(nodep), datap);
	return nodep;
}

/**
 * Initialize a node and set its data pointer to null.
 * @param nodep points to a node.
 * @return a pointer to the node.
 */
static inline diminuto_list_t * diminuto_list_nullinit(
	diminuto_list_t * nodep
) {
    return diminuto_list_datainit(nodep, (void *)0);
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return a pointer to the next node.
 * @param nodep points to a node.
 * @return a pointer to the next node.
 */
static inline diminuto_list_t * diminuto_list_next(
	const diminuto_list_t * nodep
) {
    return nodep->next;
}

/**
 * Return a pointer to the previous node.
 * @param nodep points to a node.
 * @return a pointer to to the previous node.
 */
static inline diminuto_list_t * diminuto_list_prev(
	const diminuto_list_t * nodep
) {
    return nodep->prev;
}

/**
 * Return a pointer to the root node.
 * @param nodep points to a node.
 * @return a pointer to the root node.
 */
static inline diminuto_list_t * diminuto_list_root(
	const diminuto_list_t * nodep
) {
    return nodep->root;
}

/**
 * Return a pointer to the data. The returned pointer may be null.
 * @param nodep points to a node.
 * @return a pointer to the data.
 */
static inline void * diminuto_list_data(
	const diminuto_list_t * nodep
) {
    return nodep->data;
}

/*******************************************************************************
 * QUERIES
 ******************************************************************************/

/**
 * Return true if the next node points to the node itself, indicating that
 * there are no other nodes on the list.
 * @param nodep points to a node.
 * @return true if the list is empty, false otherwise.
 */
static inline int diminuto_list_isempty(
	const diminuto_list_t * nodep
) {
    return (nodep == diminuto_list_next(nodep));
}

/**
 * Returns true if the node is its own root, indicating that it is the root
 * of the list.
 * @param nodep points to a node.
 * @return true if the node is the root, false otherwise.
 */
static inline int diminuto_list_isroot(
	const diminuto_list_t * nodep
) {
    return (nodep == diminuto_list_root(nodep));
}

/**
 * Returns true if a node is a member of a list.
 * @param rootp points to the list root node.
 * @param nodep points to the node.
 * @return true the node is a member of the list with the root, false otherwise.
 */
static inline int diminuto_list_ismember(
	const diminuto_list_t * rootp,
	const diminuto_list_t * nodep
) {
    return (rootp == diminuto_list_root(nodep));
}

/**
 * Returns true two nodes are members of the same list. This works even if one
 * or both of the nodes are root nodes.
 * @param firstp points to the first node.
 * @param secondp points to the second node.
 * @return true if two nodes are members of the same list, false otherwise.
 */
static inline int diminuto_list_aresiblings(
	const diminuto_list_t * firstp,
	const diminuto_list_t * secondp
) {
    return (diminuto_list_root(firstp) == diminuto_list_root(secondp));
}

/*******************************************************************************
 * CONDITIONAL OPERATIONS
 ******************************************************************************/

/**
 * Return the data pointer of a node or null if the node pointer is null.
 * @param nodep points to a node or null if there is no node.
 * @return the data pointer for a node or null.
 */
static inline void * diminuto_list_dataif(
	const diminuto_list_t * nodep
) {
    return (nodep != (diminuto_list_t *)0) ? nodep->data : (void *)0;
}

/**
 * Return a pointer to the first node on a list of which a node is a member, or
 * null if the list is empty.
 * @param nodep points to a node.
 * @return a pointer to the first node on a list or null if the list is empty.
 */
static inline diminuto_list_t * diminuto_list_head(
	const diminuto_list_t * nodep
) {
    return (diminuto_list_isempty(diminuto_list_root(nodep))
        ? (diminuto_list_t *)0
        : diminuto_list_next(diminuto_list_root(nodep)));
}

/**
 * Return a pointer to the last node on a list of which a node is a member, or
 * null is the list is empty.
 * @param nodep points to a node.
 * @return a pointer to the first node on a list or null if the list is empty.
 */
static inline diminuto_list_t * diminuto_list_tail(
	const diminuto_list_t * nodep
) {
    return (diminuto_list_isempty(diminuto_list_root(nodep))
        ? (diminuto_list_t *)0
        : diminuto_list_prev(diminuto_list_root(nodep)));
}

/*******************************************************************************
 * LAST-IN FIRST-OUT (LIFO) STACK API
 ******************************************************************************/

/**
 * Push a node onto the top of a stack.
 * @param stackp points to a node that is on the stack (including the root).
 * @param nodep points to the node to be pushed onto the stack.
 * @return a pointer to the node that was pushed onto the stack.
 */
static inline diminuto_list_t * diminuto_list_push(
	diminuto_list_t * stackp,
	diminuto_list_t * nodep
) {
    return diminuto_list_insert(diminuto_list_root(stackp), nodep);
}

/**
 * Pop a node off the top of a stack.
 * @param stackp points to a node that is on the stack (including the root).
 * @return a pointer to the node popped off or null if the stack was empty.
 */
static inline diminuto_list_t * diminuto_list_pop(
	diminuto_list_t * stackp
) {
    return (diminuto_list_isempty(diminuto_list_root(stackp))
        ? (diminuto_list_t *)0
        : diminuto_list_remove(diminuto_list_next(diminuto_list_root(stackp))));
}

/*******************************************************************************
 * FIRST-IN FIRST-OUT (FIFO) QUEUE API
 ******************************************************************************/

/**
 * Enqueue a node onto the tail of a queue.
 * @param queuep points to a node that is on the queue (including the root).
 * @param nodep points to the node to be enqueued.
 * @return a pointer to the node that was enqueued onto the queue.
 */
static inline diminuto_list_t * diminuto_list_enqueue(
	diminuto_list_t * queuep,
	diminuto_list_t * nodep
) {
    return diminuto_list_insert(diminuto_list_prev(diminuto_list_root(queuep)), nodep);
}

/**
 * Dequeue a node from the head of a queue.
 * @param queuep points to a node that is on the queue (including the root).
 * @return a pointer to a node that was dequeued or null if the queue was empty.
 */
static inline diminuto_list_t * diminuto_list_dequeue(
	diminuto_list_t * queuep
) {
	return (diminuto_list_isempty(diminuto_list_root(queuep))
        ? (diminuto_list_t *)0
        : diminuto_list_remove(diminuto_list_next(diminuto_list_root(queuep))));
}

/*******************************************************************************
 * MESSAGING API
 ******************************************************************************/

/**
 * Receive a message from a mailbox.
 * @param mboxp points to the mailbox.
 * @return a pointer to a message or null if the mailbox was empty.
 */
static inline diminuto_list_t * diminuto_list_receive(
	diminuto_list_t * mboxp
) {
	return diminuto_list_dequeue(mboxp);
}

/**
 * Send a message to a mailbox.
 * @param mboxp points to the mailbox.
 * @param mesgp points to the message to send.
 * @return a pointer to the message.
 */
static inline diminuto_list_t * diminuto_list_send(
	diminuto_list_t * mboxp,
	diminuto_list_t * mesgp
) {
    return diminuto_list_enqueue(mboxp, mesgp);
}

/**
 * Send a high priority message to the mailbox. The message will be placed
 * ahead of all the other messages in the mailbox.
 * @param mboxp points to the mailbox.
 * @param mesgp points to the message to send.
 * @return pointer to the message.
 */
static inline diminuto_list_t * diminuto_list_express(
	diminuto_list_t * mboxp,
	diminuto_list_t * mesgp
) {
    return diminuto_list_push(mboxp, mesgp);
}

/**
 * Remove a message from any mailbox it may be in.
 * @param mesgp points to the message to cancel.
 * @return a pointer to the message.
 */
static inline diminuto_list_t * diminuto_list_cancel(
	diminuto_list_t * mesgp
) {
	return diminuto_list_remove(mesgp);
}

#endif
