/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SWAP_
#define _H_COM_DIAG_DIMINUTO_SWAP_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Swaps contents of two integer variables with no third variable.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Swaps contents of two integer variables with no third variable. Really
 * of no practical use in any but the most contrained circumstances.
 * But interesting none the less.
 */

/**
 * @def DIMINUTO_SWAP
 * Swap the contents of variables @a _A_ and @a _B_ that are both of the same
 * integer type without using any temporary storage. This is a clever
 * hack applicable to only the most resource constrained environments. The
 * arguments are each evaluated three times, so side effects may cause
 * wackiness to ensue.
 */
#define DIMINUTO_SWAP(_A_, _B_) \
	do { \
		(_A_) ^= (_B_); \
		(_B_) ^= (_A_); \
		(_A_) ^= (_B_); \
	} while (0)

#endif /* _H_COM_DIAG_DIMINUTO_SWAP_ */
