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

#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include "com/diag/diminuto/diminuto_kermit.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "../src/diminuto_framer.h"
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef enum Action {
    FLETCHER    = 'F',      /* Process Fletcher-16 checksum. */
    KERMIT      = 'K',      /* Process Kermit-16 cyclic redundancy check. */
    LENGTH      = 'L',      /* Process length and Fletcher-16 checksum. */
    PAYLOAD     = 'P',      /* Compute Kermit-6 cyclic redundancy check. */
    RESET       = 'R',      /* Initialize Framer for next frame. */
    SKIP        = 'X',      /* Skip this octet. */
    SEQUENCE    = 'N',      /* Process sequence and Fletcher-16 checksum. */
    STORE       = 'S',      /* Process this octet and Kermit-16 CRC. */
} action_t;

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

diminuto_framer_t * diminuto_framer_init(diminuto_framer_t * that, void * buffer, size_t size) {
    memset(that, 0, sizeof(*that));
    that->buffer = buffer;
    that->size = size;
    that->sequence = diminuto_maximumof(uint16_t);
    that->previous = that->sequence - 1;
    that->outgoing = diminuto_maximumof(uint16_t);

    return diminuto_framer_reset(that);
}

/*******************************************************************************
 * LOW-LEVEL API
 ******************************************************************************/

