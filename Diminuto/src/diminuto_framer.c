/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the public API for the Framer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This implements the Framer feature.
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_kermit.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "../src/diminuto_framer.h"
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static inline void streamerror(FILE * stream, const char * label)
{
    if (ferror(stream)) {
        errno = EIO;
        diminuto_perror(label);
    }
}

/*******************************************************************************
 * CTOR
 ******************************************************************************/

diminuto_framer_t * diminuto_framer_init(diminuto_framer_t * that, void * buffer, size_t size)
{
    static const diminuto_framer_t FRAMER = DIMINUTO_FRAMER_INITIALIZER;

    *that = FRAMER;

    return diminuto_framer_set(that, buffer, size);
}

/*******************************************************************************
 * LOW-LEVEL API
 ******************************************************************************/

/*
 * USEFUL ASCII CHARACTERS TO KNOW
 *
 * 0x00     '\0'    NUL, control-@
 * 0x0a     '\n'    NEWLINE a.k.a NL, Line Feed, control-J
 * 0x11     '\x11'  XON a.k.a. DC1, Device Control 1, Transmit On, control-Q
 * 0x13     '\x13'  XOFF a.k.a. DC3, Device Control 3, Transmit Off, control-S
 * 0x20     ' '     Kermit Low
 * 0x2a     '*'     ESCAPEd NEWLINE
 * 0x2f     '/'     Kermit[0] High
 * 0x31     '1'     ESCAPEd XON
 * 0x33     '3'     ESCAPEd XOFF
 * 0x5d     ']'     ESCAPEd ESCAPE
 * 0x5e     '^'     ESCAPEd FLAG
 * 0x5f     '_'     Kermit[1..2] High
 * 0x7d     '}'     ESCAPE
 * 0x7e     '~'     FLAG
 */

