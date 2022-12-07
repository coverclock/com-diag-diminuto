/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUX_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_MUX_PRIVATE_

/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This describes the Mux private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
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
 * @return a pointer to the object or NULL if an error occurred.
 */
static inline diminuto_mux_set_t * diminuto_mux_set_init(diminuto_mux_set_t * setp)
{
    setp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    setp->max = DIMINUTO_MUX_MOSTNEGATIVE;
    setp->next = -1;
    FD_ZERO(&setp->active);
    FD_ZERO(&setp->ready);

    return setp;
}

static inline diminuto_mux_set_t * diminuto_mux_set_fini(diminuto_mux_set_t * setp)
{
    return (diminuto_mux_set_t *)0;
}

/**
 * Scan the list of active file descriptors in a multiplexer set and determine
 * the maximum and minimum file descriptor numbers.
 * @param setp points to the multiplexer set.
 */
extern void diminuto_mux_set_bound(diminuto_mux_set_t * setp);

/**
 * Renormalize the current file descriptor in a multiplexer set. This is
 * typically done when a file descriptor is unregistered, which may leave the
 * current file descriptor indicating a descriptor that is no longer used.
 * @param setp points to the multiplexer set.
 */
extern void diminuto_mux_set_normalize(diminuto_mux_set_t * setp);

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
extern int diminuto_mux_set_ready(diminuto_mux_set_t * setp);

/**
 * Register a file descriptor into a multiplexer set in a multiplexer.
 * @param muxp points to the multiplexer.
 * @param setp points to a multiplexer set in the multiplexer.
 * @param fd is the file descriptor to be registered.
 * @return 0 for success or <0 for failure (e.g. the fd is already registered).
 */
extern int diminuto_mux_register(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd);

/**
 * Unregister a file descriptor from a multiplexer set in a multiplexer.
 * @param muxp points to the multiplexer.
 * @param setp points to a multiplexer set in the multiplexer.
 * @param fd is the file descriptor to be unregistered.
 * @param silent if true suppresses the error message if the fd is not registered.
 * @return 0 for success or <0 for failure (e.g. the fd was not registered).
 */
extern int diminuto_mux_unregister(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd, int silent);

/**
 * Return the name of a multiplexer set.
 * @param muxp points to a multiplexer structure.
 * @param setp points to a set structure in the same multiplexer structure.
 * @return the canonical name of the set.
 */
extern const char * diminuto_mux_set_name(const diminuto_mux_t * muxp, const diminuto_mux_set_t * setp);

/**
 * Dump the signals in a signal set.
 * @param muxp points to a multiplexer structure.
 * @param sigs points to the signal set.
 */
extern void diminuto_mux_sigs_dump(const diminuto_mux_t * muxp, const sigset_t * sigs);

/**
 * Dump the file descriptors in a file descriptor set.
 * @param muxp points to a multiplexer structure.
 * @param fds points to the file descriptor set.
 */
extern void diminuto_mux_fds_dump(const diminuto_mux_t * muxp, const fd_set * fds);

/**
 * Dump a set structure in a multiplexer structure.
 * @param muxp points to the multiplexer structure.
 * @param setp points to a set structure in the same multiplexer structure.
 */
extern void diminuto_mux_set_dump(const diminuto_mux_t * muxp, const diminuto_mux_set_t * setp);

#endif
