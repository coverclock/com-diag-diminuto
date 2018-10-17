//* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_TELEGRAPH_SWAP_
#define _H_COM_DIAG_TELEGRAPH_SWAP_

/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Telegraph.html<BR>
 */

/**
 * @def TELEGRAPH_SWAP
 * Swap the contents of variables @a _A_ and @a _B_ that are both of the same
 * fundamental type without using any temporary storage. This is a clever
 * hack applicable to only the most resource constrained environments, like
 * certain eight-bit PIC micro-controllers.
 */
#define TELEGRAPH_SWAP(_A_, _B_) \
	do { \
		_A_ ^= _B_; \
		_B_ ^= _A_; \
		_A_ ^= _B_; \
	} while (0)

#endif /* _H_COM_DIAG_TELEGRAPH_SWAP_ */
