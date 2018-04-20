/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ENDIANESS_
#define _H_COM_DIAG_DIMINUTO_ENDIANESS_

/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

/**
 * I use a union now instead of the older trick of casting an integer pointer
 * to a character pointer (borrowed from X11) because it works reliably without
 * regard to pointer aliasing assumptions made by compiler optimizers.
 */
typedef union DiminutoEndianess { int word; char bytes[sizeof(int)]; } diminuto_endianess_t;

/**
 * Return true if the process on which this runs is little endian. I expect
 * the compiler to trivially inline this.
 * @return !0 if little endian, 0 if big endian.
 */
static inline int diminuto_littleendian(void) {
    static const diminuto_endianess_t endianess = { !0 };
    return endianess.bytes[0];
}

#endif
