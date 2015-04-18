/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is the ipc feature private API.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include "com/diag/diminuto/diminuto_types.h"

extern int diminuto_ipc_identify(struct sockaddr * sap, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

#endif
