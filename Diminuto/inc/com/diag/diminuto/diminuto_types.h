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
#   include <linux/types.h>
#   if 0
       typedef unsigned long uintptr_t; /* Some kernels define this, some don't. */
#   endif
#else
#   include <stddef.h>      /* size_t, ssize_t */
#   include <stdint.h>      /* intptr_t, int64_t, etc. */
#   include <stdbool.h>     /* bool */
#   include <sys/types.h>   /* pid_t */
#endif

/**
 * This type describes the integer declaration of a variable containing ticks,
 * which are the units of time used by Diminuto.
 */
typedef int64_t diminuto_ticks_t;

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

/**
 * This type describes the integer declaration of a variable containing a
 * 32-bit binary IPv4 address. An IPv4 address is conventionally represented
 * in "dot" notation as four decimal digits each representing one byte of the
 * address: "192.168.1.222".
 */
typedef uint32_t diminuto_ipv4_t;

/**
 * This type describes the base type of each slot in an array containing a
 * 128-bit binary IPv6 address. (I chose a 16-bit integer just because the
 * IPv6 notation divides the bits of the address into eight groups of 16-bits
 * each. As a side effect, this causes variables of this type, or of the IPv6
 * address type, to be short word aligned.)
 */
typedef uint16_t diminuto_ipv6_base_t;

/**
 * This type describes the structure declaration of a variable containing a
 * 128-bit binary IPv6 address. An IPv6 address is conventionally represented
 * in "colon" notation as eight groups of sixteen-bits in two hexadecimal
 * digits: "2001:0db8:0000:0000:0000: ff00:0042:8329". Zeros between colons
 * may be omitted, and leading zeros can be abbreviated by a double colon
 * [RFC5952]. The convention for encoding an IPv4 address in a IPv6 address
 * variable is as follows: the first 80bits/10bytes/5shorts are all zeros, the
 * next 16bits/2bytes/1short is are ones, and the final 32bits/4bytes/2shorts
 * contain the IPv4 address. The following IPv4-in-IPv6 representations of the
 * IPv4 address 192.168.1.222 are all equivalent:
 * "0000:0000:0000:0000:0000:FFFF:192.168.1.222", ":::::FFFF:192.168.1.222",
 * "::FFFF:192.168.1.222" [RFC4291]. This is a struct instead of an array
 * type (which do in fact exist) because structs can be passed by value to
 * functions.
 */
typedef struct { diminuto_ipv6_base_t u16[128 / 8 / sizeof(diminuto_ipv6_base_t)]; } diminuto_ipv6_t;

#endif
