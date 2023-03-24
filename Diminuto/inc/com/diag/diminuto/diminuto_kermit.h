/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_KERMIT_
#define _H_COM_DIAG_DIMINUTO_KERMIT_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Computes the 16-bit Kermit Cyclic Redundancy Check
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * THIS IS A WORK IN PROGRESS.
 *
 * The Kermit feature computes the 16-bit CRC used in the original Kermit
 * file transfer utility and sometimes referred to as CRC-16 or CRC-CCITT.
 * Note that when used in HDLC-Lite, the CRC is computed across the
 * UNESCAPED data.
 *
 * REFERENCES
 *
 * Frank da Cruz, KERMIT PROTOCOL MANUAL, Columbia University Center for
 * Computing Activities, New York, 1986
 *
 * <https://chromium.googlesource.com/external/github.com/openthread/openthread/+/refs/tags/upstream/thread-reference-20180926/doc/spinel-protocol-src/spinel-framing.md>
 *
 * <https://www.etlcpp.com/doxygen/group__crc16__kermit.html>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Concerts a binary number in the range [0..94] to a printable character.
 * No range checking is performed.
 * @param bb is the binary number.
 * @return a printable character.
 */
static inline unsigned char diminuto_kermit_tochar(uint8_t bb) {
    return (bb + (unsigned)' ');
}

/**
 * Converts a printable number into the original binary number.
 * No printable checking is performed.
 * @param pp is the printable character.
 * @return the binary number.
 */
static inline uint8_t diminuto_kermit_unchar(unsigned char pp) {
    return (pp - (unsigned)' ');
}

/**
 * Compute a sixteen-bit Kermit CRC into three printable characters to be
 * appended to the outgoing data packet.
 * @param crc is the running sixteen-bit CRC.
 * @param ap points to the first character to be emitted.
 * @param bp points to the second character to be emitted.
 * @param cp points to the third character to be emitted.
 */
extern void diminuto_kermit_crc2chars(uint16_t crc, unsigned char * ap, unsigned char * bp, unsigned char * cp);

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
