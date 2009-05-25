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
 * memtool<BR>
 *
 * EXAMPLES
 *
 * ABSTRACT
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

static void usage(FILE * stream, char * argv[])
{
    fprintf(stream, "usage: %s [ -d ] [ -a ADDDRESS ] [ -l BYTES ] [ -[1|2|4|8] ADDRESS ] [ -[s|c|r|w] NUMBER ] [ -u USECONDS ] [ ... ]\n", argv[0]);
    fprintf(stream, "       -d            Enable debug mode\n");
    fprintf(stream, "       -a ADDRESS    Map region starting at ADDRESS\n");
    fprintf(stream, "       -l BYTES      Map region BYTES in length\n");
    fprintf(stream, "       -1 ADDRESS    Use byte at ADDRESS\n");
    fprintf(stream, "       -2 ADDRESS    Use halfword at ADDRESS\n");
    fprintf(stream, "       -4 ADDRESS    Use word at ADDRESS\n");
    fprintf(stream, "       -8 ADDRESS    Use doubleword at ADDRESS\n");
    fprintf(stream, "       -s NUMBER     Set NUMBER mask at ADDRESS\n");
    fprintf(stream, "       -t            Proceed if the last result was !0\n");
    fprintf(stream, "       -c NUMBER     Clear NUMBER mask at ADDRESS\n");
    fprintf(stream, "       -r            Read ADDRESS\n");
    fprintf(stream, "       -f            Proceed if the last result was 0\n");
    fprintf(stream, "       -w NUMBER     Write NUMBER to ADDRESS\n");
    fprintf(stream, "       -u USECONDS   Sleep for USECONDS microseconds\n");
    fprintf(stream, "       -?            Print menu\n");
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
    int done = !0;
    int error = 0;

    while ((opt = getopt(argc, argv, "1:2:4:8:a:c:dfl:rs:tuw:?")) >= 0) {

        switch (opt) {

        case '1':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                size = sizeof(uint8_t);
                pointer = value;
            }
            break;

        case '2':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                size = sizeof(uint16_t);
                pointer = value;
            }
            break;

        case '4':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                size = sizeof(uint32_t);
                pointer = value;
            }
            break;

        case '8':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                size = sizeof(uint64_t);
                pointer = value;
            }
            break;

        case 'a':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                address = value;
            }
            break;

        case 'c':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else if ((error = operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0)) {
                perror(optarg);
            }
            break;

        case 'd':
            debug = !0;
            break;

        case 'f':
            done = value != 0;
            break;

        case 'l':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                length = value;
            }
            break;

        case 'r':
            if ((error = operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0)) {
                perror(optarg);
            }
            break;

        case 's':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else if ((error = operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0)) {
                perror(optarg);
            }
            break;

        case 't':
            done = value == 0;
            break;

        case 'u':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                diminuto_delay(value, 0);
            }
            break;

        case 'w':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else if ((error = operate(opt, address, length, pointer, size, &unaddress, &unlength, &value) != 0)) {
                perror(optarg);
            }
            break;

        case '?':
            usage(stderr, argv);
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
        usage(stderr, argv);
        return 1;
    }

    return 0;
}
