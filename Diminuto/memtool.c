/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2011 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * memtool [ -d ] [ -a ADDDRESS ] [ -l BYTES ] [ -[1|2|4|8] ADDRESS ] [ -m NUMBER ] [ -[s|S|c|C|w] NUMBER | -r ] [ -u USECONDS ] [ ... ]
 *
 * EXAMPLES
 *
 * memtool -a 0xffff8000 -l 4096 -4 0xffff8004 -s 0x1 -u 1000 -c 0x1
 *
 * ABSTRACT
 *
 * Allows manipulation of bytes, shorts, longs, and long longs at arbitary
 * real memory addresses. Probably needs to be run as root.
 *
 * N.B. Although memtool supports long longs (eight byte) operations, not all
 * underlying memory subsystems and memory mapped devices seem to be happy with
 * that. While one, two, and four byte operations seem to work fine, I have
 * seen wackiness ensue with eight byte operations. This may be an artifact of
 * the compiler and/or the processor and how either implements (or emulates)
 * sixty-four bit operations. Study continues.
 */

#include "diminuto_map.h"
#include "diminuto_number.h"
#include "diminuto_delay.h"
#include "diminuto_barrier.h"
#include "diminuto_core.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

static const char * program = "memtool";
static int debug = 0;

#define OPERATE(_OPT_, _DATAP_, _MASK_, _VALUEP_, _TYPE_, _FORMAT_) \
    do { \
        volatile _TYPE_ * _datap; \
        _TYPE_ _mask; \
        _TYPE_ _value; \
        _TYPE_ _datum = 1; \
        _datap = (_TYPE_ *)_DATAP_; \
        _mask = _MASK_; \
        _value = *_VALUEP_; \
        if (debug && ((_OPT_) != 'r')) { fprintf(stderr, "%s: t=%s v=0x%llx\n", program, # _TYPE_, islower(_OPT_) ? (diminuto_unsigned_t)_value : (diminuto_unsigned_t)_datum); } \
        diminuto_barrier(); \
        switch (_OPT_) { \
        case 'C': _datum <<= (*_VALUEP_); *_datap &= ~_datum;							break; \
        case 'S': _datum <<= (*_VALUEP_); *_datap = ((*_datap) & (~_mask)) | _datum;	break; \
        case 'c': *_datap &= ~_value;													break; \
        case 'r': printf(_FORMAT_ "\n", _value = *_datap);								break; \
        case 's': *_datap = ((*_datap) & (~_mask)) | _value;							break; \
        case 'w': *_datap = _value;														break; \
        } \
        diminuto_barrier(); \
        if (debug && ((_OPT_) == 'r')) { fprintf(stderr, "%s: t=%s v=0x%llx\n", program, # _TYPE_, (diminuto_unsigned_t)_value); } \
        *_VALUEP_ = _value; \
    } while (0)

static int operate(
    void ** basep,
    int opt,
    uintptr_t address,
    size_t length,
    uintptr_t pointer,
    size_t size,
    void ** addressp,
    size_t * lengthp,
    diminuto_unsigned_t mask,
    diminuto_unsigned_t * valuep
) {
    void * datap;

    if (*basep != (void *)0) {
        /* Do nothing. */
    } else if (length == 0) {
        *basep = diminuto_map_map(pointer, size, addressp, lengthp);
    } else {
        *basep = diminuto_map_map(address, length, addressp, lengthp);
    }

    datap = (length == 0) ? *basep : (char *)(*basep) + (pointer - address);

    if (debug) {
        fprintf(stderr, "%s: a=%p l=%u p=%p s=%u b=%p d=%p m=0x%llx v=%p o=%c\n",
            program, (void *)address, length, (void *)pointer, size, *basep,
            datap, mask, valuep, opt);
    }

    if (*basep == (void *)0) {
        return -1;
    }

    switch (size) {
    case sizeof(uint8_t):	OPERATE(opt, datap, mask, valuep, uint8_t,  "%u");		break;
    case sizeof(uint16_t):	OPERATE(opt, datap, mask, valuep, uint16_t, "%u");		break;
    case sizeof(uint32_t):	OPERATE(opt, datap, mask, valuep, uint32_t, "%u");		break;
    case sizeof(uint64_t):	OPERATE(opt, datap, mask, valuep, uint64_t, "%llu");	break;
    }


    if (length == 0) {
        diminuto_map_unmap(addressp, lengthp);
        *basep = (void *)0;
    }

   return 0; 
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -d ] [ -o ] [ -a ADDDRESS ] [ -l BYTES ] [ -[1|2|4|8] ADDRESS ] [ -m NUMBER ] [ -r | -[s|S|c|C|w] NUMBER ] [ -u USECONDS ] [ -t | -f ] [ ... ]\n", program);
    fprintf(stderr, "       -1 ADDRESS    Use byte at ADDRESS\n");
    fprintf(stderr, "       -2 ADDRESS    Use halfword at ADDRESS\n");
    fprintf(stderr, "       -4 ADDRESS    Use word at ADDRESS\n");
    fprintf(stderr, "       -8 ADDRESS    Use doubleword at ADDRESS\n");
    fprintf(stderr, "       -C NUMBER     Clear 1<<NUMBER mask at ADDRESS\n");
    fprintf(stderr, "       -S NUMBER     Set 1<<NUMBER mask at ADDRESS\n");
    fprintf(stderr, "       -a ADDRESS    Optionally map region at ADDRESS\n");
    fprintf(stderr, "       -c NUMBER     Clear NUMBER mask at ADDRESS\n");
    fprintf(stderr, "       -d            Enable debug mode\n");
    fprintf(stderr, "       -f            Proceed if the last result was 0\n");
    fprintf(stderr, "       -l BYTES      Optionally map BYTES in length\n");
    fprintf(stderr, "       -m NUMBER     Mask at ADDRESS with ~NUMBER prior to any set\n");
    fprintf(stderr, "       -o            Enable core dumps\n");
    fprintf(stderr, "       -r            Read ADDRESS\n");
    fprintf(stderr, "       -s NUMBER     Set NUMBER mask at ADDRESS\n");
    fprintf(stderr, "       -t            Proceed if the last result was !0\n");
    fprintf(stderr, "       -u USECONDS   Sleep for USECONDS microseconds\n");
    fprintf(stderr, "       -w NUMBER     Write NUMBER to ADDRESS\n");
    fprintf(stderr, "       -?            Print menu\n");
}

int main(int argc, char * argv[])
{
    diminuto_unsigned_t value = 0;
    diminuto_unsigned_t mask = 0;
    uintptr_t address = 0;
    size_t length = 0;
    void * base = 0;
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

    while ((opt = getopt(argc, argv, "1:2:4:8:C:I:S:a:c:dfi:l:m:ors:tu:w:?")) >= 0) {

        switch (opt) {

        case '1':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint8_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)pointer); }
            }
            break;

        case '2':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint16_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)pointer); }
            }
            break;

        case '4':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint32_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)pointer); }
            }
            break;

        case '8':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint64_t);
                pointer = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)pointer); }
            }
            break;

        case 'a':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                address = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)address); }
                diminuto_map_unmap(&unaddress, &unlength);
                base = (void *)0;
            }
            break;

        case 'C':
        case 'c':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, value); }
                error = (operate(&base, opt, address, length, pointer, size, &unaddress, &unlength, 0, &value) != 0);
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
                if (debug) { fprintf(stderr, "%s -%c %zu\n", program, opt, length); }
            }
            break;

        case 'm':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                mask = value;
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, mask); }
            }
            break;

        case 'o':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            diminuto_core_enable();
            break;

        case 'r':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            error = (operate(&base, opt, address, length, pointer, size, &unaddress, &unlength, 0, &value) != 0);
            break;

        case 'S':
        case 's':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, value); }
                error = (operate(&base, opt, address, length, pointer, size, &unaddress, &unlength, mask, &value) != 0);
            }
            break;

        case 't':
            done = (value == 0);
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
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, value); }
                error = (operate(&base, opt, address, length, pointer, size, &unaddress, &unlength, 0, &value) != 0);
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

    diminuto_map_unmap(&unaddress, &unlength);
    base = (void *)0;

    if (error) {
        usage();
        return 1;
    }

    return 0;
}
