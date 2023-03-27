/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FRAMER_
#define _H_COM_DIAG_DIMINUTO_FRAMER_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the public API for the Framer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * THIS IS A WORK IN PROGRESS
 * The Framer feature provies a mechanism for framing outgoing and incoming
 * packets on a serial port. The Framer uses an HDLC-like mechanism referred
 * to as control-octet transparenty (a.k.a. byte stuffing, or octet stuffing)
 * to do this. It also uses the same control octet characters as the High
 * Level Data Link Control protocol. (But it is in no way otherwise compatible
 * with HDLC.)
 *
 * REFERENCES
 *
 * <https://chromium.googlesource.com/external/github.com/openthread/openthread/+/refs/tags/upstream/thread-reference-20180926/doc/spinel-protocol-src/spinel-framing.md>
 *
 * <https://en.wikipedia.org/wiki/High-Level_Data_Link_Control>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>

typedef uint32_t diminuto_framer_length_t;

typedef enum DiminutoFramerState {
    DIMINUTO_FRAMER_STATE_INIT              = 'N',
    DIMINUTO_FRAMER_STATE_FLAG              = 'S',
    DIMINUTO_FRAMER_STATE_LENGTH1           = 'W',
    DIMINUTO_FRAMER_STATE_LENGTH1_ESCAPED   = 'w',
    DIMINUTO_FRAMER_STATE_LENGTH2           = 'X',
    DIMINUTO_FRAMER_STATE_LENGTH2_ESCAPED   = 'x',
    DIMINUTO_FRAMER_STATE_LENGTH3           = 'Y',
    DIMINUTO_FRAMER_STATE_LENGTH3_ESCAPED   = 'y',
    DIMINUTO_FRAMER_STATE_LENGTH4           = 'Z',
    DIMINUTO_FRAMER_STATE_LENGTH4_ESCAPED   = 'z',
    DIMINUTO_FRAMER_STATE_FLETCHERA         = 'A',
    DIMINUTO_FRAMER_STATE_FLETCHERA_ESCAPED = 'a',
    DIMINUTO_FRAMER_STATE_FLETCHERB         = 'B',
    DIMINUTO_FRAMER_STATE_FLETCHERB_ESCAPED = 'b',
    DIMINUTO_FRAMER_STATE_PAYLOAD           = 'P',
    DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED   = 'p',
    DIMINUTO_FRAMER_STATE_CRC1              = '1',
    DIMINUTO_FRAMER_STATE_CRC2              = '2',
    DIMINUTO_FRAMER_STATE_CRC3              = '3',
    DIMINUTO_FRAMER_STATE_COMPLETE          = 'C',
    DIMINUTO_FRAMER_STATE_FINAL             = 'F',
    DIMINUTO_FRAMER_STATE_ABORT             = 'E',
    DIMINUTO_FRAMER_STATE_IDLE              = 'I',
} diminuto_framer_state_t;

typedef enum DiminutoFramerToken {
    DIMINUTO_FRAMER_TOKEN_FLAG              = '~',      /* 0x7e */
    DIMINUTO_FRAMER_TOKEN_ESCAPE            = '}',      /* 0x7d */
    DIMINUTO_FRAMER_TOKEN_XON               = '\x11',   /* 0x11 */
    DIMINUTO_FRAMER_TOKEN_XOFF              = '\x13',   /* 0x13 */
    DIMINUTO_FRAMER_TOKEN_RESERVED          = '\xf8',   /* 0xf8 */
} diminuto_framer_token_t;

typedef enum DiminutoFramerConstant {
    DIMINUTO_FRAMER_CONSTANT_XOR            = ' ',      /* 0x20 */
} diminuto_framer_constant_t;

extern ssize_t diminuto_framer_writer(FILE * fp, const void * data, size_t length);

extern ssize_t diminuto_framer_cancel(FILE * fp);

extern diminuto_framer_state_t diminuto_framer_reader(FILE * fp, void * buffer, size_t size, diminuto_framer_state_t state, uint8_t ** herep, size_t * lengthp, uint16_t * crcp);

#endif
