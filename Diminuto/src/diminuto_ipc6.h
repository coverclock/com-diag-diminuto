/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC6_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC6_PRIVATE_

/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
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
 * Remarkably, the standard sockaddr structure doesn't reserve enough space to
 * actually hold an entire IPv6 in6_addr field; it's only fourteen bytes, where
 * as an IPv6 address is 16 bytes (and even that's not enough, since there are
 * other fields in the sockaddr_in6 structure). So we use a larger one of our
 * own devising, carefully mimicing the legacy structure's format. Other than
 * the initial family field that indicates what kind of address family structure
 * this really is (IPv6, IPv4, UNIX, etc.), the rest of this structure is just
 * just buffer space. Although this isn't part of the public API, it is used
 * in the ipc6 unit test.
 */
typedef struct DiminutoIpc6SocketAddress {
    unsigned short int sa_family; /* POSIX requires an unsigned short int. */
    char sa_data[256 - sizeof(unsigned short int)]; /* glibc assumes short alignment. */
} diminuto_ipc6_sockaddr_t;

/**
 * Swap an IPv6 address variable from network to host byte order in place.
 * @param addressp points to an IPv6 address variable.
 */
extern void diminuto_ipc6_ntoh6_generic(diminuto_ipv6_t * addressp);

/**
 * Swap an IPv6 address variable from network to host byte order in place
 * if necessary.
 * @param addressp points to an IPv6 address variable.
 */
static inline void diminuto_ipc6_ntoh6(diminuto_ipv6_t * addressp)
{
    if (diminuto_littleendian()) {
        diminuto_ipc6_ntoh6_generic(addressp);
    }
}

/**
 * Swap an IPv6 address variable from host to network byte order in place.
 * @param addressp points to an IPv6 address variable.
 */
extern void diminuto_ipc6_hton6_generic(diminuto_ipv6_t * addressp);

/**
 * Swap an IPv6 address variable from host to network byte order in place.
 * @param addressp points to an IPv6 address variable.
 */
static inline void diminuto_ipc6_hton6(diminuto_ipv6_t * addressp)
{
    if (diminuto_littleendian()) {
        diminuto_ipc6_hton6_generic(addressp);
    }
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
