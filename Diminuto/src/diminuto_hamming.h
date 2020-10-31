/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_TELEGRAPH_HAMMING_PRIVATE_
#define _H_COM_DIAG_TELEGRAPH_HAMMING_PRIVATE_

/**
 * @file
 * @copyright Copyright 2012-2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This describes the Hamming private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This describes the Hamming private API.
 */

#include "com/diag/diminuto/diminuto_types.h"

#define H84_D1_OFF	(3)
#define H84_D2_OFF	(2)
#define H84_D3_OFF	(1)
#define H84_D4_OFF	(0)

#define H84_D1_BIT	(1 << H84_D1_OFF)
#define H84_D2_BIT	(1 << H84_D2_OFF)
#define H84_D3_BIT	(1 << H84_D3_OFF)
#define H84_D4_BIT	(1 << H84_D4_OFF)

#define H84_P1_OFF	(7)
#define H84_P2_OFF	(6)
#define H84_S1_OFF	(5)
#define H84_P3_OFF	(4)
#define H84_S2_OFF	(3)
#define H84_S3_OFF	(2)
#define H84_S4_OFF	(1)
#define H84_P4_OFF	(0)

#define H84_P1_BIT	(1 << H84_P1_OFF)
#define H84_P2_BIT	(1 << H84_P2_OFF)
#define H84_S1_BIT	(1 << H84_S1_OFF)
#define H84_P3_BIT	(1 << H84_P3_OFF)
#define H84_S2_BIT	(1 << H84_S2_OFF)
#define H84_S3_BIT	(1 << H84_S3_OFF)
#define H84_S4_BIT	(1 << H84_S4_OFF)
#define H84_P4_BIT	(1 << H84_P4_OFF)

#define H84_O1_OFF	(0)
#define H84_O2_OFF	(1)
#define H84_O3_OFF	(2)

/**
 * Encode the four data bits (D1, D2, D3, D4) in a nibble into the four symbol
 * bits (S1, S2, S3, S4) in a symbol byte and return the symbol byte.
 * @param nib is the data nibble.
 * @return the symbol byte.
 */
static inline uint8_t diminuto_hamming84_dat2sym(uint8_t nib) {
    return	(
        (((nib & H84_D1_BIT) >> H84_D1_OFF) << H84_S1_OFF) |
        (((nib & H84_D2_BIT) >> H84_D2_OFF) << H84_S2_OFF) |
        (((nib & H84_D3_BIT) >> H84_D3_OFF) << H84_S3_OFF) |
        (((nib & H84_D4_BIT) >> H84_D4_OFF) << H84_S4_OFF)
    );
}

/**
 * Decode the four symbol bits (S1, S2, S3, S4) from a symbol byte into the four
 * data bits (D1, D2, D3, D4) in a data nibble and return the data nibble.
 * @param sym is the symbol byte.
 * @return the data nibble.
 */
static inline uint8_t diminuto_hamming84_sym2dat(uint8_t sym) {
    return	(
        (((sym & H84_S1_BIT) >> H84_S1_OFF) << H84_D1_OFF) |
        (((sym & H84_S2_BIT) >> H84_S2_OFF) << H84_D2_OFF) |
        (((sym & H84_S3_BIT) >> H84_S3_OFF) << H84_D3_OFF) |
        (((sym & H84_S4_BIT) >> H84_S4_OFF) << H84_D4_OFF)
    );
}

/**
 * Given a symbol byte containing four symbol bits (S1, S2, S3, S4), generate
 * the first even parity bit (P1) from the S1, S2, and S4 bits, and return a
 * mask with the P1 bit set.
 * @param sym is the symbol byte.
 * @return the P1 mask.
 */
static inline uint8_t diminuto_hamming84_sym2par1(uint8_t sym) {
    return (
        ((sym & H84_S1_BIT) >> H84_S1_OFF) ^
        ((sym & H84_S2_BIT) >> H84_S2_OFF) ^
        ((sym & H84_S4_BIT) >> H84_S4_OFF)
    ) << H84_P1_OFF;
}

