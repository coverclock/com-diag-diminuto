/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FLETCHER_
#define _H_COM_DIAG_DIMINUTO_FLETCHER_

/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Fletcher feature implements the computationally simple Fletcher-16
 * checksum algorithm. This checksum is used in some of the unit tests.
 *
 * REFERENCES
 *
 * J. Zweig, C. Partridge, "TCP Alternate Checksum Options", RFC 1146,
 * https://tools.ietf.org/html/rfc1146, IETF, February 1990
 *
 * "Fletcher's checksum", Wikipedia,
 * https://en.wikipedia.org/wiki/Fletcher's_checksum, 2016-12-21
 *
 * J. Fletcher, "An Arithmetic Checksum for Serial Transmissions",
 * IEEE Transactions on Communication, COM-30, No. 1, pp. 247-252,
 * January 1982
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Compute the running sixteen bit Fletcher checksum. Requires two eight-bit
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
