/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TYPES_
#define _H_COM_DIAG_DIMINUTO_TYPES_

/**
 * @file
 *
 * Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_platform.h"

#if defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)
#	include <linux/types.h>
#	if 0
		typedef unsigned long uintptr_t; /* Some kernels define this, some don't. */
#	endif
#else
#	include <stdint.h>
#	include <sys/types.h>
#endif

/**
 * This type describes the integer declaration of a variable containing ticks,
 * which are the units of time used by Diminuto.
 */
typedef int64_t diminuto_ticks_t;

/**
 * This type describes the integer declaration of a variable containing a
 * binary IPV4 address.
 */
typedef uint32_t diminuto_ipv4_t;

/**
 * This type describes the integer declaration of a variable containing a
 * binary IP port number.
 */
typedef uint16_t diminuto_port_t;

/**
 * This type describes the integer declaration of a variable containing an
 * unsigned integer returned by the diminuto_number_unsigned() function.
 */
typedef uint64_t diminuto_unsigned_t;

/**
 * This type describes the integer declaration of a variable containing a
 * signed integer turned by the diminuto_number_signed() function.
 */
typedef int64_t diminuto_signed_t;

#endif
