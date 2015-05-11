//* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_INET_
#define _H_COM_DIAG_DIMINUTO_INET_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * REFERENCES
 *
 * J. Postel, "Internet Control Message Protocol", RFC 792, September 1981
 *
 * R. Braden, D. Borman, C. Partridge, "Computing the Internet Checksum",
 * RFC 1071, September 1988
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Compute a sixteen-bit internet checksum across a buffer. Remarkably,
 * according to RFC 1071, the endianess of the processor doesn't matter.
 * @param buffer points to the buffer.
 * @param size is the number of bytes in the buffer.
 * @return the sixteen-bit RFC 1071 internet checksum.
 */
extern uint16_t diminuto_inet_checksum(void * buffer, size_t size);

#endif
