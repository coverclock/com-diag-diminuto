/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * If you want to pipe stderr to a log command, here is one way using bash:
 *
 *	YOUR_COMMAND 2> >(./out/host/bin/log)
 *
 * Here are some test cases:
 *
 *	echo "message 1" 2> >(log) 1>&2
 *
 *	echo "message 2" 2> >(log -S) 1>&2
 *
 *	cat /dev/null 2> >(log -S "message 3") 1>&2
 *
 * An unfortunate side-effect of implementing a command line log utility for
 * use in (for example) a bash script is that the usual mechanisms for
 * determining if you are a daemon don't work. This utility allows you to
 * force the use of syslog(3) using the "-S" option. But I'm still pondering
 * a way to automate this.
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
    int unconditional = 0;
    char * buffer = (char *)0;
    size_t length = 256;
    char * endptr = (char *)0;
    int opt = '\0';
    extern char * optarg;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "B:N:O:F:SUEacewnid?")) >= 0) {

        switch (opt) {

        case 'E':
            mask = DIMINUTO_LOG_MASK_EMERGENCY;
            priority = DIMINUTO_LOG_PRIORITY_EMERGENCY;
            break;

        case 'F':
            facility = strtoul(optarg, &endptr, 0);
            break;

        case 'B':
            length = strtoul(optarg, &endptr, 0);
            break;

        case 'N':
            name = optarg;
            break;

        case 'O':
            option = strtoul(optarg, &endptr, 0);
            break;

        case 'S':
            diminuto_log_forced = true;
            break;

        case 'U':
            unconditional = !0;
            break;

        case 'a':
            mask = DIMINUTO_LOG_MASK_ALERT;
            priority = DIMINUTO_LOG_PRIORITY_ALERT;
            break;

        case 'c':
            mask = DIMINUTO_LOG_MASK_CRITICAL;
            priority = DIMINUTO_LOG_PRIORITY_CRITICAL;
            break;

        case 'd':
            mask = DIMINUTO_LOG_MASK_DEBUG;
            priority = DIMINUTO_LOG_PRIORITY_DEBUG;
            break;

        case 'e':
            mask = DIMINUTO_LOG_MASK_ERROR;
            priority = DIMINUTO_LOG_PRIORITY_ERROR;
            break;

        case 'i':
            mask = DIMINUTO_LOG_MASK_INFORMATION;
            priority = DIMINUTO_LOG_PRIORITY_INFORMATION;
            break;

        case 'n':
            mask = DIMINUTO_LOG_MASK_NOTICE;
            priority = DIMINUTO_LOG_PRIORITY_NOTICE;
            break;

        case 'w':
            mask = DIMINUTO_LOG_MASK_WARNING;
            priority = DIMINUTO_LOG_PRIORITY_WARNING;
            break;

        case '?':
        default:
            fprintf(stderr, "usage: %s [ -B BUFSIZE ] [ -N NAME ] [ -O OPTION ] [ -F FACILITY ] [ -S ] [ -E | -a | -c | -e | -w | -n | -i | -d ] [ -U ] MESSAGE ... \n", program);
            return 1;
            break;

        }

    }

    diminuto_log_setmask();

    if (unconditional || ((diminuto_log_mask & mask) != 0)) {

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

    buffer = (char *)malloc(length);
    if (buffer == (char *)0) {
        diminuto_perror("malloc");
        return 1;
    }

    /*
     * It's important to consume the standard input stream even if logging
     * is not enabled so as to not send the upstream pipeline a SIGPIPE.
     */

    while (fgets(buffer, length, stdin) != (char *)0) {
        if (unconditional || ((diminuto_log_mask & mask) != 0)) {
            diminuto_log_log(priority, "%s", buffer);
        }
    }

    free(buffer);

    return 0;
}
