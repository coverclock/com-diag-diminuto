/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TYPES_
#define _H_COM_DIAG_DIMINUTO_TYPES_

/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#if defined(__KERNEL__) || defined(MODULE)
#	include <linux/types.h>
#	if 0
		typedef unsigned long uintptr_t; /* Some kernels define this, some don't. */
#	endif
#else
#	include <stdint.h>
#	include <sys/types.h>
#endif

typedef uint64_t diminuto_usec_t;

typedef uint32_t diminuto_ipv4_t;

typedef uint16_t diminuto_port_t;

typedef uint64_t diminuto_unsigned_t;

typedef int64_t diminuto_signed_t;

#endif
