/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_serial.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    int rc;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;
    const char * program;
    int fd;
    int bitspersecond = 115200;
    int databits = 8;
    int paritybit = 0;
    int stopbits = 1;
    int modemcontrol = 0;
    int xonxoff = 0;
    int rtscts = 0;
    const char * device = "/dev/null";
    FILE * fp;
    int datum;
    int debug = 0;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "?125678D:b:dehlmnos")) >= 0) {

    	switch (opt) {

    	case '1':
    		stopbits = 1;
    		break;

    	case '2':
    		stopbits = 2;
    		break;

    	case '5':
    		databits = 5;
    		break;

    	case '6':
    		databits = 6;
    		break;

    	case '7':
    		databits = 7;
    		break;

    	case '8':
    		databits = 8;
    		break;

    	case 'D':
    		device = optarg;
    		break;

    	case 'b':
    		bitspersecond = strtoul(optarg, (char **)0, 0);
    		break;

        case 'd':
           	debug = !0;
           	break;

    	case 'e':
    		paritybit = 2;
    		break;

    	case 'h':
    		rtscts = !0;
    		break;

    	case 'l':
    		modemcontrol = 0;
    		break;

    	case 'm':
    		modemcontrol = !0;
    		break;

    	case 'n':
    		paritybit = 0;
    		break;

    	case 'o':
    		paritybit = 1;
    		break;

    	case 's':
    		xonxoff = !0;
    		break;

        case '?':
            fprintf(stderr, "usage: %s [ -d ] [ -D DEVICE ] [ -b SPEED ] [ -(5|6|7|8) ] [ -(1|2) ] [ -(o|e|n) ] [ -s ] [ -h ] [ -(l|m) ]\n", program);
            break;

    	}

    }

    if (debug) {
        fprintf(stderr, "%s: -D %s -b %d -%d -%d %s%s%s%s%s\n", program, device, bitspersecond, databits, stopbits, (!paritybit) ? "-n " : (paritybit & 1) ? "-o " : "-e ", xonxoff ? "-s " : "", rtscts ? "-h " : "", modemcontrol ? "-m " : "-l ", debug ? "-d" : "");
    }

    fd = open(device, O_RDWR);
    assert(fd >= 0);

    rc = diminuto_serial_set(fd, bitspersecond, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts);
    assert(rc == 0);

    rc = diminuto_serial_raw(fd);
    assert(rc == 0);

    fp = fdopen(fd, "r+");
    assert(fp != (FILE *)0);

    rc = diminuto_serial_unbuffered(fp);
    assert(rc == 0);

    while ((datum = fgetc(fp)) != EOF) {
        if (debug) {
            rc = fputc(datum, stdout);
            assert(rc == datum);
        }
        rc = fputc(datum, fp);
        assert(rc == datum);
    }

    rc = fclose(fp);
    assert(rc == 0);

    exit(0);
}
