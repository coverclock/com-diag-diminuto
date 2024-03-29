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
 * The Framer feature implements a mechanism for framing outgoing and incoming
 * binary packets in a serial stream. The Framer uses an HDLC-like mechanism, 
 * control-octet transparency (a.k.a. byte stuffing, or octet stuffing),
 * to do this. It uses the same control octet tokens or characters as the
 * High Level Data Link Control (HDLC) protocol, but it is in no way otherwise
 * compatible with HDLC.
 *
 * The Framer uses the following special tokens (which in no way prohibits
 * their use by the application). N.B. the double backslashes are to make
 * the Doxygen tool happy.
 *
 * FLAG a.k.a. '~', 0x7e, tilde.
 *
 * ESCAPE a.k.a. '}', 0x7d, right curley bracket (NOT ASCII ESCAPE);
 *
 * XON a.k.a. '\\x11', 0x11, DC1, Device Control 1, Transmit On;
 *
 * XOFF a.k.a. '\\x13', 0x13, DC3,. Device Control 3, Transmit Off.
 *
 * NEWLINE a.k.a. '\n', 0x0a, NL, line feed.
 *
 * A Framer frame looks like this:
 *
 * FLAG[1] LENGTH[2+] SEQUENCE[2+] FLETCHER[2+] PAYLOAD[LENGTH+] CRC[3] NEWLINE[1]
 *
 * where
 *
 * FLAG[1]: is a FLAG token (there can be multiple FLAG tokens in a row);
 *
 * SEQUENCE[2+]: is a simple two-octet monotonically increasing sequence
 * number field (which has no effect on whether or not frames are accepted)
 * in network byte order, plus any necessary ESCAPE tokens;
 *
 * LENGTH[2+]: is the two-octet payload length field in network byte order,
 * containing the length of the unESCAPEd payload, plus any necessary ESCAPE
 * tokens (yielding a maximum payload size of 65535 octets);
 *
 * FLETCHER[2+]: is the Fletcher-16 checksum A and B octets, computed across
 * the unESCAPEd SEQUENCE and LENGTH fields in network byte order, plus any
 * necessary ESCAPE tokens;
 *
 * PAYLOAD[LENGTH+]: is the payload, of length of the unESCAPEd LENGTH field
 * in host byte order, plus any necessary ESCAPE tokens (maximum unESCAPEd
 * length of 65535 octets);
 *
 * CRC[3]: is the Kermit-16 cyclic redundancy check octets, computed across
 * the unESCAPEd PAYLOAD (the Kermit-16 CRC octets will never fall within a
 * range requiring ESCAPE tokens - see the Kermit feature unit test);
 *
 * NEWLINE[1]: is a NEWLINE token (since all other NEWLINE characters are
 * ESCAPEd, this makes it easy to use tools like the standard I/O library
 * to read frames without even using a Framer).
 *
 * The specialness of XON and XOFF allows these characters to be used by the
 * underlying serial driver hardware or software in the usual manner for
 * flow control, without interfering with their use by the application as
 * data values. Normally, the serial driver will intercept XON and XOFF and
 * handle them appropriately. But in the event that software flow control
 * is enabled on one end but not the other, their appearance in the data
 * stream will not impact the application. If the Framer receives an XOFF,
 * it will turn on the throttle flag in the object, and turn it off when
 * XON is received; this allows the application to, for example, temporarily
 * cease transmitting frames.
 *
 * The specialness of NEWLINE makes it simple for applications to collect
 * a frame in a buffer from any data source without even using a Framer,
 * using, for example, fgets(3). There are examples of this in the unit test
 * suite.
 *
 * The Framer can be approached at via the low-level API by just driving its
 * state machine by simulating it via whatever data source the application uses.
 *
 * The mid-level API uses standard input/output streams but leaves the
 * management of the Framer object to the application.
 *
 * The high-level API provides more automation by managing a Framer object
 * internally, as a local variable reinitialized upon every call, at the
 * expense of abstracting out all of the details of what is going on with
 * the input stream.
 *
 * Similarly, framing of output can be handled by the application, using the
 * Fletcher checksum and Kermit cyclic redundancy check features itself, along
 * with the emit function that simplifies inserting the necessary ESCAPE tokens.
 * Or, it can be done with increasing levels of automation.
 *
 * The uses of standard I/O streams by the mid- and high-level APIs allows
 * the appication to use a variety of stream-type data sources, like pipes,
 * named pipes (FIFOs), serial ports, TCP stream sockets, and the like.
 *
 * Examples of all of these approaches can be found in the Framer unit test
 * suite.
 *
