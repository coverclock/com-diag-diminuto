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
    fprintf(stream, "       -c NUMBER     Clear NUMBER mask at ADDRESS\n");
    fprintf(stream, "       -r            Read ADDRESS\n");
    fprintf(stream, "       -w NUMBER     Write NUMBER to ADDRESS\n");
    fprintf(stream, "       -u USECONDS   Sleep for USECONDS microseconds\n");
    fprintf(stream, "       -?            Print menu\n");
}

static int operate(
    int opt,
    uintptr_t address,
    size_t length,
    uintptr_t pointer,
    size_t size,
    void ** addressp,
    size_t * lengthp,
    uint64_t value64
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
        {
            uint8_t * pointer;
            uint8_t value;
            pointer = (uint8_t *)effective;
            value = value64;
            switch (opt) {
            case 'c':
                *pointer &= ~value;
                break;
            case 'r':
                value = *pointer;
                printf("%u\n", value);
                break;
            case 's':
                *pointer |= value;
                break;
            case 'w':
                *pointer = value;
                break;
            }
        }
        break;

    case sizeof(uint16_t):
        {
            uint16_t * pointer;
            uint16_t value;
            pointer = (uint16_t *)effective;
            value = value64;
            switch (opt) {
            case 'c':
                *pointer &= ~value;
                break;
            case 'r':
                value = *pointer;
                printf("%u\n", value);
                break;
            case 's':
                *pointer |= value;
                break;
            case 'w':
                *pointer = value;
                break;
            }
        }
        break;

    case sizeof(uint32_t):
        {
            uint32_t * pointer;
            uint32_t value;
            pointer = (uint32_t *)effective;
            value = value64;
            switch (opt) {
            case 'c':
                *pointer &= ~value;
                break;
            case 'r':
                value = *pointer;
                printf("%lu\n", value);
                break;
            case 's':
                *pointer |= value;
                break;
            case 'w':
                *pointer = value;
                break;
            }
        }
        break;

    case sizeof(uint64_t):
        {
            uint64_t * pointer;
            uint64_t value;
            pointer = (uint64_t *)effective;
            value = value64;
            switch (opt) {
            case 'c':
                *pointer &= ~value;
                break;
            case 'r':
                value = *pointer;
                printf("%llu\n", value);
                break;
            case 's':
                *pointer |= value;
                break;
            case 'w':
                *pointer = value;
                break;
            }
        }
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

int main(int argc, char * argv[])
{
    int debug = 0;
    uint64_t value = 0;
    uintptr_t address = 0;
    size_t length = 0;
    void * unaddress = 0;
    size_t unlength = 0;
    uintptr_t pointer = 0;
    size_t size = 0;
    int opt;
    extern char * optarg;
    int error = 0;

    while ((opt = getopt(argc, argv, "dal1248scrwu?")) >= 0) {

        switch (opt) {

        case 'd':
            debug = !0;
            break;

        case 'a':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                address = value;
            }
            break;

        case 'l':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                length = value;
            }
            break;

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

        case 's':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                operate(opt, address, length, pointer, size, &unaddress, &unlength, value);
            }
            break;

        case 'c':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                operate(opt, address, length, pointer, size, &unaddress, &unlength, value);
            }
            break;

        case 'r':
            operate(opt, address, length, pointer, size, &unaddress, &unlength, value);
            break;

        case 'w':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                operate(opt, address, length, pointer, size, &unaddress, &unlength, value);
            }
            break;

        case 'u':
            if ((error = diminuto_number(optarg, &value) != '\0')) {
                perror(optarg);
            } else {
                diminuto_delay(value, 0);
            }
            break;

        case '?':
            usage(stdout, argv);
            break;

        default:
            error = !0;
            break;

        }

        if (error) {
            usage(stderr, argv);
            exit(1);
        }

    }

    if (unlength != 0) {
        diminuto_unmap(unaddress, unlength);
    }

    return 0;
}