diminuto_framer_state_t diminuto_framer_machine(diminuto_framer_t * that, int token)
{
    uint8_t ch = '\0';
    action_t action = CONSUME;
    diminuto_framer_state_t prior = DIMINUTO_FRAMER_STATE_RESET;

    prior = that->state;

    /*
     * The standard I/O library uses (int)-1 as an End Of File (EOF)
     * indicator. We have to make sure we don't confuse this value
     * with the unsigned octet data value 0xff.
     */

    if (token == EOF) {
        that->state = DIMINUTO_FRAMER_STATE_FINAL;
    } else {
        ch = token;
        ++that->total;
    }

    switch (that->state) {

    case DIMINUTO_FRAMER_STATE_RESET:
        switch (ch) {
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            /*
             * Ignore everything else while waiting to synchronize.
             */
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLAG:
        switch (ch) {
        case NEWLINE:
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            /*
             * Multiple FLAG octets are acceptable.
             */
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            action = STORE;
            that->state = DIMINUTO_FRAMER_STATE_LENGTH;
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_LENGTH:
        switch (ch) {
        case NEWLINE:
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            if (that->limit > 1) {
                action = STORE;
            } else {
                action = LENGTH;
                that->state = DIMINUTO_FRAMER_STATE_SEQUENCE;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED:
        switch (ch) {
        case NEWLINE:
            /*
             * NEWLINE will never be valid here because XORing it with
             * MASK yields 0x2a a.k.a. '*', and the asterisk should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            /*
             * FLAG will never be valid here because XORing it with
             * MASK yields 0x5e a.k.a. '^', and the caret should
             * never have been ESCAPEd. An ESCAPE followed by a FLAG
             * is an ABORT.
             */
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
            break;
        case ESCAPE:
            /*
             * ESCAPE will never be valid here because XORing it with
             * MASK yields 0x5d a.k.a. ']', and the close bracket should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_LENGTH;
            } else {
                action = LENGTH;
                that->state = DIMINUTO_FRAMER_STATE_SEQUENCE;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_SEQUENCE:
        switch (ch) {
        case NEWLINE:
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            if (that->limit > 1) {
                action = STORE;
            } else {
                action = SEQUENCE;
                that->state = DIMINUTO_FRAMER_STATE_FLETCHER;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED:
        switch (ch) {
        case NEWLINE:
            /*
             * NEWLINE will never be valid here because XORing it with
             * MASK yields 0x2a a.k.a. '*', and the asterisk should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            /*
             * FLAG will never be valid here because XORing it with
             * MASK yields 0x5e a.k.a. '^', and the caret should
             * never have been ESCAPEd. An ESCAPE followed by a FLAG
             * is an ABORTx .
             */
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
            break;
        case ESCAPE:
            /*
             * ESCAPE will never be valid here because XORing it with
             * MASK yields 0x5d a.k.a. ']', and the close bracket should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_SEQUENCE;
            } else {
                action = SEQUENCE;
                that->state = DIMINUTO_FRAMER_STATE_FLETCHER;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLETCHER:
        switch (ch) {
        case NEWLINE:
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            if (that->limit > 1) {
                action = STORE;
            } else {
                action = FLETCHER;
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED:
        switch (ch) {
        case NEWLINE:
            /*
             * NEWLINE will never be valid here because XORing it with
             * MASK yields 0x2a a.k.a. '*', and the asterisk should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            /*
             * FLAG will never be valid here because XORing it with
             * MASK yields 0x5e a.k.a. '^', and the caret should
             * never have been ESCAPEd. An ESCAPE followed by a FLAG
             * is an ABORT.
             */
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
            break;
        case ESCAPE:
            /*
             * ESCAPE will never be valid here because XORing it with
             * MASK yields 0x5d a.k.a. ']', and the close bracket should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_FLETCHER;
            } else {
                action = FLETCHER;
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_PAYLOAD:
        switch (ch) {
        case NEWLINE:
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            if (that->limit > 1) {
                action = STORE;
            } else {
                action = PAYLOAD;
                that->state = DIMINUTO_FRAMER_STATE_KERMIT;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:
        switch (ch) {
        case NEWLINE:
            /*
             * NEWLINE will never be valid here because XORing it with
             * MASK yields 0x2a a.k.a. '*', and the asterisk should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            /*
             * FLAG will never be valid here because XORing it with
             * MASK yields 0x5e a.k.a. '^', and the caret should
             * never have been ESCAPEd. An ESCAPE followed by a FLAG
             * is an ABORT.
             */
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
            break;
        case ESCAPE:
            /*
             * ESCAPE will never be valid here because XORing it with
             * MASK yields 0x5d a.k.a. ']', and the close bracket should
             * never have been ESCAPEd.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            } else {
                action = PAYLOAD;
                that->state = DIMINUTO_FRAMER_STATE_KERMIT;
            }
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_KERMIT:
        switch (ch) {
        case NEWLINE:
            /*
             * NEWLINE falls outside of the Kermit encoding ranges.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case FLAG:
            /*
             * FLAG falls outside of the Kermit encoding ranges.
             */
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            /*
             * ESCAPE falls outside of the Kermit encoding ranges.
             */
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            /*
             * We never have to worry about the Kermit character being ESCAPEd
             * because their encoding always puts them into the ranges of
             * (' ' .. '/') or (' ' .. '_'), which exclude the control octets
             * FLAG ('~') or ESCAPE ('}'). The Kermit unit test suite verifies
             * this.
             */
            if (that->limit == 3) {
                action = STORE;
            } else if (that->limit == 2) {
                action = STORE;
            } else if (that->limit == 1) {
                action = KERMIT;
                that->state = DIMINUTO_FRAMER_STATE_NEWLINE;
            } else {
                /*
                 * Should never happen.
                 */
                that->state = DIMINUTO_FRAMER_STATE_INVALID;
            }
        }
        break;

    case DIMINUTO_FRAMER_STATE_NEWLINE:
        switch (ch) {
        case NEWLINE:
            /*
             * Requiring that the NEWLINE be the last character of the frame
             * and that it be escaped everywhere else allows the application
             * to easily accumulate a frame in a buffer without even using
             * a Framer.
             */
            action = TERMINATE;
            that->state = DIMINUTO_FRAMER_STATE_COMPLETE;
            break;
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        default:
            that->state = DIMINUTO_FRAMER_STATE_INVALID;
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_COMPLETE:
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
    case DIMINUTO_FRAMER_STATE_INVALID:
    case DIMINUTO_FRAMER_STATE_IDLE:
        switch (ch) {
        case XON:
            that->throttle = false;
            break;
        case XOFF:
            that->throttle = true;
            break;
        }
        break;

    }

    switch (action) {

    case RESET:
        that->here = (uint8_t *)&(that->length0);
        that->limit = sizeof(that->length0);
        that->length0 = 0;
        that->total = 1; /* Because we already consumed the FLAG. */
        /*
         * Do not reset sequence, previous, or generated.
         */
        that->crc = 0;
        that->a = 0;
        that->b = 0;
        break;

    case STORE:
        diminuto_contract(that->limit > 0);
        *(that->here++) = ch;
        --(that->limit);
        break;

    case LENGTH:
        diminuto_contract(that->limit > 0);
        *(that->here++) = ch;
        --(that->limit);
        /*
         * Must calculate checksum on network byte ordered field so all
         * architectures see the same value.
         */
        that->a = that->b = 0;
        (void)diminuto_fletcher_16(&(that->length0), sizeof(that->length0), &(that->a), &(that->b));
        that->here = (uint8_t *)&(that->sequence0);
        that->limit = sizeof(that->sequence0);
        break;

    case SEQUENCE:
        diminuto_contract(that->limit > 0);
        *(that->here++) = ch;
        --(that->limit);
        /*
         * Must calculate checksum on network byte ordered field so all
         * architectures see the same value.
         */
        (void)diminuto_fletcher_16(&(that->sequence0), sizeof(that->sequence0), &(that->a), &(that->b));
        that->here = (uint8_t *)&(that->sum);
        that->limit = sizeof(that->sum);
        break;

    case FLETCHER:
        diminuto_contract(that->limit > 0);
        *(that->here++) = ch;
        --(that->limit);
        if ((that->sum[0] != that->a) || (that->sum[1] != that->b)) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else {
            /*
             * Don't use LENGTH or SEQUENCE fields' values until the
             * checksum has been validated.
             */
            that->length0 = ntohs(that->length0);
            that->sequence0 = ntohs(that->sequence0);
            if (that->length0 == 0) {
                that->here = (uint8_t *)&(that->check);
                that->limit = sizeof(that->check);
                that->state = DIMINUTO_FRAMER_STATE_KERMIT;
            } else if (that->length0 > that->size) {
                that->state = DIMINUTO_FRAMER_STATE_OVERFLOW;
            } else {
                that->here = (uint8_t *)(that->buffer);
                that->limit = that->length0;
            }
        }
        break;

    case PAYLOAD:
        diminuto_contract(that->limit > 0);
        *(that->here++) = ch;
        --(that->limit);
        that->crc = diminuto_kermit_16(that->buffer, that->length0, 0);
        that->here = (uint8_t *)&(that->check);
        that->limit = sizeof(that->check);
        break;

    case KERMIT:
        diminuto_contract(that->limit > 0);
        *(that->here++) = ch;
        --(that->limit);
        if (!diminuto_kermit_firstisvalid(that->check[0])) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else if (!diminuto_kermit_secondisvalid(that->check[1])) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else if (!diminuto_kermit_thirdisvalid(that->check[2])) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else if ((that->crc0 = diminuto_kermit_chars2crc(that->check[0], that->check[1], that->check[2])) != that->crc) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else {
            /* Do nothing. */
        }
        break;

    case TERMINATE:
        /*
         * If and only if the length of the received payload is less than
         * the size of the application buffer, store a NUL ('\0') character
         * past the end of the received payload. Like the NL ('\n') character
         * at the end of every frame, this is mostly a convenience feature,
         * and definitely not useful for binary data.
         */
        if (that->length0 < that->size) {
            ((uint8_t *)that->buffer)[that->length0] = '\0';
        }
        /*
         * We only report sequence numbers for successfully completed
         * frames. Disgarded, invalid, failed, aborted, restarted, or
         * otherwise uncompleted frames will be revealed to the applicaiton
         * by a discrepancy in the previous and current sequence numbers.
         * Similarly, we don't update the length field until the frame
         * is completed.
         */
        that->previous = that->sequence;
        that->sequence = that->sequence0;
        that->length = that->length0;
        break;

    case CONSUME:
        /*
         * Do nothing.
         */
        break;

    }

    if (!that->debug) {
        /*
         * Do nothing.
         */
    } else if (isprint(token)) {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: state {%c}+'%c'={%c}[%zu]\n", that, prior, token, that->state, that->limit);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: state {%c}+'\\x%x'={%c}[%zu]\n", that, prior, token, that->state, that->limit);
    }

    return that->state;
}

ssize_t diminuto_framer_emit(FILE * stream, const void * data, size_t length)
{
    ssize_t result = 0;
    uint8_t * pp = (uint8_t *)data;
    uint8_t ch = '\0';
    int rc = -1;

    while ((length--) > 0) {

        ch = *(pp++);

        /*
         * The octets FLAG and ESCAPE are ESCAPEd so that they are not
         * interpreted as control octets. XON and XOFF are ESCAPEd so
         * that they can be transmitted, as an ESCAPEd '1' and an ESCAPEd
         * '3' respectively, so that they are not interpreted by the serial
         * driver as the software flow control octets DC1 or DC3. (Other
         * special device control characters could be added to this list
         * as necessary.)
         */

        switch (ch) {
        case NEWLINE:
        case FLAG:
        case ESCAPE:
        case XON:
        case XOFF:
            ch ^= MASK;
            rc = fputc(ESCAPE, stream);
            if (rc == EOF) {
                streamerror(stream, "fputc");
                return EOF;
            }
            ++result;
            break;
        default:
            /* Do nothing. */
            break;
        }

        rc = fputc(ch, stream);
        if (rc == EOF) {
            streamerror(stream, "fputc");
            return EOF;
        }
        ++result;

        /*
         * emit() does not flush the standard I/O stream.
         */

    }

    return result;
}

ssize_t diminuto_framer_abort(FILE * stream)
{
    ssize_t result = EOF;
    static const uint8_t data[] = { ESCAPE, FLAG, };
    size_t rc = 0;

    do {

        rc = fwrite(&data, sizeof(data), 1, stream);
        if (rc != 1) {
            errno = EIO;
            diminuto_perror("fwrite");
            break;
        }

        rc = fflush(stream);
        if (rc == EOF) {
            diminuto_perror("fflush");
            break;
        }

        result = sizeof(data);

    } while (false);

    return result;
}

/*******************************************************************************
 * MID-LEVEL API
 ******************************************************************************/

ssize_t diminuto_framer_reader(FILE * stream, diminuto_framer_t * that)
{
    ssize_t result = 0;
    diminuto_framer_state_t state = DIMINUTO_FRAMER_STATE_IDLE;
    diminuto_framer_state_t prior = DIMINUTO_FRAMER_STATE_IDLE;
    int token = EOF;

    do {

        token = fgetc(stream);
        if (token == EOF) {
            streamerror(stream, "fgetc");
        }

        prior = that->state;
        state = diminuto_framer_machine(that, token);

        switch (state) {

        case DIMINUTO_FRAMER_STATE_COMPLETE:
            if (that->length == 0) {
                DIMINUTO_LOG_INFORMATION("diminuto_framer@%p: empty? {%c} [%u]\n", that, prior, that->length);
                diminuto_framer_reset(that);
            } else {
                DIMINUTO_LOG_DEBUG("diminuto_framer@%p: complete. {%c} #%u [%zu] [%u]\n", that, prior, that->sequence, that->total, that->length);
                result = that->total;
            }
            break;

        case DIMINUTO_FRAMER_STATE_FINAL:
            result = EOF;
            DIMINUTO_LOG_INFORMATION("diminuto_framer@%p: final! {%c} (%d)\n", that, prior, token);
            break;

        case DIMINUTO_FRAMER_STATE_ABORT:
            DIMINUTO_LOG_NOTICE("diminuto_framer@%p: abort! {%c} '\\x%2.2x'\n", that, prior, token);
            diminuto_framer_reset(that);
            break;

        case DIMINUTO_FRAMER_STATE_FAILED:
            DIMINUTO_LOG_WARNING("diminuto_framer@%p: failed! {%c} (0x%2.2x%2.2x) (0x%2.2x%2.2x) (0x%4.4x) (0x%4.4x)\n", that, prior, that->sum[0], that->sum[1], that->a, that->b, that->crc0, that->crc);
            diminuto_framer_reset(that);
            break;

        case DIMINUTO_FRAMER_STATE_OVERFLOW:
            DIMINUTO_LOG_ERROR("diminuto_framer@%p: overflow! {%c} [%u] [%zu]\n", that, prior, that->length0, that->size);
            diminuto_framer_reset(that);
            break;

        case DIMINUTO_FRAMER_STATE_INVALID:
            DIMINUTO_LOG_WARNING("diminuto_framer@%p: invalid! {%c} '\\x%2.2x' [%zu]\n", that, prior, token, that->total);
            diminuto_framer_reset(that);
            break;

        default:
            /* Do nothing. */
            break;

        }

    } while ((result == 0) && (diminuto_file_ready(stream) > 0));

    /*
     * (result <  0) : EOF or other terminal serial error.
     * (result == 0) : continue accumulating frame octets.
     * (result >  0) : the total number of octets read for the completed frame.
     */

    return result;
}

ssize_t diminuto_framer_writer(FILE * stream, diminuto_framer_t * that, const void * data, size_t length)
{
    ssize_t result = EOF;
    uint16_t header = 0;
    uint16_t generated = 0;
    uint8_t fletcher[2] = { 0, 0, };
    unsigned char kermit[3] = { ' ', ' ', ' ' };
    uint16_t crc = 0;
    ssize_t emitted = 0;
    ssize_t total = 0;
    size_t written = 0;
    int rc = 0;

    do {

        if (length > diminuto_maximumof(uint16_t)) {
            errno = E2BIG;
            break;
        }

        /*
         * FLAG[1]
         */

        rc = fputc(FLAG, stream);
        if (rc == EOF) {
            streamerror(stream, "fputc");
            break;
        }
        total += 1;

        /*
         * Must calculate checksum on network byte ordered fields so both
         * big endian and little endian architectures see the same value.
         */

        /*
         * LENGTH[4]
         */

        header = htons(length);
        (void)diminuto_fletcher_16(&header, sizeof(header), &(fletcher[0]), &(fletcher[1]));
        emitted = diminuto_framer_emit(stream, &header, sizeof(header));
        if (emitted == EOF) {
            break;
        }
        total += emitted;

        /*
         * SEQUENCE[2]
         */

        generated = htons(++that->generated);
        (void)diminuto_fletcher_16(&generated, sizeof(generated), &(fletcher[0]), &(fletcher[1]));
        emitted = diminuto_framer_emit(stream, &generated, sizeof(generated));
        if (emitted == EOF) {
            break;
        }
        total += emitted;

        /*
         * FLETCHER[2]
         */

        emitted = diminuto_framer_emit(stream, fletcher, sizeof(fletcher));
        if (emitted == EOF) {
            break;
        }
        total += emitted;

        /*
         * PAYLOAD[LENGTH]
         */

        if (length > 0) {
            crc = diminuto_kermit_16(data, length, 0);
            emitted = diminuto_framer_emit(stream, data, length);
            if (emitted == EOF) {
                break;
            }
            total += emitted;
        }

        /*
         * KERMIT[3]
         */

        diminuto_kermit_crc2chars(crc, &(kermit[0]), &(kermit[1]), &(kermit[2]));
        written = fwrite(kermit, sizeof(kermit), 1, stream);
        if (written != 1) {
            errno = EIO;
            diminuto_perror("fwrite");
            break;
        }
        total += sizeof(kermit);

        /*
         * NEWLINE[1]
         */

        rc = fputc(NEWLINE, stream);
        if (rc == EOF) {
            streamerror(stream, "fputc");
            break;
        }
        total += 1;

        rc = fflush(stream);
        if (rc == EOF) {
            diminuto_perror("fflush");
            break;
        }

        DIMINUTO_LOG_DEBUG("diminuto_framer@%p: emitted. #%u [%zd] [%zu]\n", that, that->generated, total, length);

        result = total;

    } while (false);

    /*
     * (result <  0) : EOF or other terminal serial error.
     * (result == 0) : cannot occur.
     * (result >  0) : the total number of octets written for the entire frame.
     */

    return result;
}

/*******************************************************************************
 * HIGH-LEVEL API
 ******************************************************************************/

ssize_t diminuto_framer_read(FILE * stream, void * buffer, size_t size)
{
    ssize_t result = EOF;
    ssize_t octets = EOF;
    diminuto_framer_t framer = DIMINUTO_FRAMER_INITIALIZER;
   
    (void)diminuto_framer_set(&framer, buffer, size); 

    do {
        octets = diminuto_framer_reader(stream, &framer);
    } while (octets == 0);

    if (octets > 0) {
        result = framer.length;
    }

    return result;
}

ssize_t diminuto_framer_write(FILE * stream, const void * data, size_t length)
{
    ssize_t result = EOF;
    ssize_t octets = EOF;
    diminuto_framer_t framer = DIMINUTO_FRAMER_INITIALIZER;
   
    octets = diminuto_framer_writer(stream, &framer, data, length);

    if (octets > 0) {
        result = length;
    }

    return result;
}

/*******************************************************************************
 * DUMPER
 ******************************************************************************/
 
void diminuto_framer_dump(const diminuto_framer_t * that)
{
    diminuto_log_emit("framer@%p: buffer=%p\n", that, that->buffer);
    diminuto_log_emit("framer@%p: here=%p\n", that, that->here);
    diminuto_log_emit("framer@%p: size=%zu\n", that, that->size);
    diminuto_log_emit("framer@%p: limit=%zu\n", that, that->limit);
    diminuto_log_emit("framer@%p: total=%zu\n", that, that->total);
    diminuto_log_emit("framer@%p: state='%c'\n", that, that->state);
    diminuto_log_emit("framer@%p: length0=%u\n", that, that->length0);
    diminuto_log_emit("framer@%p: length=%u\n", that, that->length);
    diminuto_log_emit("framer@%p: sequence0=%u\n", that, that->sequence0);
    diminuto_log_emit("framer@%p: sequence=%u\n", that, that->sequence);
    diminuto_log_emit("framer@%p: previous=%u\n", that, that->previous);
    diminuto_log_emit("framer@%p: generated=%u\n", that, that->generated);
    diminuto_log_emit("framer@%p: crc=0x%4.4x\n", that, that->crc);
    diminuto_log_emit("framer@%p: crc0=0x%4.4x\n", that, that->crc);
    diminuto_log_emit("framer@%p: a=0x%2.2x b=0x%2.2x\n", that, that->a, that->b);
    diminuto_log_emit("framer@%p: sum=[0x%2.2x,0x%2.2x]\n", that, that->sum[0], that->sum[1]);
    diminuto_log_emit("framer@%p: check=[0x%2.2x,0x%2.2x,0x%2.2x] valid=[%d,%d,%d] crc=0x%4.4x\n", that, that->check[0], that->check[1], that->check[2], diminuto_kermit_firstisvalid(that->check[0]), diminuto_kermit_secondisvalid(that->check[1]), diminuto_kermit_thirdisvalid(that->check[2]), diminuto_kermit_chars2crc(that->check[0], that->check[1], that->check[2]));
    diminuto_log_emit("framer@%p: throttle=%d\n", that, that->throttle);
    diminuto_log_emit("framer@%p: debug=%d\n", that, that->debug);
}
