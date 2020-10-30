/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_HAMMING_
#define _H_COM_DIAG_DIMINUTO_HAMMING_

/**
 * @file
 * @copyright Copyright 2012-2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a Hamming(8,4) encode and decode.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Hamming feature implements a Hamming(8,4) encode and decode
 * function.
 *
 * REFERENCES
 *
 * R. W. Hamming, "Error Detecting and Error Correcting Codes", BELL SYSTEM
 * TECHNICAL JOURNAL, XXIX.2, April 1950.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Given a data nibble with four data bits, return a Hamming(8,4)-encoded
 * symbol byte with four symbol bits and four data bits.
 * @param nib is the data nibble.
 * @return the symbol byte.
 */
extern uint8_t diminuto_hamming84_nib2sym(uint8_t nib);

/**
 * Given a Hamming(8,4)-encoded symbol byte with four symbol bits and four
 * parity bits, return the data nibble with four data bits (if no bit errors
 * are detected), the corrected data nibble with four data bits (if a single
 * bit error was detected), or zero (if more than a single bit error was
 * detected). Also return the number of errors detected, zero, one, or two.
 * @param sym is the symbol byte.
 * @param errorsp points to a variable into which will be placed the number of
 * errors detected.
 * @return the data nibble or zero.
 */
extern uint8_t diminuto_hamming84_sym2nib(uint8_t sym, uint8_t * errorsp);

#endif
