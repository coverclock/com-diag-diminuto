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
#include "com/diag/diminuto/diminuto_time.h"
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
    int xonxoff = 0;
    int rtscts = 0;
    const char * device = "/dev/null";
    FILE * fp;
    int input;
    int output;
    size_t current = 0;
    int end = 0;
    diminuto_usec_t now;
    diminuto_usec_t then;
    double elapsed;
    size_t count;
    int bitspercharacter;
    int running = 0;

    program = ((program = strchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "125678D:b:ehlmnos")) >= 0) {

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

    	}

    }

    fd = open("/dev/null", O_RDWR);
    ASSERT(fd >= 0);

    rc = diminuto_serial_set(fd, bitspersecond, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts);
    ASSERT(rc < 0);

    rc = close(fd);
    ASSERT(rc == 0);

    CHECKPOINT("\"%s\" %d %d%c%d %s %s %s\n", device, bitspersecond, databits, "NOE"[paritybit], stopbits, modemcontrol ? "modem" : "local", xonxoff ? "xonxoff" : "noswflow", rtscts ? "rtscts" : "nohwflow");

    fd = open(device, O_RDWR);
    ASSERT(fd >= 0);

    rc = diminuto_serial_set(fd, bitspersecond, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts);
    ASSERT(rc == 0);

    rc = diminuto_serial_raw(fd);
    ASSERT(rc == 0);

    fp = fdopen(fd, "r+");
    ASSERT(fp != (FILE *)0);

    rc = diminuto_serial_unbuffered(fp);
    ASSERT(rc == 0);

    bitspercharacter = 1 + databits + ((paritybit != 0) ? 1 : 0) + stopbits;
    output = 0x00;

    while (!0) {

        count = 0;
    	then = diminuto_time_elapsed();

    	while (!0) {

       		fputc(output, fp);
     		input = fgetc(fp);

     		if (!running) {
     			CHECKPOINT("running\n");
     			running = !0;
     		}

      		ASSERT(input == output);

      		output = (output + 1) % (1 << (sizeof(char) * 8));

      		++count;

        	now = diminuto_time_elapsed();
        	if ((now - then) >= 1000000) {
        		break;
        	}

    	}

    	elapsed = (now - then) / 1000000.0;
    	CHECKPOINT("%lu characters in %.6lf seconds at %d bits per second and %d bits per character\n", count, elapsed, bitspersecond, bitspercharacter);

    }

    EXIT();
}
