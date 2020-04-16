/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * There's a lot of duplication here, but I'm paranoid that way. Remarkably,
 * I once worked on an embedded project using a proprietary non-GNU C compiler
 * in which the sizeof of signed type was not the same as the sizeof of the
 * unsigned of the same type. I was a little astounded by that. Also, note
 * that you can take the sizeof a void and of a function type (as opposed to a
 * void or function pointer). It's good to know these things.
 */

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_offsetof.h"

#define printsizeof(_TYPE_) printf("sizeof(%s)=%zu\nsizeof(%s*)=%zu\n", #_TYPE_, sizeof(_TYPE_), #_TYPE_, sizeof(_TYPE_*))

#define issigned(_TYPE_) (((_TYPE_)(~(_TYPE_)0)) < 0)

#define printsignof(_TYPE_) printf("sizeof(%s)=%zu\nsizeof(%s*)=%zu\nissigned(%s)=%d\n", #_TYPE_, sizeof(_TYPE_), #_TYPE_, sizeof(_TYPE_*), #_TYPE_, issigned(_TYPE_))

typedef enum Enum { ENUM = 0 } enum_t;

#if defined(PROBLEMATIC)
typedef void (function_t)(void);

struct Zero {
    uint8_t field[0];
};

typedef uint8_t (Array)[0];
#endif

int main(void)
{
    printsignof(char);
    printsignof(signed char);
    printsignof(unsigned char);
    printf("sizeof('%c')=%zu\n", '?', sizeof('?'));
    printf("sizeof(\"%s\")=%zu\n", "STRING", sizeof("STRING"));
    printsignof(short);
    printsignof(signed short);
    printsignof(unsigned short);
    printsignof(int);
    printsignof(signed int);
    printsignof(unsigned int);
    printsignof(long);
    printsignof(signed long);
    printsignof(unsigned long);
    printsignof(long long);
    printsignof(signed long long);
    printsignof(unsigned long long);
    printsizeof(float);
    printsizeof(double);
#if defined(PROBLEMATIC)
    printsizeof(void);
#endif
    printsizeof(bool);
    printsignof(enum_t);
#if defined(PROBLEMATIC)
    printsizeof(function_t);
#endif
    printsignof(int8_t);
    printsignof(uint8_t);
    printsignof(int16_t);
    printsignof(uint16_t);
    printsignof(int32_t);
    printsignof(uint32_t);
    printsignof(int64_t);
    printsignof(uint64_t);
    printsignof(intptr_t);
    printsignof(uintptr_t);
    printsignof(wchar_t);
    printsignof(wint_t);
    printsignof(size_t);
    printsignof(ssize_t);
    printsignof(pid_t);
    printsignof(time_t);
    printsignof(dev_t);
    printsignof(ino_t);
    printsignof(mode_t);
    printsignof(nlink_t);
    printsignof(uid_t);
    printsignof(gid_t);
    printsignof(off_t);
    printsignof(blksize_t);
    printsignof(blkcnt_t);
    printsizeof(pthread_t);
    printsizeof(pthread_mutex_t);
    printsizeof(pthread_cond_t);
    printsignof(diminuto_ticks_t);
    printsignof(diminuto_sticks_t);
    printsizeof(diminuto_ipv4_t);
    printsizeof(diminuto_ipv6_t);
    printsignof(diminuto_port_t);
    printsignof(diminuto_unsigned_t);
    printsignof(diminuto_signed_t);
#if defined(PROBLEMATIC)
    printsizeof(struct Zero);
    printsizeof(Array);
#endif
    return 0;
}
