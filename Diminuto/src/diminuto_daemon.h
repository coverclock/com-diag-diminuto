/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DAEMON_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_DAEMON_PRIVATE_

/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This is the daemon feature private API.
 */

typedef enum DiminutoDaemonTest {
	DIMINUTO_DAEMON_TEST_NONE = 0,
	DIMINUTO_DAEMON_TEST_GETPPID,
	DIMINUTO_DAEMON_TEST_INIT,
	DIMINUTO_DAEMON_TEST_FORK,
	DIMINUTO_DAEMON_TEST_WAITPID,
	DIMINUTO_DAEMON_TEST_REFORK,
} diminuto_daemon_test_t;

extern diminuto_daemon_test_t diminuto_daemon_testing;

static diminuto_daemon_test_t diminuto_daemon_test(diminuto_daemon_test_t test)
{
	int tested;

	tested = diminuto_daemon_testing;
	diminuto_daemon_testing = test;

	return tested;
}

#endif
