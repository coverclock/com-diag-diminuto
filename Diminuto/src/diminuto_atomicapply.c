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

void * diminuto_atomicapply(pthread_mutex_t * mutexp, diminuto_atomicapply_functor_t * functorp, void * datap, void * contextp)
{
	void * resultp = (void *)0;

	DIMINUTO_CRITICAL_SECTION_BEGIN(mutexp);
		resultp = (*functorp)(datap, contextp);
	DIMINUTO_CRITICAL_SECTION_END;

	return resultp;
}
