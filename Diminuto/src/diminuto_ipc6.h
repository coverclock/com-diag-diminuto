/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC6_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC6_PRIVATE_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is the ipc6 feature private API.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include "com/diag/diminuto/diminuto_types.h"

extern void diminuto_ipc6_ntoh6(diminuto_ipv6_t * addressp);

extern void diminuto_ipc6_hton6(diminuto_ipv6_t * addressp);

extern int diminuto_ipc6_identify(struct sockaddr * sap, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

#endif
