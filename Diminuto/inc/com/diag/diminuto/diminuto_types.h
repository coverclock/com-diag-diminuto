/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TYPES_
#define _H_COM_DIAG_DIMINUTO_TYPES_

/**
 * @file
 * @copyright Copyright 2010-2023 Digital Aggregates Corporation, Colorado, USA.
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
#   include <linux/limits.h>    /* NAME_MAX, PATH_MAX */
#   include <linux/un.h>
#   if 0
       typedef unsigned long uintptr_t;
#   endif
#else
#   include <stddef.h>          /* size_t, ssize_t, wint_t, etc. */
#   include <stdint.h>          /* intptr_t, int64_t, etc. */
#   include <stdbool.h>         /* bool */
#   include <wchar.h>           /* wchar_t, wint_t */
#   include <limits.h>          /* _POSIX_PATH_MAX: 256 */
#   include <sys/types.h>       /* pid_t */
#   include <sys/param.h>       /* MAXPATHLEN: PATH_MAX */
#   include <linux/limits.h>    /* PATH_MAX (includes terminating NUL): 4096 */
#   include <linux/un.h>        /* UNIX_PATH_MAX: 108 */
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

/**
 * Enumerates the various buffer sizes needed for the various 2string functions.
 */
enum DiminutoTypesBufferSizes {
    DIMINUTO_TYPES_IPV4_BUFFER_SIZE     = sizeof("255.255.255.255"),                            /* (4*3)+(3*1)+1=16 */
    DIMINUTO_TYPES_IPV6_BUFFER_SIZE     = sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"),    /* (8*4)+(7*1)+1=40 */
    DIMINUTO_TYPES_IP_BUFFER_SIZE       = DIMINUTO_TYPES_IPV6_BUFFER_SIZE,                      /* 40 */
    DIMINUTO_TYPES_LOCAL_BUFFER_SIZE    = NAME_MAX,                                             /* 255 */
    DIMINUTO_TYPES_PATH_BUFFER_SIZE     = PATH_MAX,                                             /* 4096 */
    DIMINUTO_TYPES_PORT_BUFFER_SIZE     = sizeof("65535"),                                      /* 6 */
    DIMINUTO_TYPES_ENDPOINT_BUFFER_SIZE = DIMINUTO_TYPES_LOCAL_BUFFER_SIZE,                     /* (((1+40+1)+(1+6)+(1+6))+1+(16+(1+6)+(1+6))+1)<257 */
};

/**
 * This defines a type of character array that can contain a Local
 * (UNIX domain) socket path. This is NOT large enough to
 * hold any arbitary file system path.
 */
typedef char (diminuto_local_t)[DIMINUTO_TYPES_LOCAL_BUFFER_SIZE];

/**
 * This defines a type of character array that can contain an IPv4 address
 * as a printable string including the terminating NUL.
 */
typedef char (diminuto_ipv4_buffer_t)[DIMINUTO_TYPES_IPV4_BUFFER_SIZE];

/**
 * This defines a type of character array that can contain an IPv6 address
 * as a printable string including the terminating NUL.
 */
typedef char (diminuto_ipv6_buffer_t)[DIMINUTO_TYPES_IPV6_BUFFER_SIZE];

/**
 * This defines a type of character array that can contain either an
 * IPv4 or IPv6 address as a printable string including the terminating NUL.
 */
typedef char (diminuto_ip_buffer_t)[DIMINUTO_TYPES_IP_BUFFER_SIZE];

/**
 * This defines the type of character array that can contain an endpoint
 * as a printable string including a terminating NUL.
 */
typedef char (diminuto_endpoint_buffer_t)[DIMINUTO_TYPES_ENDPOINT_BUFFER_SIZE];

/**
 * This defines a type of character array that can contain a port number
 * as a printable string including the terminating NUL. It estimates the
 * size of the printable string based on the sizeof() the port type.
 */
typedef char (diminuto_port_buffer_t)[DIMINUTO_TYPES_PORT_BUFFER_SIZE];

/**
 * This defines the type of character array that can contain a printable
 * local socket path, which is in fact the same thing as a local socket
 * path.
 */
typedef diminuto_local_t diminuto_local_buffer_t;

/**
 * This defines a type of character array that can contain a file
 * system path including its terminating NUL. This array is quite
 * large: 4096 on Linux systems. It allows the maximum Linux path
 * size, and is a lot larger than the POSIX path limit: 256 bytes.
 */
typedef char (diminuto_path_t)[DIMINUTO_TYPES_PATH_BUFFER_SIZE];

/**
 * This defines a type that is of zero length but two-byte aligned.
 */
typedef uint16_t (diminuto_align2_t)[0] /* Likely pedantic warnings. */
#if defined(__GNUC__)
    __attribute__ ((aligned(2)))
#else
#   warning Required byte alignment is implicitly assumed.
#endif
    ;

/**
 * This defines a type that is of zero length but four-byte aligned.
 */
typedef uint32_t (diminuto_align4_t)[0] /* Likely pedantic warnings. */
#if defined(__GNUC__)
    __attribute__ ((aligned(4)))
#endif
    ;

/**
 * This defines a type that is of zero length but eight-byte aligned.
 * Note that GCC on some 32-bit processors (e.g. i686) align uint64_t
 * variables to four-bytes; this overcomes that.
 */
typedef uint64_t (diminuto_align8_t)[0] /* Likely pedantic warnings. */
#if defined(__GNUC__)
    __attribute__ ((aligned(8)))
#endif
    ;

/**
 * This is the type you need when you want to use a "%lld" printf format.
 */
typedef long long signed int diminuto_lld_t;

/**
 * This is the type you need when you want to use a "%llu" printf format.
 */
typedef long long unsigned int diminuto_llu_t;

/**
 * This is the type used to hold a file descriptor (including a socket).
 * The Linux/GNU/Posix APIs use int for this, but having a specific type
 * helps with documenting object structures, which is about the only place
 * this type is used.
 */
typedef int diminuto_fd_t;

/**
 * This is the type used to hold a signal number.
 * The Linux/GNU/Posix APIs use int for this, but having a specific type
 * helps with documenting object structures, which is about the only place
 * this type is used.
 */
typedef int diminuto_sig_t;

/**
 * This is the type used to hold an error number.
 * The Linux/GNU/Posix APIs use int for this, but having a specific type
 * helps with documenting object structures, which is about the only place
 * this type is used.
 */
typedef int diminuto_errno_t;

#endif
