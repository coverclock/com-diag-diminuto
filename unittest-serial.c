/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_phex.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    const char * device = "/dev/null";
    FILE * fp;
    int datum;
    size_t current = 0;
    int end = 0;

    program = ((program = strchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "125678D:b:elmno")) >= 0) {

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

    	case 'e':
    		paritybit = 2;
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

    	}

    }

    CHECKPOINT("\"%s\" %d %d%c%d %s\n", device, bitspersecond, databits, "NOE"[paritybit], stopbits, modemcontrol ? "modem" : "local");

    fd = open(device, O_RDWR);
    ASSERT(fd >= 0);

    rc = diminuto_serial_set(fd, bitspersecond, databits, paritybit, stopbits, modemcontrol);
    ASSERT(rc == 0);

    rc = diminuto_serial_raw(fd);
    ASSERT(rc == 0);

    fp = fdopen(fd, "r+");
    ASSERT(fp != (FILE *)0);

    rc = diminuto_serial_unbuffered(fp);
    ASSERT(rc == 0);

    while (!0) {

    	datum = fgetc(fp);
    	diminuto_phex_emit(stderr, datum, 80, 0, 0, 0, &current, &end, !0);
    	fputc(datum, fp);

    }

    EXIT();
}