`* This feature was developed in direct support of the Fothergill sub-project.
 * Fothergill uses the LoRa (Long-Range) radio technology. Specifically, the
 * SparkFun LoRaSerial Kit provides two paired LoRa radios with interfaces
 * that enumerate over USB as serial ports. Framers have been successfully
 * used to pass data over this technology. Note that the SparkFun docs
 * mention that the product may drop packets, and the Framer sequence numbers
 * verify this. Using Framer with the LoRaSerial devices was tested using a
 * Total Phase Beagle 12 USB analyzer to examine frames on the wire at
 * each end of the connection.
 *
 * REFERENCES
 *
 * "HIGH LEVEL DATA LINK CONTROL PROCEDURES - PART 1: FRAME STRUCTURE",
 * ISO 3309-1984, ISO, 1984
 *
 * W. Simpson, ed., "PPP in HDLC-Like Framing', RFC 1662, 1994-07
 *
 * <https://chromium.googlesource.com/external/github.com/openthread/openthread/+/refs/tags/upstream/thread-reference-20180926/doc/spinel-protocol-src/spinel-framing.md>
 *
 * <https://en.wikipedia.org/wiki/High-Level_Data_Link_Control>
 * 
 * <https://github.com/coverclock/com-diag-hazer>
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
 * This enumeration defines the states in which that the Framer input state
 * machine may be. RESET is the start state for every new received
 * frame. Terminal states are COMPLETE (a complete frame has been received
 * correctly), FINAL (end of file or othr error on the input stream has
 * occurred), ABORT (the far end transmitted an abort sequence signalling
 * the near end to abandon the current frame), FAILED (the checmsum or cyclic
 * redundance check did not match), OVERFLOW (the received frame is longer
 * than the buffer the application provided), INVALID (the frame had an
 * invalid character sequence),  and IDLE (a special state in which all
 * all received tokens are discarded). Once the state machine reaches
 * a terminal state, the caller is responsible to resetting the machine into
 * the RESET state before more tokens can be processed; the Reader
 * function does this automatically for all terminal states other than the
 * COMPLETE state.
 */
typedef enum DiminutoFramerState {
    DIMINUTO_FRAMER_STATE_ABORT             = 'A',  /* Abort received. */
    DIMINUTO_FRAMER_STATE_COMPLETE          = 'C',  /* Frame complete. */
    DIMINUTO_FRAMER_STATE_FAILED            = 'X',  /* CS or CRC failed. */
    DIMINUTO_FRAMER_STATE_FINAL             = 'E',  /* End of file. */
    DIMINUTO_FRAMER_STATE_FLAG              = 'S',  /* One or more FLAGs. */
    DIMINUTO_FRAMER_STATE_FLETCHER          = 'F',  /* Checksum[2]. */
    DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED  = 'f',  /* Escaped checksum[2]. */
    DIMINUTO_FRAMER_STATE_IDLE              = 'I',  /* Idle. */
    DIMINUTO_FRAMER_STATE_INVALID           = 'V',  /* Frame invalid. */
    DIMINUTO_FRAMER_STATE_KERMIT            = 'K',  /* CRC[3]. */
    DIMINUTO_FRAMER_STATE_LENGTH            = 'L',  /* Length[2]. */
    DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED    = 'l',  /* Escaped Length[2]. */
    DIMINUTO_FRAMER_STATE_NEWLINE           = 'W',  /* NEWLINE[1]. */
    DIMINUTO_FRAMER_STATE_OVERFLOW          = 'O',  /* Buffer overflow. */
    DIMINUTO_FRAMER_STATE_PAYLOAD           = 'P',  /* Payload. */
    DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED   = 'p',  /* Escaped payload[...]. */
    DIMINUTO_FRAMER_STATE_RESET             = 'R',  /* Reset. */
    DIMINUTO_FRAMER_STATE_SEQUENCE          = 'N',  /* Sequence[2]. */
    DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED  = 'n',  /* Escaped Sequence[2]. */
} diminuto_framer_state_t;

/**
 * This defines the structure of the Framer object.
 */
typedef struct DiminutoFramer {
    void * buffer;                      /* Pointer to application buffer. */
    uint8_t * here;                     /* Pointer to next unused octet. */
    size_t size;                        /* Size of application buffer. */
    size_t limit;                       /* Remaining octets in current field. */
    size_t total;                       /* Total number of octets in frame. */
    diminuto_framer_state_t state;      /* FSM state. */
    uint16_t length0;                   /* Working payload length. */
    uint16_t length;                    /* Received payload length. */
    uint16_t sequence0;                 /* Working incoming sequence number. */
    uint16_t sequence;                  /* Complete incoming sequence number. */
    uint16_t previous;                  /* Previous incoming sequence number. */
    uint16_t generated;                 /* Previous outgoing sequence number. */
    uint16_t crc;                       /* Computed cyclic redundancy check. */
    uint16_t crc0;                      /* Received cyclic redundancy check. */
    uint8_t a;                          /* Computed checksum A. */
    uint8_t b;                          /* Computed checksum B. */
    uint8_t sum[2];                     /*`Received checksum A, B. */
    uint8_t check[3];                   /* Received CRC A, B, C. */
    bool throttle;                      /* XOFF received, wait for XON. */
    bool debug;                         /* Debug on or off. */
} diminuto_framer_t;

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_FRAMER_WINDOW
 * Generates the constant for the window size for Framer sequence numbers
 * (this is one more than the maximum sequence number, since sequence numbers
 * are zero-based).
 */
