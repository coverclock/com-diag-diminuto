/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FRAMER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_FRAMER_PRIVATE_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the private API for the Framer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This defines the private API for the Framer feature.
 */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum DiminutoFramerToken {
    ESCAPE      = '}',      /* 0x7d */
    FLAG        = '~',      /* 0x7e == 0b01111110 */
    NEWLINE     = '\n',     /* 0x0a */
    XOFF        = '\x13',   /* 0x13 */
    XON         = '\x11',   /* 0x11 */
};

enum DiminutoFramerMask {
    MASK        = ' ',      /* 0x20 */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef enum DiminutoFramerAction {
    CONSUME     = 'C',      /* Consume this octet without processing. */
    FLETCHER    = 'F',      /* Process Fletcher-16 checksum. */
    KERMIT      = 'K',      /* Process Kermit-16 cyclic redundancy check. */
    LENGTH      = 'L',      /* Process length and Fletcher-16 checksum. */
    PAYLOAD     = 'P',      /* Compute Kermit-6 cyclic redundancy check. */
    RESET       = 'R',      /* Initialize Framer for next frame. */
    SEQUENCE    = 'N',      /* Process sequence and Fletcher-16 checksum. */
    STORE       = 'S',      /* Process this octet and Kermit-16 CRC. */
} action_t;

#endif
