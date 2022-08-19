/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the File functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Displays whether stderr is a serial device or not.
 *
 * EXAMPLES
 *
 * isserial
 * isserial daemon
 * isserial hangup
 * isserial service
 * isserial 2> FILE
 * isserial daemon 2> FILE
 * isserial hangup 2> FILE
 * isserial service 2> FILE
 * isserial 2> /dev/null
 * isserial daemon 2> /dev/null
 * isserial hangup 2> /dev/null
 * isserial service 2> /dev/null
 */

#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_hangup.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[])
{
    const char * result = (const char *)0;
    int xc = 2;
    int priority = DIMINUTO_LOG_PRIORITY_NOTICE;

    if (diminuto_serial_valid(fileno(stderr))) {
        result = "true";
        xc = 0;
    } else {
        result = "false";
        xc = 1;
    }

    diminuto_log_write(fileno(stderr), priority, "isserial: BEFORE: diminuto_log_write: %s\n",  result);
    diminuto_log_syslog(priority, "isserial: BEFORE: diminuto_log_syslog: %s\n", result);
    diminuto_log_log(priority, "isserial: BEFORE: diminuto_log_log: %s\n", result);
    diminuto_log_emit("isserial: BEFORE: diminuto_log_emit: %s\n", result);
    fprintf(stderr, "isserial: BEFORE: fprintf: %s\n", result);
    printf("isserial: BEFORE: printf: %s\n", result);

    if (argc < 2) { /* Do nothing. */ }
    else if (*argv[1] == 'd') { diminuto_daemon("isserial"); }
    else if (*argv[1] == 'h') { diminuto_hangup_install(!0); }
    else if (*argv[1] == 's') { diminuto_service(); }
    else { return 2; }

    diminuto_log_write(fileno(stderr), priority, "isserial: AFTER: diminuto_log_write: %s\n",  result);
    diminuto_log_syslog(priority, "isserial: AFTER: diminuto_log_syslog: %s\n", result);
    diminuto_log_log(priority, "isserial: AFTER: diminuto_log_log: %s\n", result);
    diminuto_log_emit("isserial: AFTER: diminuto_log_emit: %s\n", result);
    fprintf(stderr, "isserial: AFTER: fprintf: %s\n", result);
    printf("isserial: AFTER: printf: %s\n", result);

    return xc;
}
