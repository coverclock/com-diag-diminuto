/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <stdio.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_log.h"

#define P(_NAME_, _FORMAT_) printf("%s=" _FORMAT_ "\n", #_NAME_, _NAME_)

int main(void)
{
    P(DIMINUTO_LOG_MASK_EMERGENCY, "0x%x");
    P(DIMINUTO_LOG_MASK_ALERT, "0x%x");
    P(DIMINUTO_LOG_MASK_CRITICAL, "0x%x");
    P(DIMINUTO_LOG_MASK_ERROR, "0x%x");
    P(DIMINUTO_LOG_MASK_WARNING, "0x%x");
    P(DIMINUTO_LOG_MASK_NOTICE, "0x%x");
    P(DIMINUTO_LOG_MASK_INFORMATION, "0x%x");
    P(DIMINUTO_LOG_MASK_DEBUG, "0x%x");

    P(DIMINUTO_LOG_MASK_ALL, "0x%x");
    P(DIMINUTO_LOG_MASK_NONE, "0x%x");
    P(DIMINUTO_LOG_MASK_DEFAULT, "0x%x");

    P(DIMINUTO_LOG_PRIORITY_EMERGENCY, "%d");
    P(DIMINUTO_LOG_PRIORITY_ALERT, "%d");
    P(DIMINUTO_LOG_PRIORITY_CRITICAL, "%d");
    P(DIMINUTO_LOG_PRIORITY_ERROR, "%d");
    P(DIMINUTO_LOG_PRIORITY_WARNING, "%d");
    P(DIMINUTO_LOG_PRIORITY_NOTICE, "%d");
    P(DIMINUTO_LOG_PRIORITY_INFORMATION, "%d");
    P(DIMINUTO_LOG_PRIORITY_DEBUG, "%d");

    P(DIMINUTO_LOG_IDENT_DEFAULT, "\"%s\"");
    P(DIMINUTO_LOG_OPTION_DEFAULT, "0x%x");
    P(DIMINUTO_LOG_FACILITY_DEFAULT, "0x%x");

    P(DIMINUTO_LOG_DESCRIPTOR_DEFAULT, "%d");
    P(DIMINUTO_LOG_STREAM_DEFAULT, "%p");
    P(DIMINUTO_LOG_MASK_NAME_DEFAULT, "\"%s\"");
    P(DIMINUTO_LOG_MASK_VALUE_ALL, "\"%s\"");
    P(DIMINUTO_LOG_BUFFER_MAXIMUM, "%u");

    P(diminuto_log_ident, "\"%s\"");
    P(diminuto_log_option, "0x%x");
    P(diminuto_log_facility, "0x%x");
    P(diminuto_log_descriptor, "%d");
    P(diminuto_log_file, "%p");
    P(diminuto_log_mask_name, "\"%s\"");
    P(diminuto_log_mask, "0x%x");
    P(diminuto_log_forced, "%d");
    P(diminuto_log_cached, "%d");

    P(getenv(diminuto_log_mask_name), "\"%s\"");

    diminuto_log_setmask();

    P(diminuto_log_mask, "0x%x");

    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG), "%d");

    return 0;
}
