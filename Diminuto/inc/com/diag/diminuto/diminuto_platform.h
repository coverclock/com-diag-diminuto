/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PLATFORM_
#define _H_COM_DIAG_DIMINUTO_PLATFORM_

/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#undef COM_DIAG_DIMINUTO_PLATFORM
#undef COM_DIAG_DIMINUTO_PLATFORM_KERNEL
#undef COM_DIAG_DIMINUTO_PLATFORM_GLIBC
#undef COM_DIAG_DIMINUTO_PLATFORM_UCLIBC
#undef COM_DIAG_DIMINUTO_PLATFORM_BIONIC

#if defined(__KERNEL__) || defined(MODULE)

#	include <linux/version.h>

#   define COM_DIAG_DIMINUTO_PLATFORM_KERNEL (LINUX_VERSION_CODE)
#   define COM_DIAG_DIMINUTO_PLATFORM "kernel"

#else

#   include <features.h>
#   include <sys/cdefs.h>

#   if defined(__UCLIBC_MAJOR__) && defined(__UCLIBC_MINOR__) && defined(__UCLIBC_SUBLEVEL__)

#       define COM_DIAG_DIMINUTO_PLATFORM_UCLIBC (1000000 + ((__UCLIBC_MAJOR__) * 10000) + ((__UCLIBC_MINOR__) * 100) + (__UCLIBC_SUBLEVEL__))
#       define COM_DIAG_DIMINUTO_PLATFORM "uclibc"

#   elif defined(__UCLIBC__)

#       define COM_DIAG_DIMINUTO_PLATFORM_UCLIBC (!0)
#       define COM_DIAG_DIMINUTO_PLATFORM "uclibc"

#   elif defined(__BIONIC__)

#       define COM_DIAG_DIMINUTO_PLATFORM_BIONIC (!0)
#       define COM_DIAG_DIMINUTO_PLATFORM "bionic"

#   elif defined(__GLIBC__) && defined(__GLIBC_MINOR__)

#       define COM_DIAG_DIMINUTO_PLATFORM_GLIBC (((__GLIBC__) << 16) + __GLIBC_MINOR__)
#       define COM_DIAG_DIMINUTO_PLATFORM "glibc"

#   elif defined(__GLIBC__)

#       define COM_DIAG_DIMINUTO_PLATFORM_GLIBC (!0)
#       define COM_DIAG_DIMINUTO_PLATFORM "glibc"

#   else

#       warning Cannot implicitly determine platform!
#       define COM_DIAG_DIMINUTO_PLATFORM "unknown"

#   endif

#endif

#endif
