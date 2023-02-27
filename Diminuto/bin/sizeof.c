/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2022 Digital Aggregates Corporation, Colorado, USA.
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

#define PROBLEMATIC (1)

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_offsetof.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_widthof.h"
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

#define printsizeofnoninteger(_TYPE_) \
    do { \
        printf("sizeof(%s)=%zu", #_TYPE_, sizeof(_TYPE_)); \
        printf(" _Alignof(%s)=%zu", #_TYPE_, _Alignof(_TYPE_)); \
        printf(" widthof(%s)=%zu", #_TYPE_,  widthof(_TYPE_)); \
        printf(" sizeof(%s*)=%zu", #_TYPE_, sizeof(_TYPE_*)); \
        putchar('\n'); \
    } while (0)

#define printsizeof(_TYPE_) \
    do { \
        printf("sizeof(%s)=%zu", #_TYPE_, sizeof(_TYPE_)); \
        printf(" _Alignof(%s)=%zu", #_TYPE_, _Alignof(_TYPE_)); \
        printf(" widthof(%s)=%zu", #_TYPE_,  widthof(_TYPE_)); \
        printf(" sizeof(%s*)=%zu", #_TYPE_, sizeof(_TYPE_*)); \
        printf(" issigned(%s)=%d", #_TYPE_, issigned(_TYPE_)); \
        if (issigned(_TYPE_)) { \
            printf(" minimumof(%s)=0x%llx=%lld", #_TYPE_, (long long unsigned)minimumof(_TYPE_), (long long signed)minimumof(_TYPE_)); \
            printf(" maximummof(%s)=0x%llx=%lld", #_TYPE_,  (long long unsigned)maximumof(_TYPE_), (long long signed)maximumof(_TYPE_)); \
        } else { \
            printf(" minimumof(%s)=0x%llx=%llu", #_TYPE_, (long long unsigned)minimumof(_TYPE_), (long long unsigned)minimumof(_TYPE_)); \
            printf(" maximummof(%s)=0x%llx=%llu", #_TYPE_, (long long unsigned)maximumof(_TYPE_), (long long unsigned)maximumof(_TYPE_)); \
        } \
        putchar('\n'); \
    } while (0)

#define printissigned(_TYPE_) do { printsizeof(_TYPE_); diminuto_expect(issigned(_TYPE_)); } while (0)

#define printisunsigned(_TYPE_) do { printsizeof(_TYPE_); diminuto_expect(!issigned(_TYPE_)); } while (0)

#define printvalueof(_SYMBOL_) do { uint8_t array_##_SYMBOL_[_SYMBOL_]; printf("sizeof([%s])=%zu\n", #_SYMBOL_, sizeof(array_##_SYMBOL_)); } while (0) 

typedef enum Enum { ENUM = 0 } enum_t;

typedef struct BitfieldInt { int bit : 1; } bitfield_int_t;
typedef struct BitfieldInt8 { int8_t bit : 1; } bitfield_int8_t;
typedef struct BitfieldInt16 { int16_t bit : 1; } bitfield_int16_t;
typedef struct BitfieldInt32 { int32_t bit : 1; } bitfield_int32_t;
typedef struct BitfieldInt64 { int64_t bit : 1; } bitfield_int64_t;
typedef struct BitfieldChar { char bit1 : 1; char bit2 : 1; } bitfield_char_t;
typedef struct BitfieldIntChar { int bit1 : 1; char bit2 : 1; } bitfield_intchar_t;
typedef struct BitfieldCharInt { char bit1 : 1; int bit2 : 1; } bitfield_charint_t;
typedef struct BitfieldIntInt { int bit1 : 1; int bit2 : 1; } bitfield_intint_t;

#if PROBLEMATIC
typedef void (function_t)(void);

typedef struct StructZero {
    uint8_t field[0];
} struct_zero_t;

typedef uint8_t (array_zero_t)[0];
#endif

int main(void)
{
    printsizeofnoninteger(void);
    printsizeof(char);
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
    printsizeofnoninteger(float);
    printsizeofnoninteger(double);
#if PROBLEMATIC
    printsizeofnoninteger(void);
#endif
    printsizeof(enum_t);
    printsizeofnoninteger(bool);
    printsizeofnoninteger(bitfield_int_t);
    printsizeofnoninteger(bitfield_int8_t);
    printsizeofnoninteger(bitfield_int16_t);
    printsizeofnoninteger(bitfield_int32_t);
    printsizeofnoninteger(bitfield_int64_t);
    printsizeofnoninteger(bitfield_char_t);
    printsizeofnoninteger(bitfield_intchar_t);
    printsizeofnoninteger(bitfield_charint_t);
    printsizeofnoninteger(bitfield_intint_t);
#if PROBLEMATIC
    printsizeofnoninteger(function_t);
#endif
    printissigned(int8_t);
    printisunsigned(uint8_t);
    printissigned(int16_t);
    printisunsigned(uint16_t);
    printissigned(int32_t);
    printisunsigned(uint32_t);
    printissigned(int64_t);
    printisunsigned(uint64_t);
    printissigned(intmax_t);
    printisunsigned(uintmax_t);
    printissigned(intptr_t);
    printisunsigned(uintptr_t);
    printsizeof(ptrdiff_t);
    printsizeof(wchar_t);
    printsizeof(wint_t);
    printisunsigned(size_t);
    printissigned(ssize_t);
    printsizeof(pid_t);
    printsizeof(time_t);
#if defined(_TIME_BITS)
    printvalueof(_TIME_BITS)
#endif
    printsizeof(dev_t);
    printsizeof(ino_t);
    printsizeof(mode_t);
    printsizeof(nlink_t);
    printsizeof(uid_t);
    printsizeof(gid_t);
    printsizeof(off_t);
    printsizeof(blksize_t);
    printsizeof(blkcnt_t);
    printsizeof(pthread_t);
    printsizeofnoninteger(pthread_mutex_t);
    printsizeofnoninteger(pthread_cond_t);
    printisunsigned(diminuto_ticks_t);
    printissigned(diminuto_sticks_t);
    printsizeof(diminuto_ipv4_t);
    printsizeofnoninteger(diminuto_ipv4_buffer_t);
    printsizeofnoninteger(diminuto_ipv6_t);
    printsizeofnoninteger(diminuto_ipv6_buffer_t);
    printsizeofnoninteger(diminuto_ip_buffer_t);
    printsizeof(diminuto_port_t);
    printsizeofnoninteger(diminuto_port_buffer_t);
    printsizeofnoninteger(diminuto_local_t);
    printsizeofnoninteger(diminuto_local_buffer_t);
    printsizeofnoninteger(diminuto_endpoint_buffer_t);
    printisunsigned(diminuto_unsigned_t);
    printissigned(diminuto_signed_t);
    printsizeofnoninteger(diminuto_path_t);
    printissigned(diminuto_lld_t);
    printisunsigned(diminuto_llu_t);
    printissigned(diminuto_fd_t);
#if PROBLEMATIC
    printsizeofnoninteger(struct_zero_t);
    printsizeofnoninteger(array_zero_t);
#endif
    printvalueof(PATH_MAX);
    printvalueof(UNIX_PATH_MAX);
    printvalueof(MAXPATHLEN);
    printvalueof(_POSIX_PATH_MAX);
    return 0;
}
