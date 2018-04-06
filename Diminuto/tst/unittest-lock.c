/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_lock.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char ** argv)
{
	{
		static const char * LOCKFILE = "/tmp/unittest-lock-1.pid";
		int rc;
		pid_t pid1;
		pid_t pid2;
		pid_t pid3;
		pid_t pid4;

		TEST();

		pid1 = getpid();
		ASSERT(pid1 >= 0);

		pid2 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid2 < 0);

		rc = diminuto_lock_lock(LOCKFILE);
		ASSERT(rc == 0);

		pid3 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid3 > 0);
		ASSERT(pid1 == pid3);

		rc = diminuto_lock_lock(LOCKFILE);
		ASSERT(rc < 0);

		rc = diminuto_lock_unlock(LOCKFILE);
		ASSERT(rc == 0);

		pid4 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid4 < 0);

		rc = diminuto_lock_unlock(LOCKFILE);
		ASSERT(rc < 0);

		STATUS();
	}

	{
		static const char * LOCKFILE = "/tmp/unittest-lock-2.pid";
		int rc;
		FILE * fp;
		pid_t pid1;
		pid_t pid2;
		pid_t pid3;
		pid_t pid4;

		TEST();

		pid1 = getpid();
		ASSERT(pid1 >= 0);

		pid2 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid2 < 0);

		rc = diminuto_lock_prelock(LOCKFILE);
		ASSERT(rc == 0);

		pid3 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid3 == 0);

		rc = diminuto_lock_postlock(LOCKFILE);
		ASSERT(rc == 0);

		pid3 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid3 > 0);
		ASSERT(pid1 == pid3);

		rc = diminuto_lock_lock(LOCKFILE);
		ASSERT(rc < 0);

		rc = diminuto_lock_unlock(LOCKFILE);
		ASSERT(rc == 0);

		pid4 = diminuto_lock_check(LOCKFILE);
		ASSERT(pid4 < 0);

		rc = diminuto_lock_unlock(LOCKFILE);
		ASSERT(rc < 0);

		STATUS();
	}


    EXIT();
}

