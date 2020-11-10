/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPCL_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPCL_PRIVATE_

/**
 * @file
 * @copyright Copyright 202- Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This describes the IPCL private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This describes the IPCL private API.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/socket.h>

/**
 * Extract the Local path from a socket address structure.
 * @param sap points to a socket address structure.
 * @param pathp if non-NULL points to where the path will be stored.
 * @param psize is the size of the path variable in bytes.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_ipcl_identify(struct sockaddr * sap, char *  pathp, size_t psize);

#endif
