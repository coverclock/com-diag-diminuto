/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * COMMAND [ number ]
 * cat number | COMMAND
 *
 * where COMMAND may be
 *
 * dec, oct, hex, ntohs, htons, ntohl, htonl.
 *
 * EXAMPLES
 *
 * COMMAND 0xa
 * COMMAND 012
 * COMMAND 12
 * echo 0xa | COMMAND
 * echo 012 | COMMAND
 * echo 12 | COMMAND
 *
 * ABSTRACT
 *
 * Allows input and output in bases other than decimal in scripts.
 */

#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>

typedef enum Commands {
	OCT,
	DEC,
	HEX,
	NTOHS,
	HTONS,
	NTOHL,
	HTONL,
} command_t;

int main(int argc, char * argv[])
{
    const char * name;
    const char * end;
    char buffer[64];
    const char * string;
    const char * next;
    uint64_t value64;
    uint32_t value32;
    uint16_t value16;
    int file;
    int argn = 1;
    command_t command;

    name = strrchr(argv[0], '/');
    name = (name == (char *)0) ? argv[0] : name + 1;

	if (strcmp(name, "hex") == 0) {
		command = HEX;
	} else if (strcmp(name, "oct") == 0) {
		command = OCT;
	} else if (strcmp(name, "ntohs") == 0) {
		command = NTOHS;
	} else if (strcmp(name, "htons") == 0) {
		command = HTONS;
	} else if (strcmp(name, "ntohl") == 0) {
		command = NTOHL;
	} else if (strcmp(name, "htonl") == 0) {
		command = HTONL;
	} else {
		command = DEC;
	}

    file = (argc < 2) || ((argc == 2) && (strcmp(argv[1], "-") == 0));

    if (file) {
        string = fgets(buffer, sizeof(buffer), stdin);
    } else {
        string = argv[argn++];
    }

    if (string == (const char *)0) {
        errno = EINVAL;
        perror(name);
        return 2;
    }

    while (!0) {

		next = diminuto_number(string, &value64);
		if ((*next != '\0') && (!isspace(*next))) {
			perror(string);
			return 3;
		}

		switch (command) {
		case OCT:
			printf("0%llo\n", value64);
			break;
		case HEX:
			printf("0x%llx\n", value64);
			break;
		case NTOHS:
			printf("%u\n", ntohs(value16 = value64));
			break;
		case HTONS:
			printf("%u\n", htons(value16 = value64));
			break;
		case NTOHL:
			printf("%u\n", ntohl(value32 = value64));
			break;
		case HTONL:
			printf("%u\n", htonl(value32 = value64));
			break;
		case DEC:
		default:
			printf("%llu\n", value64);
			break;
		}

	    if (file) {
	        string = fgets(buffer, sizeof(buffer), stdin);
	    } else if (argn < argc) {
	        string = argv[argn++];
	    } else {
	    	string = (const char *)0;
	    }

        if (string == (const char *)0) {
        	break;
        }

    }

    return 0;
}
