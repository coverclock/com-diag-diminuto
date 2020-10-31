/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC4_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC4_PRIVATE_

/**
 * @file
 * @copyright Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This describes the IPC4 private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This describes the IPC4 private API.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/socket.h>

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
