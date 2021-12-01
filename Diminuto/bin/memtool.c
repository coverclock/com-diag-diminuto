/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2013 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Manipulate memory registers using memory-mapped I/O.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * memtool [ -d ] [ -a ADDDRESS -l BYTES ] [ -[1|2|4|8] OFFSET ] [ -m NUMBER ] [ -[s|S|c|C|w] NUMBER | -r ] [ -u USECONDS ] [ ... ]
 *
 * EXAMPLES
 *
 * (These two commands are equivalent.)
 *
 * memtool -a 0xffff8000 -l 4096 -4 0x4        -s 0x1 -u 1000 -c 0x1 -4 0x8        -r<BR>
 * memtool                       -4 0xffff8004 -s 0x1 -u 1000 -c 0x1 -4 0xffff8008 -r<BR>
 *
 * ABSTRACT
 *
 * Allows manipulation of bytes, shorts, longs, and long longs at arbitrary
 * real memory addresses. Should probably only be run as root.
 */

#include "com/diag/diminuto/diminuto_map.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_barrier.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

static const char * program = "memtool";
static int debug = 0;

#define OPERATE(_TYPE_) \
    do { \
        volatile _TYPE_ * _datap; \
        _TYPE_ _mask; \
        _TYPE_ _value; \
        _TYPE_ _datum = 1; \
        _datap = (_TYPE_ *)datap; \
        _mask = mask; \
        _value = *valuep; \
        if (debug && (opt != 'r')) { fprintf(stderr, "%s: before t=%s v=0x%llx\n", program, # _TYPE_, islower(opt) ? (diminuto_llu_t)_value : (diminuto_llu_t)_datum); } \
        diminuto_barrier(); \
        switch (opt) { \
        case 'C': _datum <<= (*valuep); *_datap &= ~_datum;							break; \
        case 'S': _datum <<= (*valuep); *_datap = ((*_datap) & (~_mask)) | _datum;	break; \
        case 'c': *_datap &= ~_value;												break; \
        case 'r': _value = *_datap; 												break; \
        case 's': *_datap = ((*_datap) & (~_mask)) | _value;						break; \
        case 'w': *_datap = _value;													break; \
        } \
        diminuto_barrier(); \
        if (debug && (opt == 'r')) { fprintf(stderr, "%s: after t=%s v=0x%llx\n", program, # _TYPE_, (diminuto_llu_t)_value); } \
        if (opt == 'r') { printf("%llu\n", (diminuto_llu_t)_value); } \
        *valuep = _value; \
    } while (0)

static int operate(
    void ** basep,
    int opt,
    uintptr_t address,
    size_t length,
    uintptr_t offset,
    size_t size,
    void ** addressp,
    size_t * lengthp,
    diminuto_unsigned_t mask,
    diminuto_unsigned_t * valuep
) {
    void * datap;
    uintptr_t effective;

    if (*basep != (void *)0) {
        /* Do nothing. */
    } else if (length == 0) {
        *basep = diminuto_map_map(offset, size, addressp, lengthp);
    } else {
        *basep = diminuto_map_map(address, length, addressp, lengthp);
    }

    effective = (length == 0) ? offset : address + offset;
    datap = (length == 0) ? *basep : (char *)(*basep) + offset;

    if (debug) {
        fprintf(stderr, "%s: address=%p length=%zu offset=%p size=%zu effective=%p base=%p data=%p mask=0x%llx value=%p option=%c\n",
            program, (void *)address, length, (void *)offset, size, (void *)effective, *basep, datap, (diminuto_llu_t)mask, (void *)valuep, opt);
    }

    if (*basep == (void *)0) {
        return -1;
    }

    switch (size) {
    case sizeof(uint8_t):	OPERATE(uint8_t);	break;
    case sizeof(uint16_t):	OPERATE(uint16_t);	break;
    case sizeof(uint32_t):	OPERATE(uint32_t);	break;
    case sizeof(uint64_t):	OPERATE(uint64_t);	break;
    }


    if (length == 0) {
        diminuto_map_unmap(addressp, lengthp);
        *basep = (void *)0;
    }

   return 0; 
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -d ] [ -o ] [ -a ADDDRESS -l BYTES ] [ -[1|2|4|8] OFFSET ] [ -m NUMBER ] [ -r | -[s|S|c|C|w] NUMBER ] [ -u USECONDS ] [ -t | -f ] [ ... ]\n", program);
    fprintf(stderr, "       -1 OFFSET     Use byte at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -2 OFFSET     Use halfword at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -4 OFFSET     Use word at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -8 OFFSET     Use doubleword at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -C NUMBER     Clear 1<<NUMBER mask at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -S NUMBER     Set 1<<NUMBER mask at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -a ADDRESS    Map region at ADDRESS\n");
    fprintf(stderr, "       -c NUMBER     Clear NUMBER mask at ADDRESS+OFFSET\n");
    fprintf(stderr, "       -d            Enable debug mode\n");
    fprintf(stderr, "       -f            Proceed if the last result was 0\n");
    fprintf(stderr, "       -l BYTES      Map length BYTES at ADDRESS\n");
    fprintf(stderr, "       -m NUMBER     Mask at ADDRESS+OFFSET with ~NUMBER prior to subsequent sets\n");
    fprintf(stderr, "       -o            Enable core dumps\n");
    fprintf(stderr, "       -r            Read ADDRESS+OFFSET\n");
    fprintf(stderr, "       -s NUMBER     Set NUMBER mask at ADDRESS+OFFSET\n");
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
    uintptr_t offset = 0;
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
                offset = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)offset); }
            }
            break;

        case '2':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint16_t);
                offset = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)offset); }
            }
            break;

        case '4':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint32_t);
                offset = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)offset); }
            }
            break;

        case '8':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                size = sizeof(uint64_t);
                offset = value;
                if (debug) { fprintf(stderr, "%s -%c %p\n", program, opt, (void *)offset); }
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
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, (diminuto_llu_t)value); }
                error = (operate(&base, opt, address, length, offset, size, &unaddress, &unlength, 0, &value) != 0);
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
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, (diminuto_llu_t)mask); }
            }
            break;

        case 'o':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            diminuto_core_enable();
            break;

        case 'r':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            error = (operate(&base, opt, address, length, offset, size, &unaddress, &unlength, 0, &value) != 0);
            break;

        case 'S':
        case 's':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, (diminuto_llu_t)value); }
                error = (operate(&base, opt, address, length, offset, size, &unaddress, &unlength, mask, &value) != 0);
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
                if (debug) { fprintf(stderr, "%s -%c %llu\n", program, opt, (diminuto_llu_t)value); }
                value *= diminuto_delay_frequency();
                value /= 1000000;
                diminuto_delay(value, 0);
            }
            break;

        case 'w':
            if ((error = (*diminuto_number(optarg, &value) != '\0'))) {
                perror(optarg);
            } else {
                if (debug) { fprintf(stderr, "%s -%c 0x%llx\n", program, opt, (diminuto_llu_t)value); }
                error = (operate(&base, opt, address, length, offset, size, &unaddress, &unlength, 0, &value) != 0);
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