#define DIMINUTO_FRAMER_WINDOW (65536)

/**
 * @def DIMINUTO_FRAMER_INITIALIZER
 * Generate code to statically initialize a Framer in the IDLE state.
 */
#define DIMINUTO_FRAMER_INITIALIZER { \
    (void *)0, \
    (uint8_t *)0, \
    0, 0, 0, \
    DIMINUTO_FRAMER_STATE_IDLE, \
    0, 0, \
    0, DIMINUTO_FRAMER_WINDOW - 1, DIMINUTO_FRAMER_WINDOW - 2, \
    DIMINUTO_FRAMER_WINDOW - 1, \
    0, 0, \
    0, 0, \
    { '\0', '\0', }, \
    { ' ', ' ', ' ', }, \
    false, \
    false, \
}

/*******************************************************************************
 * CTOR and DTOR
 ******************************************************************************/

/**
 * Initialize a Framer object to be ready to process a frame.
 * @param that points to the Framer object.
 * @param buffer points to the buffer into which the frame is stored.
 * @param size is the size of the buffer in octets.
 * @return a pointer to the Framer object.
 */
extern diminuto_framer_t * diminuto_framer_init(diminuto_framer_t * that, void * buffer, size_t size);

/**
 * Deinitialize a Framer object, releasing any dynamically allocated resources.
 * (This current implementation of Framer has no dynamically allocated
 * resources.) As a side effect, this places the Framer object in the IDLE
 * state, which will cause the Framer to discard any input.
 * @param that points to the Framer object.
 * @return a NULL if successful, or the object pointer if error.
 */
static inline diminuto_framer_t * diminuto_framer_fini(diminuto_framer_t * that) {
    that->state = DIMINUTO_FRAMER_STATE_IDLE;
    return (diminuto_framer_t *)0;
}

/*******************************************************************************
 * SETTORS
 ******************************************************************************/

/**
 * Reinitialize a Framer object so that it is ready to process another frame
 * with the buffer it already has. This settor only alters the Framer state.
 * @param that points to the Framer object.
 * @return a pointer to the Framer object.
 */
static inline diminuto_framer_t * diminuto_framer_reset(diminuto_framer_t * that) {
    that->state = DIMINUTO_FRAMER_STATE_RESET;
    return that;
}

