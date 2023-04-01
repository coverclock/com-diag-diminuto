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
 * The Framer feature provides a mechanism for framing outgoing and incoming
 * frames on a serial port. The Framer uses an HDLC-like mechanism referred
 * to as control-octet transparency (a.k.a. byte stuffing, or octet stuffing)
 * to do this. It uses the same control octet characters as the High Level
 * Data Link Control protocol, but it is in no way otherwise compatible
 * with HDLC.
 *
 * EXAMPLE
 *
 * A Framer frame looks like this.
 *
 * FLAG LENGTH[4] FLETCHER[2] PAYLOAD ... PAYLOAD CRC[3]
 *
 * FLAG: HDLC flag token.
 *
 * LENGTH[4]: four-octet payload length (not including escape tokens)
 * field in network byte order plus necessary HDLC escape tokens.
 *
 * FLETCHER[2]: Fletcher-16 checksum A and B octets (across unescaped
 * LENGTH in network byte order) plus necessary HDLC escape tokens.
 *
 * PAYLOAD[]: payload plus necessary HDLC escape tokens.
 *
 * CRC[3]: Kermit-16 cyclic redundancy check octets (across unescaped PAYLOAD).
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

/**
 * This describes what the payload header looks like. The length of the
 * payload in octets is passed across the wire in network byte order, and
 * its checksum is computed on the network byte order field.
 */
typedef uint32_t diminuto_framer_length_t;

/**
 * This enumeration defines the states in which that the Framer input state
 * machine may be. INITIALIZE is the start state for every new received
 * frame. Terminal states are COMPLETE (a complete frame has been received
 * correctly), FINAL (end of file or othr error on the input stream has
 * occurred), ABORT (the far end transmitted an abort sequence signalling
 * the near end to abandon the current frame), FAILED (the checmsum or cyclic
 * redundance check did not match), OVERFLOW (the received frame is longer
 * than the buffer the application provided), and IDLE (a special state in
 * which all received tokens are discarded). Once the state machine reaches
 * a terminal state, the caller is responsible to resetting the machine into
 * the INITIALIZE state before more tokens can be processed; the Reader
 * function does this automatically for all terminal states other than the
 * COMPLETE state.
 */
typedef enum DiminutoFramerState {
    DIMINUTO_FRAMER_STATE_INITIALIZE        = '*',  /* Initialize. */
    DIMINUTO_FRAMER_STATE_FLAG              = '~',  /* Waiting for flag. */
    DIMINUTO_FRAMER_STATE_FLAGS             = '_',  /* May have more flags. */
    DIMINUTO_FRAMER_STATE_LENGTH            = 'L',  /* Length[4]. */
    DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED    = 'l',  /* Escaped Length[4]. */
    DIMINUTO_FRAMER_STATE_FLETCHER          = 'F',  /* Checksum[2]. */
    DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED  = 'f',  /* Escaped checksum[2]. */
    DIMINUTO_FRAMER_STATE_PAYLOAD           = 'P',  /* Payload. */
    DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED   = 'p',  /* Escaped payload[...]. */
    DIMINUTO_FRAMER_STATE_KERMIT            = 'K',  /* CRC[3]. */
    DIMINUTO_FRAMER_STATE_COMPLETE          = '+',  /* Frame complete. */
    DIMINUTO_FRAMER_STATE_FINAL             = '.',  /* End of file. */
    DIMINUTO_FRAMER_STATE_ABORT             = '!',  /* Abort received. */
    DIMINUTO_FRAMER_STATE_FAILED            = '?',  /* CS or CRC failed. */
    DIMINUTO_FRAMER_STATE_OVERFLOW          = '>',  /* Buffer overflow. */
    DIMINUTO_FRAMER_STATE_IDLE              = '-',  /* Idle. */
} diminuto_framer_state_t;

/**
 * This defines the structure of the Framer object.
 */
typedef struct DiminutoFramer {
    void * buffer;                      /* Pointer to application buffer. */
    uint8_t * here;                     /* Pointer to next unused octet. */
    size_t size;                        /* Size of application buffer. */
    size_t limit;                       /* Remaining octets in current field. */
    diminuto_framer_length_t length;    /* Received frame length. */
    diminuto_framer_state_t state;      /* FSM state. */
    uint16_t crc;                       /* Kermit cyclic redundancy check. */
    uint8_t a;                          /* Fletcher checksum A. */
    uint8_t b;                          /* Fletcher checksum B. */
    char sum[2];                        /* Received checksum. */
    char check[3];                      /* Received cyclic redundancy check. */
    bool debug;                         /* Debug on or off. */
} diminuto_framer_t;

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_FRAMER_INIT
 * Generate code to statically initialize a Framer.
 */
