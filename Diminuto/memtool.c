/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * memtool [ -d ] [ -a ADDDRESS ] [ -l BYTES ] [ -[1|2|4|8] ADDRESS ] [ -[s|c|w] NUMBER | -r ] [ -u USECONDS ] [ ... ]
 *
 * EXAMPLES
 *
 * memtool -a 0xffff8000 -l 4096 -4 0xffff8004 -s 0x1 -u 1000 -c 0x1
 *
 * ABSTRACT
 *
 * Allows manipulation of bytes, shorts, longs, and long longs at arbitary
 * real memory addresses. Probably needs to be run as root.
 */

#include "diminuto_map.h"
#include "diminuto_number.h"
#include "diminuto_delay.h"
#include "diminuto_barrier.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static const char * program = "memtool";
static int debug = 0;

#define OPERATE(_POINTER_, _TYPE_, _VALUEP_, _FORMAT_) \
    do { \
        _TYPE_ word; \
        _TYPE_ value; \
        value = *_VALUEP_; \
        switch (opt) { \
        case 'c': \
            memcpy(&word, _POINTER_, sizeof(_TYPE_)); \
            word &= ~value; \
            memcpy(_POINTER_, &word, sizeof(_TYPE_)); \
            break; \
        case 'r': \
            memcpy(&word, _POINTER_, sizeof(_TYPE_)); \
            printf(_FORMAT_ "\n", word); \
            break; \
        case 's': \
            memcpy(&word, _POINTER_, sizeof(_TYPE_)); \
            word |= value; \
            memcpy(_POINTER_, &word, sizeof(_TYPE_)); \
            break; \
        case 'w': \
            memcpy(_POINTER_, &word, sizeof(_TYPE_)); \
            word = value; \
            break; \
        } \
        *_VALUEP_ = value; \
    } while (0)

static int operate(
    int opt,
    uintptr_t address,
    size_t length,
    uintptr_t pointer,
    size_t size,
    void ** addressp,
    size_t * lengthp,
    uint64_t * valuep
) {
    void * base;
    void * effective;

    if (length == 0) {
        base = diminuto_map(pointer, size, addressp, lengthp);
        effective = base;
    } else {
        base = diminuto_map(address, length, addressp, lengthp);
        effective = base + (pointer - address);
    }

    if (debug) {
        fprintf(stderr, "%s: a=%p l=%u p=%lu s=%u b=%p e=%p v=%llx opt=%c\n",
            program, address, length, pointer, size, base, effective,
            *valuep, opt);
    }

    if (effective == (void *)0) {
        return -1;
    }

    diminuto_barrier();

    switch (size) {

    case sizeof(uint8_t):
        OPERATE(effective, uint8_t, valuep, "%u");
        break;

    case sizeof(uint16_t):
        OPERATE(effective, uint16_t, valuep, "%u");
        break;

    case sizeof(uint32_t):
        OPERATE(effective, uint32_t, valuep, "%lu");
        break;

    case sizeof(uint64_t):
        OPERATE(effective, uint64_t, valuep, "%llu");
        break;

    }

    diminuto_barrier();

    if (length == 0) {
        diminuto_unmap(*addressp, *lengthp);
        *addressp = (void *)0;
        *lengthp = 0;
    }

   return 0; 
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -d ] [ -a ADDDRESS ] [ -l BYTES ] [ -[1|2|4|8] ADDRESS ] [ -r | -[s|c|w] NUMBER ] [ -u USECONDS ] [ ... ]\n", program);
    fprintf(stderr, "       -d            Enable debug mode\n");
    fprintf(stderr, "       -a ADDRESS    Optionally map region at ADDRESS\n");
    fprintf(stderr, "       -l BYTES      Optionally map BYTES in length\n");
    fprintf(stderr, "       -1 ADDRESS    Use byte at ADDRESS\n");
    fprintf(stderr, "       -2 ADDRESS    Use halfword at ADDRESS\n");
    fprintf(stderr, "       -4 ADDRESS    Use word at ADDRESS\n");
    fprintf(stderr, "       -8 ADDRESS    Use doubleword at ADDRESS\n");
    fprintf(stderr, "       -s NUMBER     Set NUMBER mask at ADDRESS\n");
    fprintf(stderr, "       -t            Proceed if the last result was !0\n");
    fprintf(stderr, "       -c NUMBER     Clear NUMBER mask at ADDRESS\n");
    fprintf(stderr, "       -r            Read ADDRESS\n");
    fprintf(stderr, "       -f            Proceed if the last result was 0\n");
    fprintf(stderr, "       -u USECONDS   Sleep for USECONDS microseconds\n");
    fprintf(stderr, "       -w NUMBER     Write NUMBER to ADDRESS\n");
    fprintf(stderr, "       -?            Print menu\n");
}

int main(int argc, char * argv[])
{
    uint64_t value = 0;
    uintptr_t address = 0;
    size_t length = 0;
    void * unaddress = 0;
    size_t unlength = 0;
    uintptr_t pointer = 0;
    size_t size = 0;
    int opt;
    extern char * optarg;
    int done = 0;
    int error = 0;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "1:2:4:8:a:c:dfl:rs:tu:w:?")) >= 0) {

        switch (opt) {

        case '1':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint8_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, pointer); }
            }
            break;

        case '2':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint16_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, pointer); }
            }
            break;

        case '4':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint32_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, pointer); }
            }
            break;

        case '8':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint64_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, pointer); }
            }
            break;

        case 'a':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                address = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, address); }
            }
            break;

        case 'c':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c %llx\n", program, opt, value); }
                error = (operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0);
            }
            break;

        case 'd':
            debug = !0;
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            break;

        case 'f':
            done = value != 0;
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            break;

        case 'l':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                length = value;
                if (debug) { fprintf(stderr, "%s -%c %z\n", program, opt, length); }
            }
            break;

        case 'r':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            error = (operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0);
            break;

        case 's':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c %llx\n", program, opt, value); }
                error = (operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0);
            }
            break;

        case 't':
            done = value == 0;
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            break;

        case 'u':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c %llu\n", program, opt, value); }
                diminuto_delay(value, 0);
            }
            break;

        case 'w':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c %llx\n", program, opt, value); }
                error = (operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0);
            }
            break;

        case '?':
            usage();
            break;

        default:
            error = !0;
            break;

        }

        if (error) {
            break;
        }

        if (done) {
            break;
        }

    }

    if (unlength != 0) {
        diminuto_unmap(unaddress, unlength);
    }

    if (error) {
        usage();
        return 1;
    }

    return 0;
}
