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

static void maybe(void)
{
	DIMINUTO_LOG(LOG_DEBUG, "%s[%d]: maybe?\n", __FILE__, __LINE__);
}

#define DIMINUTO_LOG_DISABLE
#include "diminuto_log.h"

static void no(void)
{
	DIMINUTO_LOG(LOG_ERR, "%s[%d]: no!\n", __FILE__, __LINE__);
}

#undef DIMINUTO_LOG_DISABLE
#include "diminuto_log.h"

static void yes(void)
{
	DIMINUTO_LOG(LOG_DEBUG, "%s[%d]: yes.\n", __FILE__, __LINE__);
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
#include "diminuto_log.h"

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

static diminuto_log_mask_t mask = DIMINUTO_LOG_MASK_NONE;

#undef DIMINUTO_LOG_EMERGENCY_DISABLE
#undef DIMINUTO_LOG_ALERT_DISABLE
#undef DIMINUTO_LOG_CRITICAL_DISABLE
#undef DIMINUTO_LOG_ERROR_DISABLE
#undef DIMINUTO_LOG_WARNING_DISABLE
#undef DIMINUTO_LOG_NOTICE_DISABLE
#undef DIMINUTO_LOG_INFORMATION_DISABLE
#undef DIMINUTO_LOG_DEBUG_DISABLE
#undef DIMINUTO_LOG_MASK
#define DIMINUTO_LOG_MASK mask
#include "diminuto_log.h"

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
    mask = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE NONE\n");
    mask = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE EMERGENCY\n");
    mask = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE ALERT\n");
    mask = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE CRITICAL\n");
    mask = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE ERROR\n");
    mask = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE WARNING\n");
    mask = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE NOTICE\n");
    mask = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE INFORMATION\n");
    mask = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE DEBUG\n");
    mask = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    fprintf(stderr, "PRIVATE NONE\n");
    mask = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    fprintf(stderr, "END\n");
    return 0;
}
