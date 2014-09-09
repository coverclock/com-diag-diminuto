/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * It is useful to run this under valgrind to insure that it doesn't have
 * any memory leaks.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_path.h"

int main(int argc, char ** argv)
{
	char * result;

	/*
	 * I tried to pick stuff that an embedded Linux system was likely to have
	 * so that this could be run on the target.
	 */

	result = diminuto_path_scan("/usr/sbin:/usr/bin:/sbin:/bin", "ls");
	ASSERT(result != (char *)0);
	EXPECT(strcmp(result, "/bin/ls") == 0);
	free(result);

	result = diminuto_path_find("PATH", "ls");
	ASSERT(result != (char *)0);
	EXPECT(strcmp(result, "/bin/ls") == 0);
	free(result);

	result = diminuto_path_find("PATH", "rm");
	ASSERT(result != (char *)0);
	EXPECT(strcmp(result, "/bin/rm") == 0);
	free(result);

	result = diminuto_path_find("PATH", "head");
	ASSERT(result != (char *)0);
	EXPECT(strcmp(result, "/usr/bin/head") == 0);
	free(result);

	/*
	 * This next one has the added benefit that it tests soft links. It has
	 * the added deficit that we don't really know what the result should
	 * be.
	 */

	result = diminuto_path_find("LD_LIBRARY_PATH", "libdiminuto.so");
	ASSERT(result != (char *)0);
	free(result);

	/*
	 * And of course some should fail.
	 */

	result = diminuto_path_scan("/usr/sbin:/usr/bin:/sbin:/bin", "COM_DIAG_DIMINUTO_NOTFOUND");
	ASSERT(result == (char *)0);

	result = diminuto_path_scan((const char *)0, "ls");
	ASSERT(result == (char *)0);

	result = diminuto_path_scan("/usr/sbin:/usr/bin:/sbin:/bin", (const char *)0);
	ASSERT(result == (char *)0);

	result = diminuto_path_scan("/COM_DIAG_DIMINUTO_NOTFOUND/sbin:/COM_DIAG_DIMINUTO_NOTFOUND/bin", "ls");
	ASSERT(result == (char *)0);

	result = diminuto_path_find("PATH", "COM_DIAG_DIMINUTO_NOTFOUND");
	ASSERT(result == (char *)0);

	result = diminuto_path_find("COM_DIAG_DIMINUTO_NOTFOUND", "ls");
	ASSERT(result == (char *)0);

	result = diminuto_path_find((const char *)0, "ls");
	ASSERT(result == (char *)0);

	result = diminuto_path_find("PATH", (const char *)0);
	ASSERT(result == (char *)0);

	result = diminuto_path_find((const char *)0, (const char *)0);
	ASSERT(result == (char *)0);

	EXIT();
}
