/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Displays on standard output the calling processes' process id,
 * its session id, it's parent's process id, and it's parents's
 * session id.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
	pid_t pid;
	pid_t sid;
	pid_t ppid;
	pid_t psid;
	pid = getpid();
	sid = getsid(0);
	ppid = getppid();
	psid = getsid(ppid);
	printf("%d %d %d %d\n", pid, sid, ppid, psid);
	return ((pid >= 0) && (sid >= 0) && (ppid >= 0) && (psid >= 0)) ? 0 : 1;
}