/*
 * USEFUL ASCII CHARACTERS TO KNOW
 *
 * 0x11     DC1     XON
 * 0x13     DC2     XOFF
 * 0x20     ' '     Kermit Low
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
    action_t action = SKIP;
    uint16_t crc = 0;
    diminuto_framer_state_t prior = DIMINUTO_FRAMER_STATE_RESET;

    prior = that->state;

    if (token == EOF) {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: EOF\n", that);
        that->state = DIMINUTO_FRAMER_STATE_FINAL;
    } else {
        ch = token;
    }

    switch (that->state) {

    case DIMINUTO_FRAMER_STATE_RESET:
        switch (ch) {
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        default:
            /*
             * Ignore everything but a FLAG.
             */
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLAG:
        switch (ch) {
        case FLAG:
            /*
             * Multiple FLAG octets must be acceptable, since HDLC frames
             * both begin and end with a FLAG, with successive frames only
             * optionally having a single FLAG (not two) in between.
             */
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
            break;
        case XON:
        case XOFF:
            /*
             * Ignore software flow control.
             */
            break;
        default:
            action = STORE;
            that->state = DIMINUTO_FRAMER_STATE_LENGTH;
            break;
        }
        break;

    case DIMINUTO_FRAMER_STATE_LENGTH:
        switch (ch) {
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
            break;
        case XON:
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED;
            break;
        case XON:
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED;
            break;
        case XON:
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case FLAG:
            action = RESET;
            that->state = DIMINUTO_FRAMER_STATE_FLAG;
            break;
        case ESCAPE:
            that->state = DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED;
            break;
        case XON:
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case XOFF:
            /*
             * Ignore software flow control.
             */
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
        case XOFF:
            /*
             * Ignore software flow control.
             */
            break;
        default:
            /*
             * We never have to worry about the Kermit character being ESCAPEd
             * because their encoding always puts them into the ranges of
             * (' ' .. '/') or (' ' .. '_'), which exclude the control octets
             * FLAG ('~') or ESCAPE ('}'). The Kermit unit test suite verifies
             * this.
             */
            if ((that->limit == 3) && diminuto_kermit_firstisvalid(ch)) {
                action = STORE;
            } else if ((that->limit == 2) && diminuto_kermit_secondisvalid(ch)) {
                action = STORE;
            } else if ((that->limit == 1) && diminuto_kermit_thirdisvalid(ch)) {
                action = KERMIT;
                that->state = DIMINUTO_FRAMER_STATE_COMPLETE;
            } else {
                that->state = DIMINUTO_FRAMER_STATE_INVALID;
            }
        }
        break;

    case DIMINUTO_FRAMER_STATE_COMPLETE:
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
    case DIMINUTO_FRAMER_STATE_INVALID:
    case DIMINUTO_FRAMER_STATE_IDLE:
    default:
        break;

    }

    switch (action) {

    case RESET:
        that->here = (uint8_t *)&(that->length);
        that->limit = sizeof(that->length);
        that->total = 0;
        that->length = 0;
        /*
         * Do not reset sequence, previous, or outgoing.
         */
        that->crc = 0;
        that->a = 0;
        that->b = 0;
        break;

    case STORE:
        *(that->here++) = ch;
        --(that->limit);
        break;

    case LENGTH:
        *(that->here++) = ch;
        --(that->limit);
        /*
         * Must calculate checksum on network byte ordered field so all
         * architectures see the same value.
         */
        that->a = that->b = 0;
        (void)diminuto_fletcher_16(&(that->length), sizeof(that->length), &(that->a), &(that->b));
        that->length = ntohl(that->length);
        that->here = (uint8_t *)&(that->candidate);
        that->limit = sizeof(that->candidate);
        break;

    case SEQUENCE:
        *(that->here++) = ch;
        --(that->limit);
        /*
         * Must calculate checksum on network byte ordered field so all
         * architectures see the same value.
         */
        (void)diminuto_fletcher_16(&(that->candidate), sizeof(that->candidate), &(that->a), &(that->b));
        that->candidate = ntohs(that->candidate);
        that->here = (uint8_t *)&(that->sum);
        that->limit = sizeof(that->sum);
        break;

    case FLETCHER:
        *(that->here++) = ch;
        --(that->limit);
        if ((that->sum[0] != that->a) || (that->sum[1] != that->b)) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else if (that->length == 0) {
            that->here = (uint8_t *)&(that->check);
            that->limit = sizeof(that->check);
            that->state = DIMINUTO_FRAMER_STATE_KERMIT;
        } else if (that->length > that->size) {
            that->state = DIMINUTO_FRAMER_STATE_OVERFLOW;
        } else {
            that->here = (uint8_t *)(that->buffer);
            that->limit = that->length;
        }
        break;

    case PAYLOAD:
        *(that->here++) = ch;
        --(that->limit);
        that->crc = diminuto_kermit_16(that->buffer, that->length, 0);
        that->here = (uint8_t *)&(that->check);
        that->limit = sizeof(that->check);
        break;

    case KERMIT:
        *(that->here++) = ch;
        --(that->limit);
        crc = diminuto_kermit_chars2crc(that->check[0], that->check[1], that->check[2]);
        if (crc != that->crc) {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        } else {
            /*
             * We only report sequence numbers for successfully completed
             * frames. Disgarded, invalid, failed, aborted, restarted, or
             * otherwise uncompleted frames will be revealed to the applicaiton
             * by a discrepancy in the previous and current sequence numbers.
             * Exception: when the sequence field rolls over from 65535 to
             * 0.
             */
            that->previous = that->sequence;
            that->sequence = that->candidate;
        }
        break;

    case SKIP:
    default:
        break;

    }

    if (!that->debug) {
        /* Do nothing. */
    } else if (isprint(token)) {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: state (%c)+'%c'[%u]=(%c)[%zu]\n", that, prior, token, that->length, that->state, that->limit);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: state (%c)+'\\x%x'[%u]=(%c)[%zu]\n", that, prior, token, that->length, that->state, that->limit);
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
    int ch = EOF;
    int fd = -1;

    fd = fileno(stream);

    do {

        ch = fgetc(stream);
        if (ch == EOF) {
            streamerror(stream, "fgetc");
        } else {
            ++that->total;
        }

        state = diminuto_framer_machine(that, ch);

        /*
         * We have take care that a high speed by very noisy serial
         * connection doesn't flood the log. So most of the logging
         * is done at DEBUG or INFORMATION log levels.
         */

        switch (state) {

        case DIMINUTO_FRAMER_STATE_COMPLETE:
            if (that->length == 0) {
                DIMINUTO_LOG_INFORMATION("diminuto_framer@%p: empty?\n", that);
                diminuto_framer_reset(that);
            } else {
                result = that->total;
                DIMINUTO_LOG_DEBUG("diminuto_framer@%p: complete. [%zd]\n", that, result);
            }
            break;

        case DIMINUTO_FRAMER_STATE_FINAL:
            result = EOF;
            DIMINUTO_LOG_INFORMATION("diminuto_framer@%p: final!\n", that);
            break;

        case DIMINUTO_FRAMER_STATE_ABORT:
            DIMINUTO_LOG_NOTICE("diminuto_framer@%p: abort!\n", that);
            diminuto_framer_reset(that);
            break;

        case DIMINUTO_FRAMER_STATE_FAILED:
            DIMINUTO_LOG_INFORMATION("diminuto_framer@%p: failed!\n", that);
            diminuto_framer_reset(that);
            break;

        case DIMINUTO_FRAMER_STATE_OVERFLOW:
            DIMINUTO_LOG_ERROR("diminuto_framer@%p: overflow!\n", that);
            diminuto_framer_reset(that);
            break;

        case DIMINUTO_FRAMER_STATE_INVALID:
            DIMINUTO_LOG_INFORMATION("diminuto_framer@%p: invalid!\n", that);
            diminuto_framer_reset(that);
            break;

        default:
            /* Do nothing. */
            break;

        }

    } while ((result == 0) && ((diminuto_file_ready(stream) > 0) || (diminuto_serial_valid(fd) && (diminuto_serial_available(fd) > 0))));

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
    uint32_t header = 0;
    uint16_t sequence = 0;
    uint8_t fletcher[2] = { 0, 0, };
    unsigned char kermit[3] = { ' ', ' ', ' ' };
    uint16_t crc = 0;
    ssize_t emitted = 0;
    ssize_t total = 0;
    size_t written = 0;
    int rc = 0;

    do {

        if (length > diminuto_maximumof(uint32_t)) {
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

        header = htonl(length);
        (void)diminuto_fletcher_16(&header, sizeof(header), &(fletcher[0]), &(fletcher[1]));
        emitted = diminuto_framer_emit(stream, &header, sizeof(header));
        if (emitted == EOF) {
            break;
        }
        total += emitted;

        /*
         * SEQUENCE[2]
         */

        sequence = htons(++that->outgoing);
        (void)diminuto_fletcher_16(&sequence, sizeof(sequence), &(fletcher[0]), &(fletcher[1]));
        emitted = diminuto_framer_emit(stream, &sequence, sizeof(sequence));
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
         * HDLC requires a FLAG at the end of every frame, but this FLAG
         * can also be the start of the next frame. This functions puts
         * a FLAG and the start *and* at the end of every frame, and the
         * state machine must ignore extra FLAGs at the beginning of the
         * next frame.
         */

        rc = fputc(FLAG, stream);
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
    ssize_t readen = EOF; /* Because past tense "read" is a system call. */
    diminuto_framer_t framer = DIMINUTO_FRAMER_INIT;
   
    (void)diminuto_framer_init(&framer, buffer, size); 

    do {
        readen = diminuto_framer_reader(stream, &framer);
    } while (readen == 0);

    if (readen > 0) {
        result = framer.length;
    }

    (void)diminuto_framer_fini(&framer);

    return result;
}

ssize_t diminuto_framer_write(FILE * stream, const void * data, size_t length)
{
    ssize_t result = EOF;
    ssize_t written = EOF;
    diminuto_framer_t framer = DIMINUTO_FRAMER_INIT;
   
    (void)diminuto_framer_init(&framer, (void *)0, 0); 

    written = diminuto_framer_writer(stream, &framer, data, length);

    if (written > 0) {
        result = length;
    }

    (void)diminuto_framer_fini(&framer);

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
    diminuto_log_emit("framer@%p: length=%u\n", that, that->length);
    diminuto_log_emit("framer@%p: candidate=%u\n", that, that->candidate);
    diminuto_log_emit("framer@%p: sequence=%u\n", that, that->sequence);
    diminuto_log_emit("framer@%p: previous=%u\n", that, that->previous);
    diminuto_log_emit("framer@%p: outgoing=%u\n", that, that->outgoing);
    diminuto_log_emit("framer@%p: crc=0x%4.4x\n", that, that->crc);
    diminuto_log_emit("framer@%p: a=0x%2.2x b=0x%2.2x\n", that, that->a, that->b);
    diminuto_log_emit("framer@%p: sum=[0x%2.2x,0x%2.2x]\n", that, that->sum[0], that->sum[1]);
    diminuto_log_emit("framer@%p: check=[0x%2.2x,0x%2.2x,0x%2.2x] valid=[%d,%d,%d] crc=0x%4.4x\n", that, that->check[0], that->check[1], that->check[2], diminuto_kermit_firstisvalid(that->check[0]), diminuto_kermit_secondisvalid(that->check[1]), diminuto_kermit_thirdisvalid(that->check[2]), diminuto_kermit_chars2crc(that->check[0], that->check[1], that->check[2]));
    diminuto_log_emit("framer@%p: debug=%d\n", that, that->debug);
}
