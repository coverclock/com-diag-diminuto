/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ATOMICAPPLY_
#define _H_COM_DIAG_DIMINUTO_ATOMICAPPLY_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Atomically perform a read-modify-write operation. EXPERIMENTAL
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Provides a mechanism through which a library can provide a thread-safe
 * settor in which the caller-specified read-modify-write of a shared library
 * variable is done without the caller having to know any more than the data
 * type of the shared variable.
 * EXPERIMENTAL
 */

#include <stdint.h>
#include <pthread.h>

static const void * DIMINUTO_ATOMIC_ERROR = (void *)(intptr_t)-1;

/**
 * This defines the prototype for a functor that applies a context (whatever
 * that is) to a shared data variable and returns some result.
 */
typedef void * (diminuto_atomic_functor_t)(void * datap, void * contextp);

/**
 * Atomically apply a caller-provided functor to a shared data variable along
 * with a caller-provided context while requiring the caller to only know
 * about the underlying type of the shared data variable.
 * @param mutexp points to a mutex used to serialized the operation.
 * @param functorp points to the caller functor.
 * @param datap points to the shared data variable.
 * @param contextp points to the caller context.
 * @return a value.
 */
extern void * diminuto_atomic_apply(pthread_mutex_t * mutexp, diminuto_atomic_functor_t * functorp, void * datap, void * contextp);

#endif
