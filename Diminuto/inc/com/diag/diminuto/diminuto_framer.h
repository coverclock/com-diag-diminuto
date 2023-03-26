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
 * packets on a serial port. The Framer uses an HDLC-like mechanism to do
 * this.
 *
 * REFERENCES
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>

typedef enum DiminutoFramerState {
    DIMINUTO_FRAMER_STATE_START             = 'S',
    DIMINUTO_FRAMER_STATE_LENGTH1_ESCAPED   = 'h',
    DIMINUTO_FRAMER_STATE_LENGTH1           = 'H',
    DIMINUTO_FRAMER_STATE_LENGTH2_ESCAPED   = 'l',
    DIMINUTO_FRAMER_STATE_LENGTH2           = 'L',
    DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED   = 'p',
    DIMINUTO_FRAMER_STATE_PAYLOAD           = 'P',
    DIMINUTO_FRAMER_STATE_CRC1              = '1',
    DIMINUTO_FRAMER_STATE_CRC2              = '2',
    DIMINUTO_FRAMER_STATE_CRC3              = '3',
    DIMINUTO_FRAMER_STATE_COMPLETE          = 'C',
    DIMINUTO_FRAMER_STATE_FINAL             = 'F',
    DIMINUTO_FRAMER_STATE_ERROR             = 'E',
    DIMINUTO_FRAMER_STATE_IDLE              = 'I',
} diminuto_framer_state_t;

extern ssize_t diminuto_framer_writer(FILE * fp, const void * buffer, size_t length);

extern diminuto_framer_state_t diminuto_framer_reader(FILE * fp, void * buffer, size_t size, diminuto_framer_state_t state, uint8_t ** here, size_t * length);

#endif
