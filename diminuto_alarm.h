/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ALARM_
#define _H_COM_DIAG_DIMINUTO_ALARM_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>

/**
 * Send a process a SIGALRM signal.
 * @param pid is the process identifier.
 * @return pid or <0 if an error occurred.
 */
extern pid_t diminuto_alarm(pid_t pid);

/**
 * Return true if the caller received a SIGALRM, false otherwise.
 * @return true if the caller received a SIGALRM, false otherwise.
 */
extern int diminuto_alarmed(void);

/**
 * Install a SIGALRM signal handler in the caller.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_alarmable(void);

#endif
