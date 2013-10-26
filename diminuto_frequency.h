/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FREQUENCY_
#define _H_COM_DIAG_DIMINUTO_FREQUENCY_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * @def COM_DIAG_DIMINUTO_FREQUENCY
 * This symbol defines the period of the fundamental Diminuto time unit
 * "tick" expressed in Hertz or ticks per second. It is not part of the
 * public interface. A function that returns this value is available in
 * the Diminuto time API.
 */
#define COM_DIAG_DIMINUTO_FREQUENCY (1000000)

/**
 * @def COM_DIAG_DIMINUTO_FROM_TICKS
 * If @a _HERTZ_ is a constant, the compiler will almost certainly optimize
 * most of this code out.
 */
#define COM_DIAG_DIMINUTO_TICKS_FROM(_TICKS_, _HERTZ_) \
	((COM_DIAG_DIMINUTO_FREQUENCY < (_HERTZ_)) ? ((_TICKS_) * ((_HERTZ_) / COM_DIAG_DIMINUTO_FREQUENCY)) : \
		(COM_DIAG_DIMINUTO_FREQUENCY > (_HERTZ_)) ? ((_TICKS_) / (COM_DIAG_DIMINUTO_FREQUENCY / (_HERTZ_))) : \
			(_TICKS_))

/**
 * @def COM_DIAG_DIMINUTO_TO_TICKS
 * If @a _HERTZ_ is a constant, the compiler will almost certainly optimize
 * most of this code out.
 */
#define COM_DIAG_DIMINUTO_TICKS_TO(_VALUE_, _HERTZ_) \
	((COM_DIAG_DIMINUTO_FREQUENCY < (_HERTZ_)) ? (((diminuto_ticks_t)(_VALUE_)) / ((_HERTZ_) / COM_DIAG_DIMINUTO_FREQUENCY)) : \
		(COM_DIAG_DIMINUTO_FREQUENCY > (_HERTZ_)) ? (((diminuto_ticks_t)(_VALUE_)) * (COM_DIAG_DIMINUTO_FREQUENCY / (_HERTZ_))) : \
			(_VALUE_))

#endif