/**
 * Reinitialize a Framer object so that it is ready to process another frame
 * with the specified buffer. This settor only alters the buffer pointer and
 * its size and calls reset().
 * @param that points to the Framer object.
 * @param buffer points to the buffer into which the frame is stored.
 * @param size is the size of the buffer in octets.
 * @return a pointer to the Framer object.
 */
static inline diminuto_framer_t * diminuto_framer_set(diminuto_framer_t * that, void * buffer, size_t size) {
    that->buffer = buffer;
    that->size = size;
    return diminuto_framer_reset(that);
}

/*******************************************************************************
 * GETTORS
 ******************************************************************************/

/**
 * Return true if the Framer is in the COMPLETE state, false otherwise.
 * @param that points to the Framer object.
 * @return true if COMPLETE, false otherwise.
 */
static inline bool diminuto_framer_iscomplete(const diminuto_framer_t * that) {
    return (that->state == DIMINUTO_FRAMER_STATE_COMPLETE);
}

/**
 * Return true if the Framer is in a terminal state, false otherwise.
 * Terminal states are COMPLETE, FINAL, ABORT, FAILED, OVERFLOW, INVALID,
 * and IDLE. A Framer in a terminal state must be re-initialized or
 * reset before it can deliver more frames.
 * @param that points to the Framer object.
 * @return true if terminal, false otherwise.
 */
static inline bool diminuto_framer_isterminal(const diminuto_framer_t * that) {
    switch (that->state) {
    case DIMINUTO_FRAMER_STATE_COMPLETE:
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
    case DIMINUTO_FRAMER_STATE_INVALID:
    case DIMINUTO_FRAMER_STATE_IDLE:
        return true;
    default:
        return false;
    }
}

/**
 * Return true if the Framer is in an error state, false otherwise.
 * Error states are FINAL, ABORT, FAILED, OVERFLOW, and INVALID.
 * Note that IDLE is not an error state. A Framer in an error state
 * must be re-initialized using init(), or reset using reset(), before
 * it can deliver more frames.
 * @param that points to the Framer object.
 * @return true if error, false otherwise.
 */
static inline bool diminuto_framer_iserror(const diminuto_framer_t * that) {
    switch (that->state) {
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
    case DIMINUTO_FRAMER_STATE_INVALID:
        return true;
    default:
        return false;
    }
}

/**
 * If the Framer is COMPLETE, return the length of the received frame, or
 * EOF if it is not.
 * @param that points to the Framer object.
 * @return the frame length if COMPLETE, EOF otherwise.
 */
static inline ssize_t diminuto_framer_getlength(const diminuto_framer_t * that) {
    return diminuto_framer_iscomplete(that) ? (ssize_t)(that->length) : (ssize_t)EOF;
}

/**
 * If the Framer is COMPLETE, return a pointer to the buffer containing the
 * received frame, or NULL if it is not.
 * @param that points to the Framer object.
 * @return a pointer to the frame buffer if COMPLETE, NULL if not.
 */
static inline void * diminuto_framer_getbuffer(const diminuto_framer_t * that) {
    return diminuto_framer_iscomplete(that) ? that->buffer : (void *)0;
}

/**
 * Return true if it is LIKELY that the far-end rolled over by comparing the
 * previous and current sequence numbers. The reasoning is that the previous
 * sequence is the largest possible, and the current sequence is zero.
 * @param that points to the Framer object.
 * @return true if it is LIKELY that the far-end rolledover, false otherwise.
 */
static inline bool diminuto_framer_didrollover(const diminuto_framer_t * that) {
    return (that->previous == (DIMINUTO_FRAMER_WINDOW - 1)) && (that->sequence == 0);
}

/**
 * Return true if it is LIKELY that the far-end restarted by comparing the
 * previous and current sequence numbers. The reasoning is that the previous
 * sequence is not the largest possible, yet the current sequence is zero.
 * @param that points to the Framer object.
 * @return true if it is LIKELY that the far-end restarted, false otherwise.
 */
static inline bool diminuto_framer_didfarend(const diminuto_framer_t * that) {
    return (that->previous != (DIMINUTO_FRAMER_WINDOW - 1)) && (that->sequence == 0);
}

