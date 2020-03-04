/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUX_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_MUX_PRIVATE_

/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This describes the private API for the mux feature, some of which may be used
 * by other Diminuto features. They are not accessible however to applications
 * outside of Diminuto.
 */

#include "com/diag/diminuto/diminuto_mux.h"

static const int DIMINUTO_MUX_MOSTPOSITIVE = ~(((int)1)<<((sizeof(int)*8)-1)); /* Most positive integer. */
static const int DIMINUTO_MUX_MOSTNEGATIVE =  (((int)1)<<((sizeof(int)*8)-1)); /* Most negative integer (produces -pendantic warning). */

/**
 * Initialize a multiplexer set.
 * @param setp points to the multiplexer set.
 */
static inline void diminuto_mux_set_init(diminuto_mux_set_t * setp)
{
    setp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    setp->max = DIMINUTO_MUX_MOSTNEGATIVE;
    setp->next = -1;
    FD_ZERO(&setp->active);
    FD_ZERO(&setp->ready);
}

/**
 * Scan the list of active file descriptors in a multiplexer set and determine
 * the maximum and minimum file descriptor numbers.
 * @param setp points to the multiplexer set.
 */
static void diminuto_mux_set_bound(diminuto_mux_set_t * setp);

/**
 * Renormalize the current file descriptor in a multiplexer set. This is
 * typically done when a file descriptor is unregistered, which may leave the
 * current file descriptor indicating a descriptor that is no longer used.
 * @param setp points to the multiplexer set.
 */
static void diminuto_mux_set_normalize(diminuto_mux_set_t * setp);

/**
 * Determine if the current number of file descriptors includes all of those
 * in a particular multiplexer set.
 * @param setp points to the multiplexer set.
 * @param nfdsp points to the current number of file descriptors.
 */
static inline void diminuto_mux_set_census(diminuto_mux_set_t * setp, int * nfdsp)
{
    if (*nfdsp <= setp->max) { *nfdsp = setp->max + 1; }
}

/**
 * Reset the current file descriptor in a multiplexer set. This is typically
 * done when after a select(2) is done by the wait() function and the current
 * file descriptor indicates that the multiplexer set was formerly idle.
 * @param setp points to the multiplexer set.
 */
static inline void diminuto_mux_set_reset(diminuto_mux_set_t * setp)
{
    if (setp->next >= 0) {
        /* Do nothing. */
    } else if (setp->max < 0) {
       /* Do nothing. */
    } else {
        setp->next = setp->min;
    }
}

/**
 * Return the next ready file descriptor, if any, in a multiplexer set.
 * @param setp points to the multiplexer set.
 * @return a ready file descriptor or -1 if none.
 */
static int diminuto_mux_set_ready(diminuto_mux_set_t * setp);

/**
 * Register a file descriptor into a multiplexer set in a multiplexer.
 * @param muxp points to the multiplexer.
 * @param setp points to a multiplexer set in the multiplexer.
 * @param fd is the file descriptor to be registered.
 * @return 0 for success or <0 for failure (e.g. the fd is already registered).
 */
static int diminuto_mux_register(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd);

/**
 * Unregister a file descriptor from a multiplexer set in a multiplexer.
 * @param muxp points to the multiplexer.
 * @param setp points to a multiplexer set in the multiplexer.
 * @param fd is the file descriptor to be unregistered.
 * @return 0 for success or <0 for failure (e.g. the fd was not registered).
 */
static int diminuto_mux_unregister(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd);

#endif
