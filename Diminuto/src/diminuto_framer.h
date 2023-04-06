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
    FLAG        = '~',              /* 0x7e == 0b01111110 */
    ESCAPE      = '}',              /* 0x7d */
    XON         = '\x11',           /* 0x11 */
    XOFF        = '\x13',           /* 0x13 */
};

enum DiminutoFramerMask {
    MASK        = ' ',      /* 0x20 */
};

#endif
