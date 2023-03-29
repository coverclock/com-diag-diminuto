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
 * to as control-octet transparency (a.k.a. byte stuffing, or octet stuffing)
 * to do this. It uses the same control octet characters as the High Level
 * Data Link Control protocol (but it is in no way otherwise compatible
 * with HDLC).
 *
 * EXAMPLE
 *
 * A Framer frame looks like this.
 *
 * FLAG LENGTH[4] FLETCHERA FLETCHERB PAYLOAD ... PAYLOAD CRC1 CRC2 CRC3
 *
 * FLAG: HDLC flag token.
 *
 * LENGTH: four-octet payload length field in network byte order plus necessary escape characters.
 *
 * FLETCHERA FLETCHERB: Fletcher-16 checksum A and B octets plus necessary escape characters.
 *
 * PAYLOAD[LENGTH]: payload plus necessary escape characters.
 *
 * CRC[3]: Kermit-16 cyclic redundancy check octets.
 *
 * REFERENCES
 *
 * <https://chromium.googlesource.com/external/github.com/openthread/openthread/+/refs/tags/upstream/thread-reference-20180926/doc/spinel-protocol-src/spinel-framing.md>
 *
 * <https://en.wikipedia.org/wiki/High-Level_Data_Link_Control>
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef uint32_t diminuto_framer_length_t;

typedef enum DiminutoFramerState {
    DIMINUTO_FRAMER_STATE_INITIALIZE        = '*',  /* Initialize. */
    DIMINUTO_FRAMER_STATE_FLAG              = '~',  /* Waiting for flag. */
    DIMINUTO_FRAMER_STATE_LENGTH            = 'L',  /* Length[4]. */
    DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED    = 'l',  /* Escaped Length[4]. */
    DIMINUTO_FRAMER_STATE_FLETCHER          = 'F',  /* Checksum[2]. */
    DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED  = 'f',  /* Escaped checksum[2]. */
    DIMINUTO_FRAMER_STATE_PAYLOAD           = 'P',  /* Payload. */
    DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED   = 'p',  /* Escaped payload. */
    DIMINUTO_FRAMER_STATE_KERMIT            = 'K',  /* CRC[3]. */
    DIMINUTO_FRAMER_STATE_COMPLETE          = '+',  /* Frame complete. */
    DIMINUTO_FRAMER_STATE_FINAL             = '.',  /* End of file. */
    DIMINUTO_FRAMER_STATE_ABORT             = '!',  /* Abort received. */
    DIMINUTO_FRAMER_STATE_FAILED            = '?',  /* CS or CRC failed. */
    DIMINUTO_FRAMER_STATE_OVERFLOW          = '>',  /* Buffer overflow. */
    DIMINUTO_FRAMER_STATE_IDLE              = '-',  /* Idle. */
} diminuto_framer_state_t;

typedef struct DiminutoFramer {
    void * buffer;                      /* Pointer to application buffer. */
    uint8_t * here;                     /* Pointer to next unused octet. */
    size_t size;                        /* Size of application buffer. */
    size_t limit;                       /* Remaining octets in current field. */
    diminuto_framer_length_t length;    /* Received packet length. */
    diminuto_framer_state_t state;      /* FSM state. */
    uint16_t crc;                       /* Kermit cyclic redundancy check. */
    uint8_t a;                          /* Fletcher checksum A. */
    uint8_t b;                          /* Fletcher checksum B. */
    char sum[2];                        /* Received checksum. */
    char check[3];                      /* Received cyclic redundancy check. */
} diminuto_framer_t;

/*******************************************************************************
 * CTOR, DTOR, AND RETOR
 ******************************************************************************/

static inline diminuto_framer_t * diminuto_framer_init(diminuto_framer_t * that, void * buffer, size_t size) {
    memset(that, 0, sizeof(*that));
    that->buffer = buffer;
    that->size = size;
    that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
    return that;
}

static inline diminuto_framer_t * diminuto_framer_fini(diminuto_framer_t * that) {
    return (diminuto_framer_t *)0;
}

static inline diminuto_framer_t * diminuto_framer_reinit(diminuto_framer_t * that) {
    that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
    return that;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline bool diminuto_framer_complete(const diminuto_framer_t * that) {
    return (that->state == DIMINUTO_FRAMER_STATE_COMPLETE);
}

static inline ssize_t diminuto_framer_length(const diminuto_framer_t * that) {
    return diminuto_framer_complete(that) ? (ssize_t)(that->length) : (ssize_t)(-1);
}

static inline void * diminuto_framer_buffer(const diminuto_framer_t * that) {
    return diminuto_framer_complete(that) ? that->buffer : (void *)0;
}

/*******************************************************************************
 * STATE MACHINE
 ******************************************************************************/

extern diminuto_framer_state_t diminuto_framer_machine(diminuto_framer_t * that, int token);

/*******************************************************************************
 * READERS AND WRITERS
 ******************************************************************************/

extern ssize_t diminuto_framer_emit(FILE * stream, const void * data, size_t length);

extern ssize_t diminuto_framer_writer(FILE * stream, const void * data, size_t length);

extern ssize_t diminuto_framer_abort(FILE * stream);

extern ssize_t diminuto_framer_reader(FILE * stream, diminuto_framer_t * that);

#endif
