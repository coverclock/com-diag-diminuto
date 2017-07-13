/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PLATFORM_
#define _H_COM_DIAG_DIMINUTO_PLATFORM_

/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This header file tries to infer the underlying platform. It has been tested
 * on Ubuntu 14.04 for both user-space builds ("glibc") and kernel module builds
 * ("kernel"), uClibc ("uclibc"), Android ("bionic"), Cygwin ("cygwin"), and
 * Mac OS X ("darwin"). It has also been used on ChrUbuntu, which is an Ubuntu
 * environment running on top of ChromeOS, but this just looks like Ubuntu
 * ("glibc"). HOWEVER, note that Diminuto has never been intended to run on
 * anything but Linux/GNU, so future compatibility with Android, Cygwin, or
 * Mac OS is not guaranteed.
 */

#undef COM_DIAG_DIMINUTO_PLATFORM_EXPLICIT
#undef COM_DIAG_DIMINUTO_PLATFORM_KERNEL
#undef COM_DIAG_DIMINUTO_PLATFORM_GLIBC
#undef COM_DIAG_DIMINUTO_PLATFORM_UCLIBC
#undef COM_DIAG_DIMINUTO_PLATFORM_BIONIC
#undef COM_DIAG_DIMINUTO_PLATFORM_CYGWIN
#undef COM_DIAG_DIMINUTO_PLATFORM_DARWIN
#undef COM_DIAG_DIMINUTO_PLATFORM_UNKNOWN

#if defined(COM_DIAG_DIMINUTO_PLATFORM)

#   warning Explicitly defined platform?
#   define COM_DIAG_DIMINUTO_PLATFORM_EXPLICIT (!0)

#elif defined(__KERNEL__) || defined(MODULE)

#   include <linux/version.h>

#   define COM_DIAG_DIMINUTO_PLATFORM_KERNEL (LINUX_VERSION_CODE)
#   define COM_DIAG_DIMINUTO_PLATFORM "kernel"

#elif defined(__APPLE__)

#   define COM_DIAG_DIMINUTO_PLATFORM_DARWIN (1000000 + (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__))
#   define COM_DIAG_DIMINUTO_PLATFORM "darwin"

#elif defined(__CYGWIN__)

#   define COM_DIAG_DIMINUTO_PLATFORM_CYGWIN (!0)
#   define COM_DIAG_DIMINUTO_PLATFORM "cygwin"

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
#       define COM_DIAG_DIMINUTO_PLATFORM_UNKNOWN (!0)
#       define COM_DIAG_DIMINUTO_PLATFORM "unknown"

#   endif

#endif

#endif
