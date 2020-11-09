/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TYPES_
#define _H_COM_DIAG_DIMINUTO_TYPES_

/**
 * @file
 * @copyright Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines types consistently across all features.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Type feature provides a one-stop-shop for C types used by Diminuto.
 */

#include "com/diag/diminuto/diminuto_platform.h"

#if defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)
#   include <linux/types.h>
#   include <linux/limits.h>
#   include <linux/un.h>
#   if 0
       typedef unsigned long uintptr_t; /* Some kernels define this, some don't. */
#   endif
#else
#   include <stddef.h>          /* size_t, ssize_t */
#   include <stdint.h>          /* intptr_t, int64_t, etc. */
#   include <stdbool.h>         /* bool */
#   include <sys/types.h>       /* pid_t */
#   include <linux/limits.h>    /* PATH_MAX (includes terminating NUL) */
#   include <linux/un.h>        /* UNIX_PATH_MAX */
#   if 0
#       include <linux/limits.h>    /* PATH_MAX: 4096 */
#       include <linux/un.h>        /* UNIX_PATH_MAX: 108 */
#       include <sys/param.h>       /* MAXPATHLEN: PATH_MAX */
#       include <limits.h>          /* _POSIX_PATH_MAX: 256 */
#   endif
#endif

/**
 * This type describes the integer declaration of a variable containing unsigned
 * ticks, which are the units of time used by Diminuto. See the comments in
 * diminuto_frequency.h for more insight.
 */
typedef uint64_t diminuto_ticks_t;

/**
 * This type describes the integer declaration of a variable containing signed
 * ticks, which are the units of time used by Diminuto. See the comments in
 * diminuto_frequency.h for more insight.
 */
typedef int64_t diminuto_sticks_t;

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
 * functions. I chose a 16-bit integer because the IPv6 notation divides
 * the bits of the address into eight groups of 16-bits each. As a side
 * effect, this causes variables of this type to be short word aligned.
 */
typedef struct { uint16_t u16[128 / 8 / sizeof(uint16_t)]; } diminuto_ipv6_t;

/**
 * This type describes the integer declaration of a variable containing a
 * binary IP port number.
 */
typedef uint16_t diminuto_port_t;

/*
 * I don't define a type for Local (UNIX domain) paths because I tried
 * that and I just found that it obfuscated what was going on.
 */

/**
 * @def DIMINUTO_IPV4_SIZE
 * This defines the buffer size, including the terminating NUL, needed to
 * express an IPv4 address string.
 */
#define DIMINUTO_IPV4_SIZE sizeof("255.255.255.255")

/**
 * This defines a type of character array that can contain an IPv4 address
 * string.
 */
typedef char (diminuto_ipv4_buffer_t)[DIMINUTO_IPV4_SIZE];

/**
 * @def DIMINUTO_IPV6_SIZE
 * This defines the buffer size, including the terminating NUL, needed to
 * express an IPv6 address string.
 */
#define DIMINUTO_IPV6_SIZE sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")

/**
 * This defines a type of character array that can contain an IPv6 address
 * string.
 */
typedef char (diminuto_ipv6_buffer_t)[DIMINUTO_IPV6_SIZE];

/**
 * @def DIMINUTO_LOCAL_SIZE
 * This defines the buffer size, including the terminating NUL, needed
 * to express a Local (UNIX domain socket path) string.
 * UNIX domain socket names have a smaller size limit than full
 * blown file system names; a full path may be reduced to a smaller
 * length once soft links etc. are resolved.
 */
#define DIMINUTO_LOCAL_SIZE UNIX_PATH_MAX

/**
 * This defines a type of character array that can contain a Local
 * (UNIX domain socket path) string. This is NOT large enough to
 * hold any arbitary file system path.
 */
typedef char (diminuto_local_buffer_t)[DIMINUTO_LOCAL_SIZE];

/**
 * @def DIMINUTO_PATH_SIZE
 * This defines the buffer size, including the terminating NUL, needed
 * to express a full file system path. This can be quite large,
 * typically 4096 bytes (4KB).
 */
#define DIMINUTO_PATH_SIZE PATH_MAX

/**
 * This defines a type of character array that can contain a file
 * system path including its terminating NUL.
 */
typedef char (diminuto_path_t)[DIMINUTO_PATH_SIZE];

#endif