#define DIMINUTO_FRAMER_INIT { (void *)0, }

/*******************************************************************************
 * DEBUG
 ******************************************************************************/

/**
 * Set debug mode on or off and return the prior mode. When debug mode
 * is enabled, the Framer state machine logs all state transitions at
 * the DEBUG level.
 * @param that points to the Framer object.
 * @param debug is true to enable debug mode, false to disable it.
 * @return the prior debug mode.
 */
bool diminuto_framer_debug(diminuto_framer_t * that, bool debug) {
    bool prior = that->debug;
    that->debug = debug;
    return prior;
}

/*******************************************************************************
 * CTOR, DTOR, AND RETOR
 ******************************************************************************/

/**
 * Reinitialize a Framer object so that it is ready to process another frame
 * using the same buffer with which it was initialized. Depending on the
 * application, either reinit() or init() may be used to prepare to process
 * subsequent frames after the first one.
 * @param that points to the Framer object.
 * @return a pointer to the Framer object.
 */
static inline diminuto_framer_t * diminuto_framer_reinit(diminuto_framer_t * that) {
    that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
    return that;
}

/**
 * Initialize a Framer object to be ready to process a frame.
 * @param that points to the Framer object.
 * @param buffer points to the buffer into which the frame is stored.
 * @param size is the size of the buffer in octets.
 * @return a pointer to the Framer object.
 */
static inline diminuto_framer_t * diminuto_framer_init(diminuto_framer_t * that, void * buffer, size_t size) {
    memset(that, 0, sizeof(*that));
    that->buffer = buffer;
    that->size = size;
    return diminuto_framer_reinit(that);
}

/**
 * Deinitialize a Framer object, releasing any dynamically allocated resources.
 * (This current implementation of Framer has no dynamically allocated
 * resources.) As a side effect, this places the Framer object in the IDLE
 * state< which will cause the Framer to discard any input.
 * @param that points to the Framer object.
 * @return a NULL.
 */
