/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Atomically perform a read-modify-write operation. EXPERIMENTAL
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * EXPERIMENTAL
 */

#include "com/diag/diminuto/diminuto_atomicapply.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"

/*
 * Note that the result pointer variable will not be modified if somehow
 * the mutex fails to lock. In that case, the function will exit with
 * errno set to indicate the nature of the failure.
 */
void * diminuto_atomic_apply(pthread_mutex_t * mutexp, diminuto_atomic_functor_t * functorp, void * sharedp, void * contextp)
{
	void * resultp = (void *)DIMINUTO_ATOMIC_ERROR;

	DIMINUTO_CRITICAL_SECTION_BEGIN(mutexp);
		resultp = (*functorp)(sharedp, contextp);
	DIMINUTO_CRITICAL_SECTION_END;

	return resultp;
}
