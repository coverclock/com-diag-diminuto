/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is part of the Log unit test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is part of the Log unit test.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include <stdio.h>
#include "../src/diminuto_log.h"

static void maybe(void)
{
    fprintf(stderr, "TEST maybe: (%c){0x%x}\n", diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG(DIMINUTO_LOG_PRIORITY_DEBUG, "%s[%d](%c){0x%x} maybe?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
}

#define DIMINUTO_LOG_DISABLE
#include "com/diag/diminuto/diminuto_log.h"

static void no(void)
{
    fprintf(stderr, "TEST no: (%c){0x%x}\n", diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG(DIMINUTO_LOG_PRIORITY_ERROR, "%s[%d](%c){0x%x} no!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
}

#undef DIMINUTO_LOG_DISABLE
#include "com/diag/diminuto/diminuto_log.h"

static void yes(void)
{
    fprintf(stderr, "TEST yes: (%c){0x%x}\n", diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG(DIMINUTO_LOG_PRIORITY_INFORMATION, "%s[%d](%c){0x%x} yes.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
}

static void all(void)
{
    fprintf(stderr, "TEST all: (%c){0x%x}\n", diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_EMERGENCY("%s[%d](%c){0x%x} Emergency.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_ALERT("%s[%d](%c){0x%x} Alert.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_CRITICAL("%s[%d](%c){0x%x} Critical.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_ERROR("%s[%d](%c){0x%x} Error.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_WARNING("%s[%d](%c){0x%x} Warning.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_NOTICE("%s[%d](%c){0x%x} Notice.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_INFORMATION("%s[%d](%c){0x%x} Information.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_DEBUG("%s[%d](%c){0x%x} Debug.\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
}

#define DIMINUTO_LOG_EMERGENCY_DISABLE
#define DIMINUTO_LOG_ALERT_DISABLE
#define DIMINUTO_LOG_CRITICAL_DISABLE
#define DIMINUTO_LOG_ERROR_DISABLE
#define DIMINUTO_LOG_WARNING_DISABLE
#define DIMINUTO_LOG_NOTICE_DISABLE
#define DIMINUTO_LOG_INFORMATION_DISABLE
#define DIMINUTO_LOG_DEBUG_DISABLE
#include "com/diag/diminuto/diminuto_log.h"

static void none(void)
{
    fprintf(stderr, "TEST none: (%c){0x%x}\n", diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_EMERGENCY("%s[%d](%c){0x%x} Emergency!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_ALERT("%s[%d](%c){0x%x} Alert!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_CRITICAL("%s[%d](%c){0x%x} Critical!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_ERROR("%s[%d](%c){0x%x} Error!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_WARNING("%s[%d](%c){0x%x} Warning!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_NOTICE("%s[%d](%c){0x%x} Notice!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_INFORMATION("%s[%d](%c){0x%x} Information!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_DEBUG("%s[%d](%c){0x%x} Debug!\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
}

static diminuto_log_mask_t diminuto_log_subsystem[] = { DIMINUTO_LOG_MASK_NONE };
#if defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)
EXPORT_SYMBOL(diminuto_log_subsystem);
#endif

#undef DIMINUTO_LOG_EMERGENCY_DISABLE
#undef DIMINUTO_LOG_ALERT_DISABLE
#undef DIMINUTO_LOG_CRITICAL_DISABLE
#undef DIMINUTO_LOG_ERROR_DISABLE
#undef DIMINUTO_LOG_WARNING_DISABLE
#undef DIMINUTO_LOG_NOTICE_DISABLE
#undef DIMINUTO_LOG_INFORMATION_DISABLE
#undef DIMINUTO_LOG_DEBUG_DISABLE
#undef DIMINUTO_LOG_MASK
#define DIMINUTO_LOG_MASK diminuto_log_subsystem[0]
#include "com/diag/diminuto/diminuto_log.h"

static void mine(void)
{
    fprintf(stderr, "TEST mine: (%c){0x%x}\n", diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_EMERGENCY("%s[%d](%c){0x%x} Emergency?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_ALERT("%s[%d](%c){0x%x} Alert?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_CRITICAL("%s[%d](%c){0x%x} Critical?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_ERROR("%s[%d](%c){0x%x} Error?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_WARNING("%s[%d](%c){0x%x} Warning?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_NOTICE("%s[%d](%c){0x%x} Notice?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_INFORMATION("%s[%d](%c){0x%x} Information?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
    DIMINUTO_LOG_DEBUG("%s[%d](%c){0x%x} Debug?\n", __FILE__, __LINE__, diminuto_log_strategy, diminuto_log_mask);
}
