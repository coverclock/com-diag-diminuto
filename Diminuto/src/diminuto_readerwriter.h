/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_READERWRITER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_READERWRITER_PRIVATE_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This file defines the private API for the Reader Writer featue, which
 * exposes internal functions solely for unit testing.
 */

/*******************************************************************************
 * SYMBOLS
 ******************************************************************************/

/**
 * Reader Writer calling threads may assume the following "roles". These role
 * values are used as parameter to the Reader Writer scheduling functions
 * (below), or as node inserted and removed from the waiting list.
 * "Pending" means the thread has been awakened from the condition variable but
 * has not yet removed its token from the circular list..
 */
typedef enum DiminutoReaderWriterRole {
    ANY         = '*',  /**< Any role. */
    NONE        = '-',  /**< No role. */
    STARTED     = 'S',  /**< Initial thread role. */
    READER      = 'R',  /**< Waiting reader role. */
    WRITER      = 'W',  /**< Waiting writer role. */
    FAILED      = 'X',  /**< Cancelled, timed out, failed, etc. */
    READABLE    = 'r',  /**< Pending reader role. */
    WRITABLE    = 'w',  /**< Pending writer role. */
    RUNNING     = 'A',  /**< Running thread role. */
} diminuto_readerwriter_role_t;

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

/**
 * Remove a node from the wait list.
 * @param rwp points to the Reader Writer object.
 * @param np points to the node to be removed.
 */
extern void diminuto_readerwriter_dequeue(diminuto_readerwriter_t * rwp, diminuto_list_t * np);

/**
 * Return the node at the head of the wait list without removing it,
 * after first discarding any leading failed nodes.
 * @param rwp points to the Reader Writer object.
 * @return a pointer to the node at the head of the wait list or NULL if empty.
 */
extern diminuto_list_t * diminuto_readerwriter_head(diminuto_readerwriter_t * rwp);

/**
 * Return the node in the wait list that is the closest to the head of the list
 * without being pending. If the wait list is empty, this will be the head of
 * the list.
 * @param rwp points to the Reader Writer object.
 * @return the node after which a priority node can be inserted.
 */
extern diminuto_list_t * diminuto_readerwriter_front(diminuto_readerwriter_t * rwp);

/**
 * Insert a node onto the wait list.
 * @param rwp points to the Reader Writer object.
 * @param np points to the node to be inserted.
 * @param priority is true if high priority, false otherwise.
 */
extern void diminuto_readerwriter_enqueue(diminuto_readerwriter_t * rwp, diminuto_list_t * np, int priority);

/**
 * Return true if the indicated node in the list is now at the head, and the
 * state of that node is now pending, indicating that the waiting reader or
 * writer has been activated, false otherwise.
 * @param rwp points to the Reader Writer object.
 * @param np points to the list node for the calling thread.
 * @param pending is the pending role to be used: READABLE or WRITABLE.
 * @return true if the token is both head and pending, false otherwise.
 */
extern int diminuto_readerwriter_ready(diminuto_readerwriter_t * rwp, diminuto_list_t * np, diminuto_readerwriter_role_t pending);

#endif
