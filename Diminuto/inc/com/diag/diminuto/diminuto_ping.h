/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PING_
#define _H_COM_DIAG_DIMINUTO_PING_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc.h"

extern int diminuto_ping_datagram_peer(void);

extern ssize_t diminuto_ping_datagram_send(int fd, diminuto_ipv4_t address);

extern ssize_t diminuto_ping_datagram_recv(int fd, diminuto_ipv4_t * addressp);

static inline int diminuto_ping_close(int fd) {
    return diminuto_ipc_close(fd);
}

#endif
