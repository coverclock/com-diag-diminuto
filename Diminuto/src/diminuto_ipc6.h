/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC6_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC6_PRIVATE_

/**
 * @file
 *
 * Copyright 2015-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is the ipc6 feature private API.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_endianess.h"

/**
 * Swap an IPv6 address variable from network to host byte order in place.
 * @param addressp points to an IPv6 address variable.
 */
extern void diminuto_ipc6_ntoh6_generic(diminuto_ipv6_t * addressp);

/**
 * Swap an IPv6 address variable from network to host byte order in place
 * if necessary.
 * @param addressp points to an IPv6 address variable.
 * @return the address of the variable.
 */
static inline diminuto_ipv6_t * diminuto_ipc6_ntoh6(diminuto_ipv6_t * addressp)
{
    if (diminuto_littleendian()) { diminuto_ipc6_ntoh6_generic(addressp); }
    return addressp;
}

/**
 * Swap an IPv6 address variable from host to network byte order in place.
 * @param addressp points to an IPv6 address variable.
 */
extern void diminuto_ipc6_hton6_generic(diminuto_ipv6_t * addressp);

/**
 * Swap an IPv6 address variable from host to network byte order in place.
 * @param addressp points to an IPv6 address variable.
 * @return the address of the variable.
 */
static inline diminuto_ipv6_t * diminuto_ipc6_hton6(diminuto_ipv6_t * addressp)
{
    if (diminuto_littleendian()) { diminuto_ipc6_hton6_generic(addressp); }
    return addressp;
}

/**
 * Extract the IPv6 address and port number in host byte order from a socket
 * address structure. (IPv4 addresses are encapsulated into IPv6 addresses.)
 * @param sap points to a socket address structure.
 * @param addressp points to an IPv6 address variable, or NULL.
 * @param portp points to a port variable, or NULL.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_ipc6_identify(struct sockaddr * sap, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

#endif
