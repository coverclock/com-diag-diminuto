/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * There's a lot of duplication here, but I'm paranoid that way.
 */

#include <stdio.h>
#include <pthread.h>
#include "com/diag/diminuto/diminuto_types.h"

#define printsizeof(_TYPE_) printf("sizeof(%s)=%zu\nsizeof(%s*)=%zu\n", #_TYPE_, sizeof(_TYPE_), #_TYPE_, sizeof(_TYPE_*))

typedef enum Enum { ENUM = 0 } enum_t;

typedef void (function_t)(void);

int main(void)
{
    printsizeof(char);
    printsizeof(signed char);
    printsizeof(unsigned char);
    printsizeof(short);
    printsizeof(signed short);
    printsizeof(unsigned short);
    printsizeof(int);
    printsizeof(signed int);
    printsizeof(unsigned int);
    printsizeof(long);
    printsizeof(signed long);
    printsizeof(unsigned long);
    printsizeof(long long);
    printsizeof(signed long long);
    printsizeof(unsigned long long);
    printsizeof(float);
    printsizeof(double);
    printsizeof(void);
    printsizeof(enum_t);
    printsizeof(function_t);
    printsizeof(int8_t);
    printsizeof(uint8_t);
    printsizeof(int16_t);
    printsizeof(uint16_t);
    printsizeof(int32_t);
    printsizeof(uint32_t);
    printsizeof(int64_t);
    printsizeof(uint64_t);
    printsizeof(intptr_t);
    printsizeof(uintptr_t);
    printsizeof(size_t);
    printsizeof(ssize_t);
    printsizeof(off_t);
    printsizeof(pid_t);
    printsizeof(pthread_t);
    printsizeof(pthread_mutex_t);
    printsizeof(pthread_cond_t);
    printsizeof(diminuto_ticks_t);
    printsizeof(diminuto_ipv4_t);
    printsizeof(diminuto_port_t);
    printsizeof(diminuto_unsigned_t);
    printsizeof(diminuto_signed_t);
    return 0;
}
