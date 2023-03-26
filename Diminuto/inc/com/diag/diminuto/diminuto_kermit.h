/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_KERMIT_
#define _H_COM_DIAG_DIMINUTO_KERMIT_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Computes the 16-bit Kermit Cyclic Redundancy Check.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The Kermit feature computes the 16-bit CRC used in the original Kermit
 * file transfer utility and sometimes referred to as CRC-16 or CRC-CCITT.
 * or some combination of the two. Note that when used in the Framer featre,
 * the CRC is computed across the UNESCAPED data.
 *
 * REFERENCES
 *
 * Frank da Cruz, KERMIT PROTOCOL MANUAL, Columbia University Center for
 * Computing Activities, New York, 1986
 *
 * <https://chromium.googlesource.com/external/github.com/openthread/openthread/+/refs/tags/upstream/thread-reference-20180926/doc/spinel-protocol-src/spinel-framing.md>
 *
 * <https://www.etlcpp.com/doxygen/group__crc16__kermit.html>
 *
 * <https://crccalc.com>
 *
 * <http://sanity-free.org/147/standard_crc16_and_crc16_kermit_implementation_in_csharp.html>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Concerts a binary number in the range [0..94] to a printable character.
 * No range checking is performed.
 * @param bb is the binary number.
 * @return a printable character.
 */
static inline unsigned char diminuto_kermit_tochar(uint16_t bb) {
    return (bb + ' ');
}

/**
 * Converts a printable number into the original binary number.
 * No printable checking is performed.
 * @param pp is the printable character.
 * @return the binary number.
 */
static inline uint16_t diminuto_kermit_unchar(unsigned char pp) {
    return (pp - ' ');
}

/**
 * Returns true if the CRC character is in the valid range.
 * @param pp is the printable character.
 * @return true if the character is valid, false otherwise.
 */
static inline bool diminuto_kermit_charisvalid(unsigned char pp) {
    return ((' ' <= pp) && (pp <= '~'));
}

/**
 * Convert a sixteen-bit Kermit CRC into three printable characters to be
 * appended to the outgoing data packet.
 * @param crc is the running sixteen-bit CRC.
 * @param ap points to the first character to be emitted.
 * @param bp points to the second character to be emitted.
 * @param cp points to the third character to be emitted.
 */
extern void diminuto_kermit_crc2chars(uint16_t crc, unsigned char * ap, unsigned char * bp, unsigned char * cp);

/**
 * Convert the three printable characters encoding the CRC of a incoming data
 * packet into a sixteen-bit Kermit CRC. The three characters are assumed to be
 * valid. If they are not, wackiness ensues.
 * @param aa is the first (highest order) character.
 * @param bb is the second (middle order) character.
 * @param cc is the third (lowest order) character.
 * @return a sixteen-bit Kermit CRC. 
 */
extern uint16_t diminuto_kermit_chars2crc(unsigned char aa, unsigned char bb, unsigned char cc);

/**
 * Compute the running sixteen bit Kermit CRC. Requires a 16-bit integer
 * that is initialized to zero. Returns the current CRC.
 * @param buffer points to a data buffer to be checksummed.
 * @param length is the size of the data buffer in bytes.
 * @param crc is the running sixteen-bit CRC.
 * @return the new value of the sixteen-bit CRC.
 */
extern uint16_t diminuto_kermit_16(const void * buffer, size_t length, uint16_t crc);

#endif
