/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ENDIANESS_
#define _H_COM_DIAG_DIMINUTO_ENDIANESS_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/*
 * I use a union now instead of the older trick of casting an integer pointer
 * to a character pointer because it works reliably without regard to pointer
 * aliasing assumptions made by compiler optimizers.
 */
static union { uint32_t word; uint8_t bytes[sizeof(uint32_t)]; } diminuto_endian = { 1 };

/**
 * Return true if the process on which this runs is little endian. I expect
 * the compiler to trivially inline this.
 * @return !0 if little endian, 0 if big endian.
 */
static inline int diminuto_littleendian(void) { return diminuto_endian.bytes[0]; }

#endif
