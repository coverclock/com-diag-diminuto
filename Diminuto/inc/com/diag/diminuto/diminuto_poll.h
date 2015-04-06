/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_POLL_
#define _H_COM_DIAG_DIMINUTO_POLL_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is a socket multiplexer that uses the ppoll(2) system call instead of
 * the pselect(2) system call used by the mux feature. This feature exists
 * mostly because I wanted to benchmark ppoll(2) against pselect(2) using the
 * same abstraction and a similar API. But it isn't as efficient as it could
 * be since it sits on top of the mux feature and only differs in the wait
 * function.
 */

#include "com/diag/diminuto/diminuto_mux.h"
#define _GNU_SOURCE
#include <poll.h>

/**
 * @def COM_DIAG_DIMINUTO_POLL_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you should always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_POLL_FREQUENCY COM_DIAG_DIMINUTO_MUX_FREQUENCY

/**
 * Return the resolution of the Diminuto poller timeout units in ticks per
 * second (Hertz). Timeout durations smaller than the equivalent period in ticks
 * may not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timeout durations
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_ticks_t diminuto_poll_frequency(void) {
    return diminuto_mux_frequency();
}

/**
 * This is the poller state.
 */
typedef struct DiminutoPoll {
    diminuto_mux_t mux;
    struct pollfd * pollfd;
    nfds_t nfds;
    int min;
    int max;
    int refresh;
} diminuto_poll_t;

/**
 * Initialize a poller. Any prior state is lost, although this does not
 * effect any file descriptors. The poller inherits the current signal
 * mask of the caller such that signals that are blocked when the poller
 * is initialized will continue to be blocked when the poller is waiting.
 * Signals can be removed from this cached mask using the unblock poller
 * function.
 * @param pollp points to a poller structure.
 */
extern void diminuto_poll_init(diminuto_poll_t * pollp);

/**
 * Release any dynamically acquired storage associated with the poller.
 * @param pollp points to a poller structure.
 */
extern void diminuto_poll_fini(diminuto_poll_t * pollp);

/**
 * Register a file descriptor with the poller for reading. It is an error
 * to register a registered file descriptor. Note that although it is common
 * for a socket to be registered for both read and write, registering a socket
 * for both read or write and accept is not supported.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_register_read(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_read(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a file descriptor with the poller for writing. It is an error
 * to register a registered file descriptor. Note that although it is common
 * for a socket to be registered for both read and write, registering a socket
 * for both read or write and accept is not supported.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_register_write(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_write(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a listening file descriptor with the poller for accepting.
 * It is an error to register a registered file descriptor.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_register_accept(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_accept(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Register a file descriptor with the poller for exceptions (which
 * typically means for urgent out-of-band one byte receives on a stream socket).
 * It is an error to register a registered file descriptor.
 * @param pollp points to an initialized poller structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_register_urgent(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_register_urgent(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a file descriptor from the poller for reading. It is an error
 * to unregister an unregistered file descriptor. Note that although it is
 * common for a socket to be registered for both read and write, registering a
 * socket for both read or write and accept is not supported.
 * @param pollp points to an initialized poller structure.
 * @param fd is an registered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_read(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_read(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a file descriptor from the poller for writing. It is an error
 * to unregister an unregistered file descriptor.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_write(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_write(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a listening file descriptor from the poller for accepting.
 * It is an error to unregister an unregistered file descriptor.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_accept(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_accept(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Unregister a file descriptor from the poller for exceptions (which
 * typically means for urgent out-of-band one byte receives on a stream socket).
 * It is an error to unregister an unregistered file descriptor.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_urgent(diminuto_poll_t * pollp, int fd) {
    int rc;
    rc = diminuto_mux_unregister_urgent(&pollp->mux, fd);
    if (rc == 0) { pollp->refresh = !0; }
    return rc;
}

/**
 * Add a signal to the mask of those to be atomically unblocked while the
 * poller is waiting.
 * @param pollp points to an initialized poller structure.
 * @param signum is an unregistered blocked signal.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_register_signal(diminuto_poll_t * pollp, int signum) {
    return diminuto_mux_register_signal(&pollp->mux, signum);
}

/**
 * Remove a signal from the mask of those to be atomically unblocked while the
 * poller is waiting.
 * @param pollp points to an initialized poller structure.
 * @param signum is a registered blocked signal.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_unregister_signal(diminuto_poll_t * pollp, int signum) {
    return diminuto_mux_unregister_signal(&pollp->mux, signum);
}

/**
 * Wait until one or more registered file descriptors are ready for reading,
 * writing, or accepting, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the poller to block indefinitely until either
 * a file descriptor is ready or one of the registered signals is caught.
 * @param pollp points to an initialized poller structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for blocking.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
extern int diminuto_poll_wait(diminuto_poll_t * pollp, diminuto_ticks_t timeout);

/**
 * Return the next registered file descriptor that is ready for reading.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for reading, or <0 if none.
 */
static inline int diminuto_poll_ready_read(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_read(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for writing.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for writing, or <0 if none.
 */
static inline int diminuto_poll_ready_write(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_write(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for accepting.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
static inline int diminuto_poll_ready_accept(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_accept(&pollp->mux);
}

/**
 * Return the next registered file descriptor that is ready for exceptions.
 * Typically this means there is urgent data available to be received
 * out-of-band on the descriptor.
 * @param pollp points to an initialized poller structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
static inline int diminuto_poll_ready_urgent(diminuto_poll_t * pollp) {
    return diminuto_mux_ready_urgent(&pollp->mux);
}

/**
 * Unregister a registered file descriptor and close it. The file descriptor
 * is not closed if it is not registered.
 * @param pollp points to an initialized poller structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
static inline int diminuto_poll_close(diminuto_poll_t * pollp, int fd) {
    return diminuto_mux_close(&pollp->mux, fd);
}

#endif