static inline diminuto_framer_t * diminuto_framer_fini(diminuto_framer_t * that) {
    memset(that, 0, sizeof(*that));
    that->state = DIMINUTO_FRAMER_STATE_IDLE;
    return (diminuto_framer_t *)0;
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return true if the Framer is in the COMPLETE state, false otherwise.
 * @param that points to the Framer object.
 * @return true if COMPLETE, false otherwise.
 */
static inline bool diminuto_framer_complete(const diminuto_framer_t * that) {
    return (that->state == DIMINUTO_FRAMER_STATE_COMPLETE);
}

/**
 * Return true if the Framer is in a terminal state, false otherwise.
 * Terminal states are COMPLETE, FINAL, ABORT, FAILED, OVERFLOW, and
 * IDLE. A Framer in a terminal state must be re-initialized or
 * reinitialized before it can deliver more frames.
 * @param that points to the Framer object.
 * @return true if terminal, false otherwise.
 */
static inline bool diminuto_framer_terminal(const diminuto_framer_t * that) {
    switch (that->state) {
    case DIMINUTO_FRAMER_STATE_COMPLETE:
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
    case DIMINUTO_FRAMER_STATE_IDLE:
        return true;
    default:
        return false;
    }
}

/**
 * Return true if the Framer is in an error state, false otherwise.
 * Error states are FINAL, ABORT, FAILED, OVERFLOW. Note that IDLE
 * is not an error state. A Framer in a terminal state must be
 * re-initialized using init(), or reinitialized using reinit(), before
 * it can deliver more frames.
 * @param that points to the Framer object.
 * @return true if error, false otherwise.
 */
static inline bool diminuto_framer_error(const diminuto_framer_t * that) {
    switch (that->state) {
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
        return true;
    default:
        return false;
    }
}

/**
 * If the Framer is COMPLETE, return the length of the received frame, or
 * <0 if it is not.
 * @param that points to the Framer object.
 * @return the frame length if COMPLETE, <0 otherwise.
 */
static inline ssize_t diminuto_framer_length(const diminuto_framer_t * that) {
    return diminuto_framer_complete(that) ? (ssize_t)(that->length) : (ssize_t)(-1);
}

/**
 * If the Framer is COMPLETE, return a pointer to the buffer containing the
 * received frame, or NULL if it is not.
 * @param that points to the Framer object.
 * @return a pointer to the frame buffer if COMPLETE, NULL if not.
 */
static inline void * diminuto_framer_buffer(const diminuto_framer_t * that) {
    return diminuto_framer_complete(that) ? that->buffer : (void *)0;
}

/*******************************************************************************
 * STATE MACHINE
 ******************************************************************************/

/**
 * Implements the Framer state machine tha processes input from an input
 * stream (whatever that may be) to construct a correct completed frame.
 * The caller is responsible for restarted the state machine when it
 * reaches a terminal state. Note that COMPLETE is a terminal state. Note
 * the the reader() function automatically restarts the state machine for
 * all terminal states except for COMPLETE of a non-zero length frame.
 * @param that points to the Framer object.
 * @param token is the input token, which can accomodate the stdio EOF value.
 * @return the state value of the Framer object.
 */
extern diminuto_framer_state_t diminuto_framer_machine(diminuto_framer_t * that, int token);

/*******************************************************************************
 * LOW LEVEL API
 ******************************************************************************/

/**
 * Read an input stream and drive a Framer state machine by feeding it
 * successive input tokens. The Framer state machine is automatically
 * restarted when it reaches the ABORT, FAILED, or OVERFLOW states; this
 * causes corrupted, aborted, or too-large frames to be automatically
 * dropped while log messages are generated. The Framer state machine is
 * also automatically restarted when it reaches the COMPLETE state and a
 * frame with a zero-length payload has been received, causing such frames
 * to be silently dropped. The reader() tries not to block, so that it can
 * be driven by a Mux or other multiplexing mechanism. In a single call it
 * processes data cached inside the input stream object, or if the
 * input stream is a TTY, in the TTY driver, until it runs out of data or
 * reaches a terminal state (including COMPLETE). The application can
 * keep calling the reader() until it returns >0 (a complete correct frame
 * received containing the returned number of octets), or <0 (an error
 * occurred). After reaching COMPLETE, the Framer must be re-initialized
 * with init() or reinitialized with reinit() to received subsequent frames.
 * @param stream points to the input stream.
 * @param that points to the Framer object.
 * @return 0 if a frame is being processed, >0 if complete, <0 if error.
 */
extern ssize_t diminuto_framer_reader(FILE * stream, diminuto_framer_t * that);

/**
 * Emit data to an output stream, inserting ESCAPE tokens as necessary to
 * implement the Framer byte-stuffing algorithm.
 * @param stream points to the output stream.
 * @param data points to the data to be emitted.
 * @param length is the length of the data in octets.
 * @return the total number of octets emitted or EOF if an error occurred.
 */
extern ssize_t diminuto_framer_emit(FILE * stream, const void * data, size_t length);

/*******************************************************************************
 * HIGH LEVEL API
 ******************************************************************************/

/**
 * Read a complete correct frame from the input stream, stripping all of
 * the control tokens, checksum, and CRC, and store the payload octets
 * into the buffer provided by the caller. The caller is blocked until the
 * a complete correct frame no larger than the buffer is processed. Frames
 * that are invalid, aborted, empty, or too large, are discarded with a log
 * message generated.
 * @param stream points to the input stream.
 * @param buffer is the buffer into which the payload is stored.
 * @param size is the size of the buffer in octets.
 * @return the total number of octets received or EOF if an error occurred.
 */
extern ssize_t diminuto_framer_read(FILE * stream, void * buffer, size_t size);

/**
 * Write  a complete frame to the output stream, including a FLAG token, the
 * length, Fletcher checksum, payload, and Kermit cyclic redundancy check.
 * After the complete frame is emitted, the output stream is flushed.
 * The caller is blocked until the complete frame is emitted.
 * @param stream points to the output stream.
 * @param data points to the payload to be emitted.
 * @param length is the length of the payload in octets.
 * @return the total number of octets emitted or EOF if an error occurred.
 */
extern ssize_t diminuto_framer_write(FILE * stream, const void * data, size_t length);

/**
 * Emit the ABORT sequence to the output stream. After the ABORT sequence
 * is emitted, the output stream is flushed.
 * @param stream points to the output stream.
 * @return the total number of octets emitted or EOF if an error occurred.
 */
extern ssize_t diminuto_framer_abort(FILE * stream);

/*******************************************************************************
 * DUMPER
 ******************************************************************************/

/**
 * Logs the contents of a Framer object.
 * @param that points to the Framer object.
 */
extern void diminuto_framer_dump(const diminuto_framer_t * that);

#endif
