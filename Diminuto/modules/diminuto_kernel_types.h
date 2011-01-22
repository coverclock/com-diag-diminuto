/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_KERNEL_TYPES
#define _H_COM_DIAG_DIMINUTO_KERNEL_TYPES

/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Defines basic size-sensitive integer types. May be included by user space
 * applications or by kernel space code such as kernel modules and device
 * drivers.
 */

#if defined(__KERNEL__) || defined(MODULE)
#	include <linux/types.h>
#	if 0
		typedef unsigned long uintptr_t; /* Some kernels define this, some don't. */
#	endif
#else
#	include <stdint.h>
#	include <types.h>
#endif

#endif
