/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Print on stdout the sizeof various useful things.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * There's a lot of duplication here, but I'm paranoid that way. Remarkably,
 * I once worked on an embedded project using a proprietary non-GNU C compiler
 * in which the sizeof of signed type was not the same as the sizeof of the
 * unsigned of the same type. I was a little astounded by that. Also, note
 * that you can take the sizeof a void and of a function type (as opposed to a
 * void or function pointer). It's good to know these things.
 */

#define PROBLEMATIC

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>    /* PATH_MAX */
#include <linux/un.h>        /* UNIX_PATH_MAX */
#include <sys/param.h>       /* MAXPATHLEN */
#include <limits.h>          /* _POSIX_PATH_MAX */
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_offsetof.h"

#define printsizeof(_TYPE_) printf("sizeof(%s)=%zu sizeof(%s*)=%zu\n", #_TYPE_, sizeof(_TYPE_), #_TYPE_, sizeof(_TYPE_*))

#define issigned(_TYPE_) (((_TYPE_)(~(_TYPE_)0)) < 0)

#define printsignof(_TYPE_) printf("sizeof(%s)=%zu sizeof(%s*)=%zu issigned(%s)=%d\n", #_TYPE_, sizeof(_TYPE_), #_TYPE_, sizeof(_TYPE_*), #_TYPE_, issigned(_TYPE_))

#define printissigned(_TYPE_) do { printsignof(_TYPE_); diminuto_expect(issigned(_TYPE_)); } while (0)

#define printisunsigned(_TYPE_) do { printsignof(_TYPE_); diminuto_expect(!issigned(_TYPE_)); } while (0)

#define printvalueof(_SYMBOL_) do { uint8_t array_##_SYMBOL_[_SYMBOL_]; printf("sizeof([%s])=%zu\n", #_SYMBOL_, sizeof(array_##_SYMBOL_)); } while (0) 

typedef enum Enum { ENUM = 0 } enum_t;

typedef struct BitField { int bit1 : 1; char bit2 : 8; } bitfield_t;

#if defined(PROBLEMATIC)
typedef void (function_t)(void);

struct Zero {
    uint8_t field[0];
};

typedef uint8_t (Array)[0];
#endif

static int8_t array_PATH_MAX[PATH_MAX];
static int8_t array_UNIX_PATH_MAX[UNIX_PATH_MAX];
static int8_t array_MAXPATHLEN[MAXPATHLEN];
static int8_t array_POSIX_PATH_MAX[_POSIX_PATH_MAX];

int main(void)
{
    printsignof(char);
    printissigned(signed char);
    printisunsigned(unsigned char);
    printf("sizeof('%c')=%zu\n", '?', sizeof('?'));
    printf("sizeof(\"%s\")=%zu\n", "STRING", sizeof("STRING"));
    printissigned(short);
    printissigned(signed short);
    printisunsigned(unsigned short);
    printissigned(int);
    printissigned(signed int);
    printisunsigned(unsigned int);
    printissigned(long);
    printissigned(signed long);
    printisunsigned(unsigned long);
    printissigned(long long);
    printissigned(signed long long);
    printisunsigned(unsigned long long);
    printsizeof(float);
    printsizeof(double);
#if defined(PROBLEMATIC)
    printsizeof(void);
#endif
    printsizeof(bool);
    printsignof(enum_t);
    printsizeof(bitfield_t);
#if defined(PROBLEMATIC)
    printsizeof(function_t);
#endif
    printissigned(int8_t);
    printisunsigned(uint8_t);
    printissigned(int16_t);
    printisunsigned(uint16_t);
    printissigned(int32_t);
    printisunsigned(uint32_t);
    printissigned(int64_t);
    printisunsigned(uint64_t);
    printissigned(intptr_t);
    printisunsigned(uintptr_t);
    printsignof(ptrdiff_t);
    printsignof(wchar_t);
    printsignof(wint_t);
    printisunsigned(size_t);
    printissigned(ssize_t);
    printsignof(pid_t);
    printsignof(time_t);
#if defined(_TIME_BITS)
    printvalueof(_TIME_BITS)
#endif
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
    printisunsigned(diminuto_ticks_t);
    printissigned(diminuto_sticks_t);
    printsizeof(diminuto_ipv4_t);
    printsizeof(diminuto_ipv4_buffer_t);
    printsizeof(diminuto_ipv6_t);
    printsizeof(diminuto_ipv6_buffer_t);
    printsignof(diminuto_port_t);
    printsizeof(diminuto_local_t);
    printisunsigned(diminuto_unsigned_t);
    printissigned(diminuto_signed_t);
    printsizeof(diminuto_path_t);
    printissigned(diminuto_lld_t);
    printisunsigned(diminuto_llu_t);
#if defined(PROBLEMATIC)
    printsizeof(struct Zero);
    printsizeof(Array);
#endif
    printvalueof(PATH_MAX);
    printvalueof(UNIX_PATH_MAX);
    printvalueof(MAXPATHLEN);
    printvalueof(_POSIX_PATH_MAX);
    return 0;
}
