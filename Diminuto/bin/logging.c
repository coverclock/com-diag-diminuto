/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2019-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Display the configuration of the Log feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Display the configuration of the Log feature.
 */

#include <stdio.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_log.h"

#define P(_NAME_, _FORMAT_) printf("%s=" _FORMAT_ "\n", #_NAME_, _NAME_)
#define PV(_NAME_, _FORMAT_) printf("%s=" _FORMAT_ "\n", #_NAME_, (void *)_NAME_)

int main(void)
{
    P(DIMINUTO_LOG_MASK_EMERGENCY, "0x%02x");
    P(DIMINUTO_LOG_MASK_ALERT, "0x%02x");
    P(DIMINUTO_LOG_MASK_CRITICAL, "0x%02x");
    P(DIMINUTO_LOG_MASK_ERROR, "0x%02x");
    P(DIMINUTO_LOG_MASK_WARNING, "0x%02x");
    P(DIMINUTO_LOG_MASK_NOTICE, "0x%02x");
    P(DIMINUTO_LOG_MASK_INFORMATION, "0x%02x");
    P(DIMINUTO_LOG_MASK_DEBUG, "0x%02x");

    P(DIMINUTO_LOG_MASK_ALL, "0x%02x");
    P(DIMINUTO_LOG_MASK_NONE, "0x%02x");
    P(DIMINUTO_LOG_MASK_DEFAULT, "0x%02x");

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
    PV(DIMINUTO_LOG_STREAM_DEFAULT, "%p");
    P(DIMINUTO_LOG_MASK_NAME_DEFAULT, "\"%s\"");
    P(DIMINUTO_LOG_MASK_VALUE_ALL, "\"%s\"");
    P(DIMINUTO_LOG_BUFFER_MAXIMUM, "%u");

    P(diminuto_log_ident, "\"%s\"");
    P(diminuto_log_option, "0x%x");
    P(diminuto_log_facility, "0x%x");

    P(DIMINUTO_LOG_STRATEGY_AUTOMATIC, "%c");
    P(DIMINUTO_LOG_STRATEGY_STDERR, "%c");
    P(DIMINUTO_LOG_STRATEGY_SYSLOG, "%c");
    P(diminuto_log_strategy, "%c");
    P(diminuto_log_cached, "%d");

    P(diminuto_log_descriptor, "%d");
    P((diminuto_log_descriptor==STDOUT_FILENO), "%d");
    P((diminuto_log_descriptor==STDERR_FILENO), "%d");
    PV(diminuto_log_file, "%p");
    PV(diminuto_log_stream(), "%p");
    PV(diminuto_log_file, "%p");
    P((diminuto_log_file==stdout), "%d");
    P((diminuto_log_file==stderr), "%d");

    P(diminuto_log_mask, "0x%02x");
    P(diminuto_log_mask_name, "\"%s\"");
    P(getenv(diminuto_log_mask_name), "\"%s\"");
    P(diminuto_log_setmask(), "0x%02x");
    P(diminuto_log_mask, "0x%02x");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT), "%d");
    P(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY), "%d");

    diminuto_log_mask = 0x100;
    do {
        diminuto_log_mask >>= 1;
        DIMINUTO_LOG_DEBUG("DBUG=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_INFORMATION("INFO=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_NOTICE("NOTE=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_WARNING("WARN=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_ERROR("EROR=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_CRITICAL("CRIT=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_ALERT("ALRT=0x%02x\n", diminuto_log_mask);
        DIMINUTO_LOG_EMERGENCY("EMER=0x%02x\n", diminuto_log_mask);
    } while (diminuto_log_mask > 0x00);

    return 0;
}
