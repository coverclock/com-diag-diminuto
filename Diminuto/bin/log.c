/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Log a message using the standard Log mechanism.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Log a message using the standard Log mechanism.
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
#include "../src/diminuto_log.h"

int main(int argc, char * argv[])
{
    const char * program = "log";
    const char * name = DIMINUTO_LOG_IDENT_DEFAULT;
    int option = DIMINUTO_LOG_OPTION_DEFAULT;
    int facility = DIMINUTO_LOG_FACILITY_DEFAULT;
    diminuto_log_mask_t mask = DIMINUTO_LOG_MASK_DEFAULT;
    int priority = DIMINUTO_LOG_PRIORITY_DEFAULT;
    int unconditional = 0;
    int suppress = 0;
    int count = 0;
    char * buffer = (char *)0;
    size_t length = 256;
    char * endptr = (char *)0;
    int opt = '\0';
    extern char * optarg;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "AB:IN:O:F:SUEacdiensw?")) >= 0) {

        switch (opt) {

        case 'A':
            diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_AUTOMATIC;
            break;

        case 'B':
            length = strtoul(optarg, &endptr, 0);
            break;

        case 'E':
            mask = DIMINUTO_LOG_MASK_EMERGENCY;
            priority = DIMINUTO_LOG_PRIORITY_EMERGENCY;
            break;

        case 'F':
            facility = strtoul(optarg, &endptr, 0);
            break;

        case 'I':
            suppress = !0;
            break;

        case 'N':
            name = optarg;
            break;

        case 'O':
            option = strtoul(optarg, &endptr, 0);
            break;

        case 'S':
            diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_SYSLOG;
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

        case 's':
            diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_STDERR;
            break;

        case 'w':
            mask = DIMINUTO_LOG_MASK_WARNING;
            priority = DIMINUTO_LOG_PRIORITY_WARNING;
            break;

        case '?':
        default:
            fprintf(stderr, "usage: %s [ -B BUFSIZE ] [ -I ] [ -A | -S | -s ] [ -U ] [ -N NAME ] [ -O OPTION ] [ -F FACILITY ] [ -E | -a | -c | -e | -w | -n | -i | -d ] MESSAGE ... \n", program);
            fprintf(stderr, "       -B BUFSIZE      Allocate a buffer of BUFSIZE bytes for reading lines from standard input.\n");
            fprintf(stderr, "       -I              Suppress reading standard input.\n");
            fprintf(stderr, "       -A              Force logging to automatic.\n");
            fprintf(stderr, "       -S              Force logging to the system log even if the caller is not a daemon.\n");
            fprintf(stderr, "       -s              Force logging to standard error even if the caller is a daemon.\n");
            fprintf(stderr, "       -U              Log Unconditionally by ignoring the the log mask.\n");
            fprintf(stderr, "       -N NAME         Use the string NAME as the application name in the system log.\n");
            fprintf(stderr, "       -O OPTION       Pass the numeric value OPTION as the option to the system logger.\n");
            fprintf(stderr, "       -F FACILITY     Pass the numeric value FACILITY as the facility to the system logger.\n");
            fprintf(stderr, "       -E              Log at the EMERGENCY level.\n");
            fprintf(stderr, "       -a              Log at the ALERT level.\n");
            fprintf(stderr, "       -c              Log at the CRITICAL level.\n");
            fprintf(stderr, "       -e              Log at the ERROR level.\n");
            fprintf(stderr, "       -w              Log at the WARNING level.\n");
            fprintf(stderr, "       -n              Log at the NOTICE level.\n");
            fprintf(stderr, "       -i              Log at the INFORMATION level.\n");
            fprintf(stderr, "       -d              Log at the DEBUG level.\n");
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
                    ++count;
                }
            }
        }

    }

    if (suppress) {
        /* Do nothing. */
    } else if (count > 0) {
        /* Do nothing. */
    } else {

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

    }

    return 0;
}
