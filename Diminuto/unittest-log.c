/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_unittest.h"
#include "diminuto_log.h"
#include <stdio.h>

#include "unittest-log.h"

int main(int argc, char ** argv)
{
    fprintf(stderr, "BEGIN\n");
    yes();
    no();
    maybe();
    fprintf(stderr, "PUBLIC DEFAULT\n");
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC ALL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC EMERGENCY\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC ALERT\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC CRITICAL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC ERROR\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC WARNING\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC NOTICE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC INFORMATION\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC DEBUG\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    fprintf(stderr, "PUBLIC NONE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE DEFAULT\n");
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE ALL\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE NONE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE EMERGENCY\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE ALERT\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE CRITICAL\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE ERROR\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE WARNING\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE NOTICE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE INFORMATION\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE DEBUG\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE NONE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    fprintf(stderr, "END\n");
    return 0;
}
