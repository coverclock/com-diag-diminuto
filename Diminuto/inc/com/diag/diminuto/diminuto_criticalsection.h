/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CRITICALSECTION_
#define _H_COM_DIAG_DIMINUTO_CRITICALSECTION_

/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <pthread.h>

/**
 * @def DIMINUTO_CRITICAL_SECTION_BEGIN
 * Begin a code section that is serialized using a pthread mutex specified by
 * the caller as a pointer in the argument @a _MUTEX_P by disabling thread
 * cancellation and locking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_BEGIN(_MUTEXP_) \
	do { \
		pthread_mutex_t * _diminuto_critical_section_mutexp_ = (_MUTEXP_); \
		int _diminuto_critical_section_cancel_state_; \
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &_diminuto_critical_section_cancel_state_); \
		pthread_mutex_lock(_diminuto_critical_section_mutexp_); \
		do { \
			do {} while (0)

/**
 * @def DIMINUTO_CRITICAL_SECTION_END
 * End a code section that was serialized using the pthread mutex specified at
 * the beginning of the block by unlocking the mutex and returning the thread
 * cancellation state to its value at the beginning of the block..
 */
#define DIMINUTO_CRITICAL_SECTION_END \
		} while (0); \
		pthread_mutex_unlock(_diminuto_critical_section_mutexp_); \
		pthread_setcancelstate(_diminuto_critical_section_cancel_state_, (int *)0); \
	} while (0)

#endif
