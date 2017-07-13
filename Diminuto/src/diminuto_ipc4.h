/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC4_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC4_PRIVATE_

/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is the ipc4 feature private API.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include "com/diag/diminuto/diminuto_types.h"

/**
 * Extract the IPv4 address and port number in host byte order from a socket
 * address structure.
 * @param sap points to a socket address structure.
 * @param addressp points to an IPv4 address variable, or NULL.
 * @param portp points to a port variable, or NULL.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_ipc4_identify(struct sockaddr * sap, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

#endif
