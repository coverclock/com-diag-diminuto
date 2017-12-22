/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_platform.h"

static void maybe(void)
{
	DIMINUTO_LOG(DIMINUTO_LOG_PRIORITY_DEBUG, "%s[%d]: maybe?\n", __FILE__, __LINE__);
}

#define DIMINUTO_LOG_DISABLE
#include "com/diag/diminuto/diminuto_log.h"

static void no(void)
{
	DIMINUTO_LOG(DIMINUTO_LOG_PRIORITY_ERROR, "%s[%d]: no!\n", __FILE__, __LINE__);
}

#undef DIMINUTO_LOG_DISABLE
#include "com/diag/diminuto/diminuto_log.h"

static void yes(void)
{
	DIMINUTO_LOG(DIMINUTO_LOG_PRIORITY_INFORMATION, "%s[%d]: yes.\n", __FILE__, __LINE__);
}

static void all(void)
{
   DIMINUTO_LOG_EMERGENCY("%s[%d]: Emergency.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_ALERT("%s[%d]: Alert.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_CRITICAL("%s[%d]: Critical.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_ERROR("%s[%d]: Error.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_WARNING("%s[%d]: Warning.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_NOTICE("%s[%d]: Notice.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_INFORMATION("%s[%d]: Information.\n", __FILE__, __LINE__);
   DIMINUTO_LOG_DEBUG("%s[%d]: Debug.\n", __FILE__, __LINE__);
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
   DIMINUTO_LOG_EMERGENCY("%s[%d]: Emergency!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_ALERT("%s[%d]: Alert!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_CRITICAL("%s[%d]: Critical!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_ERROR("%s[%d]: Error!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_WARNING("%s[%d]: Warning!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_NOTICE("%s[%d]: Notice!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_INFORMATION("%s[%d]: Information!\n", __FILE__, __LINE__);
   DIMINUTO_LOG_DEBUG("%s[%d]: Debug!\n", __FILE__, __LINE__);
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
   DIMINUTO_LOG_EMERGENCY("%s[%d]: Emergency?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_ALERT("%s[%d]: Alert?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_CRITICAL("%s[%d]: Critical?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_ERROR("%s[%d]: Error?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_WARNING("%s[%d]: Warning?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_NOTICE("%s[%d]: Notice?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_INFORMATION("%s[%d]: Information?\n", __FILE__, __LINE__);
   DIMINUTO_LOG_DEBUG("%s[%d]: Debug?\n", __FILE__, __LINE__);
}
