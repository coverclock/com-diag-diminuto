/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    const char * program = "log";
    const char * name = DIMINUTO_LOG_IDENT_DEFAULT;
    int option = DIMINUTO_LOG_OPTION_DEFAULT;
    int facility = DIMINUTO_LOG_FACILITY_DEFAULT;
    diminuto_log_mask_t mask = DIMINUTO_LOG_MASK_DEFAULT;
    int priority = DIMINUTO_LOG_PRIORITY_DEFAULT;
    int opt;
    extern char * optarg;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "N:O:F:Eacewnid?")) >= 0) {

        switch (opt) {

        case 'N':
        	name = optarg;
        	break;

        case 'O':
        	option = atoi(optarg);
        	break;

        case 'F':
        	facility = atoi(optarg);
        	break;

        case 'E':
            mask = DIMINUTO_LOG_MASK_EMERGENCY;
            priority = DIMINUTO_LOG_PRIORITY_EMERGENCY;
            break;

        case 'a':
            mask = DIMINUTO_LOG_MASK_ALERT;
            priority = DIMINUTO_LOG_PRIORITY_ALERT;
        	break;

        case 'c':
            mask = DIMINUTO_LOG_MASK_CRITICAL;
            priority = DIMINUTO_LOG_PRIORITY_CRITICAL;
        	break;

        case 'e':
            mask = DIMINUTO_LOG_MASK_ERROR;
            priority = DIMINUTO_LOG_PRIORITY_ERROR;
        	break;

        case 'w':
            mask = DIMINUTO_LOG_MASK_WARNING;
            priority = DIMINUTO_LOG_PRIORITY_WARNING;
        	break;

        case 'n':
            mask = DIMINUTO_LOG_MASK_NOTICE;
            priority = DIMINUTO_LOG_PRIORITY_NOTICE;
        	break;

        case 'i':
            mask = DIMINUTO_LOG_MASK_INFORMATION;
            priority = DIMINUTO_LOG_PRIORITY_INFORMATION;
        	break;

        case 'd':
            mask = DIMINUTO_LOG_MASK_DEBUG;
            priority = DIMINUTO_LOG_PRIORITY_DEBUG;
        	break;

        case '?':
            fprintf(stderr, "usage: %s [ -N NAME ] [ -O OPTION ] [ -F FACILITY ] [ -E | -a | -c | -e | -w | -n | -i | -d ] ... \n", program);
            return 1;
            break;

        }

    }

    diminuto_log_setmask();

    if ((diminuto_log_mask & mask) != 0) {

    	diminuto_log_open_syslog(name, option, facility);

    	if (optind < argc) {
    		for (; optind < argc; ++optind) {
    			if (argv[optind] == (const char *)0) {
    				/* Do nothing. */
    			} else if (*argv[optind] == '\0') {
    				/* Do nothing. */
    			} else {
    				diminuto_log_log(priority, "%s\n", argv[optind]);
    			}
    		}
    	}

    }

    return 0;
}
