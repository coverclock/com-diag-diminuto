/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PING_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_PING_PRIVATE_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is the ping feature private API.
 */

#include "com/diag/diminuto/diminuto_types.h"

/*
 * J. Postel, "Internet Control Message Protocol", RFC 792, September 1981
 *
 * R. Braden, D. Borman, C. Partridge, "Computing the Internet Checksum",
 * RFC 1071, September 1988
 */
extern uint16_t diminuto_inet_checksum(void * payload, size_t size);

#endif
