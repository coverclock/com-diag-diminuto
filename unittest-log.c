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

int main(int argc, char ** argv)
{
    fprintf(stderr, "BEGIN\n");
    yes();
    no();
    maybe();
    fprintf(stderr, "DEFAULT\n");
    all();
    none();
    fprintf(stderr, "ALL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    fprintf(stderr, "NONE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    fprintf(stderr, "EMERGENCY\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    fprintf(stderr, "ALERT\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    fprintf(stderr, "CRITICAL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    fprintf(stderr, "ERROR\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    fprintf(stderr, "WARNING\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    fprintf(stderr, "NOTICE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    fprintf(stderr, "INFORMATION\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    fprintf(stderr, "DEBUG\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    fprintf(stderr, "END\n");
    return 0;
}
