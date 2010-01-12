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

int main(int argc, char ** argv)
{
    yes();
    no();
    maybe();
    diminuto_log(LOG_DEBUG, "done.\n");
    return 0;
}
