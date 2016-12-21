/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FLETCHER_
#define _H_COM_DIAG_DIMINUTO_FLETCHER_

/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Implement the computationally simple Fletcher-16 checksum algorithm.
 * This checksum is used in some of the unit tests.
 *
 * REFERENCES
 *
 * J. Zweig, C. Partridge, "TCP Alternate Checksum Options", RFC 1145,
 * IETF, March 1990
 *
 * "Fletcher's checksum", Wikipedia,
 * https://en.wikipedia.org/wiki/Fletcher's_checksum, 2016-12-21
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Compute the running Fletcher-16 checksum. Takes as input two eight-bit
 * counters which should each be initialized to zero. Returns the current
 * sixteen-bit Fletcher-16 checksum.
 * @param buffer points to a data buffer to be checksummed.
 * @param length is the size of the data buffer in bytes.
 * @param ap points to the A running eight-bit counter.
 * @param bp points to the B running eight-bit counter.
 * @return the current value of the sixteen bit checksum.
 */
extern uint16_t diminuto_fletcher_16(const void * buffer, size_t length, uint8_t * ap, uint8_t * bp);

#endif
