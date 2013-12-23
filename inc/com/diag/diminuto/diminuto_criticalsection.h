/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CRITICALSECTION_
#define _H_COM_DIAG_DIMINUTO_CRITICALSECTION_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <pthread.h>

#define DIMINUTO_CRITICAL_SECTION_BEGIN(_MUTEXP_) \
	do { \
		pthread_mutex_t * _diminuto_critical_section_mutex_ = (_MUTEXP_); \
		int _diminuto_critical_section_cancel_state_; \
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &_diminuto_critical_section_cancel_state_); \
		pthread_mutex_lock(_diminuto_critical_section_mutex_)

#define DIMINUTO_CRITICAL_SECTION_END \
		pthread_mutex_unlock(_diminuto_critical_section_mutex_); \
		pthread_setcancelstate(_diminuto_critical_section_cancel_state_, (int *)0); \
	} while (0)

#endif