/**
 * Return true if it is LIKELY that the near-end restarted by comparing the
 * previous and current sequence numbers. The reasoning is that the previous
 * sequence is the largest possible, which is also the initial value, but
 * the current sequence is not zero.
 * @param that points to the Framer object.
 * @return true if it is LIKELY that the near-end restarted, false otherwise.
 */
static inline bool diminuto_framer_didnearend(const diminuto_framer_t * that) {
    return (that->previous == (DIMINUTO_FRAMER_WINDOW - 1)) && (that->sequence != 0);
}

/**
 * Return the number of POSSIBLE missing frames by comparing the previous and
 * current sequence numbers. This depends on unsigned modulo arithmetic. This same
 * result can be generated for the same difference plus any multiple of 65536.
 * Note that missing and duplicated are two ways of looking at the same thing.
 * Since the sequence number can roll over, it is possible that, for example, a
 * small number of missing frames are actually a large number of duplicated frames.
 * A useful strategy is to use the smaller of the two numbers.
 * @param that points to the Framer object.
 * @return the number of POSSIBLE missing frames.
 */
static inline size_t diminuto_framer_getmissing(const diminuto_framer_t * that) {
    uint16_t difference;
    difference = that->sequence - 1 - that->previous;
    return  difference;
}

/**
 * Return the number of POSSIBLE duplicated frames by comparing the previous and
 * current sequence numbers. This depends on unsigned modulo arithmetic. The same
 * result can be generated for the same difference plus any multiple of 65536.
 * Note that missing and duplicated are two ways of looking at the same thing.
 * Since the sequence number can roll over, it is possible that, for example, a
 * small number of duplicated frames are actually a large number of missing frames.
 * A useful strategy is to use the smaller of the two numbers.
 * @param that points to the Framer object.
 * @return the number of POSSIBLE duplicated frames.
 */
static inline size_t diminuto_framer_getduplicated(const diminuto_framer_t * that) {
    uint16_t difference;
    difference = that->previous + 1 - that->sequence;
    return difference;
}

/**
 * Return the most recent incoming sequence number.
 * @param that points to the Framer object.
 * @return the most recent incoming sequence number.
 */
static inline uint16_t diminuto_framer_getincoming(const diminuto_framer_t * that) {
    return that->sequence;
}

/**
 * Return the most recent outgoing sequence number.
 * @param that points to the Framer object.
 * @return the most recent outgoing sequence number.
 */
static inline uint16_t diminuto_framer_getoutgoing(const diminuto_framer_t * that) {
    return that->generated;
}

/**
 * Return true if the serial stream is throttled (received an XOFF) or false
 * it is not (did not receive an XOFF or received a subsequent XON).
 * @param that points to the Framer object.
 * @return true if throttled, false if not.
 */
static inline bool diminuto_framer_isthrottled(const diminuto_framer_t * that) {
    return that->throttle;
}

/*******************************************************************************
 * LOW-LEVEL API
 ******************************************************************************/

/**
 * Implements the Framer state machine tha processes input from an input
 * stream (whatever that may be) to construct a correct completed frame.
 * The caller is responsible for restarted the state machine when it
 * reaches a terminal state. Note that COMPLETE is a terminal state. Note
 * the the reader() function automatically restarts the state machine for
 * all terminal states except for COMPLETE of a non-zero length frame. If
 * there is room in the application buffer after the payload has been completely
 * and correctly received, a NUL ('\0') character is appended to the end of the
 * payload, although it is not counted in the length of the payload. This is
 * purely a convenience feature, and is not useful for non-printable payloads.
 * @param that points to the Framer object.
 * @param token is the input token, which can accomodate the stdio EOF value.
 * @return the current state of the Framer object.
 */
extern diminuto_framer_state_t diminuto_framer_machine(diminuto_framer_t * that, int token);

/**
 * Emit data to an output stream, inserting ESCAPE tokens as necessary to
 * implement the Framer byte-stuffing algorithm.
 * @param stream points to the output stream.
 * @param data points to the data to be emitted.
 * @param length is the length of the data in octets.
 * @return the total number of all octets emitted, EOF if error.
 */
extern ssize_t diminuto_framer_emit(FILE * stream, const void * data, size_t length);