/**
 * Given a symbol byte containing four symbol bits (S1, S2, S3, S4), generate
 * the second even parity bit (P2) from the S1, S3, and S4 bits, and return a
 * mask with the P2 bit set.
 * @param sym is the symbol byte.
 * @return the P2 mask.
 */
static inline uint8_t diminuto_hamming84_sym2par2(uint8_t sym) {
    return (
        ((sym & H84_S1_BIT) >> H84_S1_OFF) ^
        ((sym & H84_S3_BIT) >> H84_S3_OFF) ^
        ((sym & H84_S4_BIT) >> H84_S4_OFF)
    ) << H84_P2_OFF;
}

/**
 * Given a symbol byte containing four symbol bits (S1, S2, S3, S4), generate
 * the third even parity bit (P3) from the S2, S3, and S4 bits, and return a
 * mask with the P3 bit set.
 * @param sym is the symbol byte.
 * @return the P3 mask.
 */
static inline uint8_t diminuto_hamming84_sym2par3(uint8_t sym) {
    return (
        ((sym & H84_S2_BIT) >> H84_S2_OFF) ^
        ((sym & H84_S3_BIT) >> H84_S3_OFF) ^
        ((sym & H84_S4_BIT) >> H84_S4_OFF)
    ) << H84_P3_OFF;
}

/**
 * Given a symbol byte containing four symbol bits (S1, S2, S3, S4) and the
 * three parity bits (P1, P2, P3), generate the fourth even parity bit (P4)
 * from all seven S and P bits, and return a mask with the P4 bit set.
 * @param sym is the symbol byte.
 * @return the P4 mask.
 */
static inline uint8_t diminuto_hamming84_sym2par4(uint8_t sym) {
    return (
        ((sym & H84_P1_BIT) >> H84_P1_OFF) ^
        ((sym & H84_P2_BIT) >> H84_P2_OFF) ^
        ((sym & H84_S1_BIT) >> H84_S1_OFF) ^
        ((sym & H84_P3_BIT) >> H84_P3_OFF) ^
        ((sym & H84_S2_BIT) >> H84_S2_OFF) ^
        ((sym & H84_S3_BIT) >> H84_S3_OFF) ^
        ((sym & H84_S4_BIT) >> H84_S4_OFF)
    ) << H84_P4_OFF;
}

/**
 * Return true if the symbol byte is valid, false otherwise. There are only
 * sixteen valid symbols amongst a symbol space of 255 possible values. Some
 * of the invalid symbols will represent single bit errors which can be
 * corrected. Some of the invalid symbols will present double bit errors which
 * can be detected but not corrected. For errors of more than two bits,
 * wackiness will ensue since they may appear to be legitimate symbols or
 * symbols with only a single errored bit.
 * @param sym is the symbol byte.
 * @return !0 if valid, 0 otherwise.
 */
static inline int diminuto_hamming84_sym_is_valid(uint8_t sym) {
    switch (sym) {
    case 0x00:
    case 0xd2:
    case 0x55:
    case 0x87:
    case 0x99:
    case 0x4b:
    case 0xcc:
    case 0x1e:
    case 0xe1:
    case 0x33:
    case 0xb4:
    case 0x66:
    case 0x78:
    case 0xaa:
    case 0x2d:
    case 0xff:
        return !0;
    default:
        return 0;
    }
}

/**
 * Generate a bit shift offset for a symbol byte that will identify the errored
 * bit in the event that the symbol byte contains only one bit error.
 * @param sym is the symbol byte.
 * @return a bit shift offset identifying the single errored bit.
 */
static inline uint8_t diminuto_hamming84_sym2off(uint8_t sym) {
    return (8 - (
        (((diminuto_hamming84_sym2par1(sym) ^ (sym & H84_P1_BIT)) >> H84_P1_OFF) << H84_O1_OFF) |
        (((diminuto_hamming84_sym2par2(sym) ^ (sym & H84_P2_BIT)) >> H84_P2_OFF) << H84_O2_OFF) |
        (((diminuto_hamming84_sym2par3(sym) ^ (sym & H84_P3_BIT)) >> H84_P3_OFF) << H84_O3_OFF)
    ));
}

#endif
