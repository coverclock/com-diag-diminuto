/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNINTERRUPTIBLESECTION_
#define _H_COM_DIAG_DIMINUTO_UNINTERRUPTIBLESECTION_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <signal.h>

#define DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(_SIGNAL_) \
	do { \
		sigset_t _diminuto_uninterruptible_section_now_; \
		sigset_t _diminuto_uninterruptible_section_was_; \
	    sigemptyset(&_diminuto_uninterruptible_section_now_); \
	    sigaddset(&_diminuto_uninterruptible_section_now_, _SIGNAL_); \
	    sigprocmask(SIG_BLOCK, &_diminuto_uninterruptible_section_now_, &_diminuto_uninterruptible_section_was_); \
	    do { \
	    	do {} while (0)

#define DIMINUTO_UNINTERRUPTIBLE_SECTION_END \
	    } while (0); \
		sigprocmask(SIG_SETMASK, &_diminuto_uninterruptible_section_was_, (sigset_t *)0); \
	} while (0)

#endif