/**
 * Emit the ABORT sequence to the output stream. After the ABORT sequence
 * is emitted, the output stream is flushed.
 * @param stream points to the output stream.
 * @return the total number of octets emitted, EOF if an error occurred.
 */
extern ssize_t diminuto_framer_abort(FILE * stream);

/*******************************************************************************
 * MID-LEVEL API
 ******************************************************************************/

/**
 * Read an input stream and drive a Framer state machine by feeding it
 * successive input tokens. The Framer state machine is automatically
 * restarted when it reaches the ABORT, FAILED, or OVERFLOW states. This
 * causes corrupted, aborted, or too-large frames to be automatically
 * discarded while log messages are generated. The Framer state machine is
 * also automatically restarted when it reaches the COMPLETE state and a
 * frame with a zero-length payload has been received, causing such frames
 * to be silently discarded. The reader() tries not to block, so that it can
 * be driven by a Mux or other multiplexing mechanism. In a single call it
 * processes data cached inside the input stream object, or if the
 * input stream is a TTY, in the TTY driver, until it runs out of data or
 * reaches a terminal state (including COMPLETE). The application can
 * keep calling the reader() until it returns >0 (a complete correct frame
 * received containing the returned number of octets), or EOF (an error
 * occurred). After reaching COMPLETE, the Framer must be re-initialized
 * with init() or reset with reset() to received subsequent frames.
 * @param stream points to the input stream.
 * @param that points to the Framer object.
 * @return the TOTAL number of ALL octets read if complete, 0 if busy, EOF if error.
 */
extern ssize_t diminuto_framer_reader(FILE * stream, diminuto_framer_t * that);

/**
 * Write a complete frame to the output stream, including a FLAG token, the
 * length, sequence, Fletcher checksum, payload, and Kermit cyclic redundancy
 * check. After the complete frame is emitted, the output stream is flushed.
 * The caller is blocked until the complete frame is emitted. Zero length
 * payloads not an error, even though they will be discarded on the receiving
 * end by reader(). Any NL ('\n') character in the payload will ne ESCAPEd,
 * and the emitted frame will have a NL at its end. This makes it possible
 * to use fgets(3) to receive raw frames, although such frames probably will
 * contain embedded NUL ('\0') characters, especially in the SEQUENCE and
 * LENGTH fields.
 * @param stream points to the output stream.
 * @param that points to the Framer object.
 * @param data points to the payload to be emitted.
 * @param length is the length of the payload in octets.
 * @return the TOTAL number of ALL octets written, EOF if error.
 */
extern ssize_t diminuto_framer_writer(FILE * stream, diminuto_framer_t * that, const void * data, size_t length);

/*******************************************************************************
 * HIGH-LEVEL API
 ******************************************************************************/

/**
 * Read a complete correct frame from the input stream, stripping all of
 * the control tokens, checksum, and CRC, and store the payload octets
 * into the buffer provided by the caller. The caller is blocked until the
 * a complete correct frame no larger than the buffer is processed. This
 * function uses reader() with its own local Framer object, so frames that
 * are invalid, aborted, empty, or too large, are automatically discarded
 * with a log message generated.
 * @param stream points to the input stream.
 * @param buffer is the buffer into which the payload is stored.
 * @param size is the size of the buffer in octets.
 * @return the number of PAYLOAD octets read, EOF if error.
 */
extern ssize_t diminuto_framer_read(FILE * stream, void * buffer, size_t size);

/**
 * Write a complete frame to the output stream, including a FLAG token,
 * length (in network byte order), sequence (in network byte order), Fletcher
 * checksum, payload, and Kermit cyclic redundancy check.
 * After the complete frame is emitted, the output stream is flushed.
 * The caller is blocked until the complete frame is emitted. This function
 * uses writer() with its own local Framer object.
 * @param stream points to the output stream.
 * @param data points to the payload to be emitted.
 * @param length is the length of the payload in octets.
 * @return the number of PAYLOAD octets written, EOF if error.
 */
extern ssize_t diminuto_framer_write(FILE * stream, const void * data, size_t length);

/*******************************************************************************
 * DEBUGGERS
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

/**
 * Logs the contents of a Framer object.
 * @param that points to the Framer object.
 */
extern void diminuto_framer_dump(const diminuto_framer_t * that);

#endif
