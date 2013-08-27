/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FLETCHER8_
#define _H_COM_DIAG_DIMINUTO_FLETCHER8_

/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * J. Zweig, C. Partridge, "TCP Alternate Checksum Options", RFC 1146,
 * 1990-03
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Add one byte to an existing Fletcher 16-bit checksum as described in RFC1146.
 * The A and B bytes should be initialized to zero.
 * @param datum is the byte to add to the checksum.
 * @param ap points to the A byte which must initially be zero.
 * @param bp points to the B byte which must initially be zero.
 */
static inline void diminuto_fletcher16_datum(uint8_t datum, uint8_t * ap, uint8_t * bp)
{
 	*ap += datum;
	*bp += *ap;
}

/**
 * Compute a running Fletcher 16-bit checksum as described in RFC1146. Two
 * checksum bytes are computed, the A byte and the B byte, The convention
 * is for the caller to append the bytes (in that order) to the end of the
 * checksummed data block. A running checksum is computed: the caller should
 * initialize the A and B bytes to zero before calling the function at the
 * beginning of the block to be checksummed. Additional data blocks can be
 * added to the checksum.
 * @param buffer points to the starting octet at which to begin checksumming.
 * @param length is the number of octets over which to checksum.
 * @param ap points to the A byte which must initially be zero.
 * @param bp points to the B byte which must initially be zero.
 * @return a pointer past the last checksummed octet.
 */
extern void * diminuto_fletcher16_buffer(const void * buffer, size_t length, uint8_t * ap, uint8_t * bp);

#endif
