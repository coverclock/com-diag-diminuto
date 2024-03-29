/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_

/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the IPC private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This defines the IPC private API.
 */

/**
 * Enables IPC debugging via the private API.
 */
extern int diminuto_ipc_debug;

/**
 * Changes default IPC Endpoint type from IPv4 to IPv6 via the private API.
 */
extern int diminuto_ipc_endpoint_ipv6;

#endif
