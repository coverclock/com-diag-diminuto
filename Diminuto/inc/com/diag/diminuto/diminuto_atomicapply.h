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
 * EXPERIMENTAL
 *
 * EXAMPLE
 *
 * void * application_functor(void * dp, void * cp) {
 *     void * rp = (void *)*(int *)dp;
 *     *(int *)dp = (int)cp;
 *     return rp;
 * }
 *
 * static inline int library_settor(diminuto_atomicapply_functor_t * fp, int value) {
 *     extern pthread_mutex_t library_mutex;
 *     extern int library_variable;
 *     return (int)diminuto_atomicapply(&library_mutex, fp, &library_shared, (void *)value);
 * }
 */

#include <pthread.h>

/**
 * This defines the prototype for a functor that applies a context (whatever
 * that is) to a shared data variable and returns some result.
 */
typedef void * (diminuto_atomicapply_functor_t)(void * datap, void * contextp);

/**
 * Atomically apply a caller-provided functor to a shared data variable along
 * with a caller-provided context without requiring the caller to only know
 * about the underlying type of the shared data variable.
 * @param mutexp points to a mutex used to serialized the operation.
 * @param functorp points to the caller functor.
 * @param datap points to the shared data variable.
 * @param contextp points to the caller context.
 * @return a value.
 */
extern void * diminuto_atomicapply(pthread_mutex_t * mutexp, diminuto_atomicapply_functor_t * functorp, void * datap, void * contextp);

#endif
