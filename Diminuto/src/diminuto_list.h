/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LIST_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_LIST_PRIVATE_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the IPC private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This defines the List private API, which is exposed only for unit
 * testing, or sometimes for exceptional application circumstances.
 */

/**
 * This is the limit on how many iterations the list audit may run before it
 * declares defeat. It is a global variable, but is not part of the public
 * API.
 */
extern int diminuto_list_audit_limit;

#endif
