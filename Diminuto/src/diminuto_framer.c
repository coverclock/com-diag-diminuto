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
#include <errno.h>
#include <ctype.h>
#include <arpa/inet.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum Token {
    FLAG        = '~',              /* 0x7e */
    ESCAPE      = '}',              /* 0x7d */
    XON         = '\x11',           /* 0x11 */
    XOFF        = '\x13',           /* 0x13 */
    RESERVED    = (uint8_t)'\xf8',  /* 0xf8 */
};

enum Mask {
    MASK        = ' ',      /* 0x20 */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef enum Action {
    FLETCHER    = 'F',
    KERMIT      = 'K',
    LENGTH      = 'L',
    PAYLOAD     = 'P',
    SKIP        = 'X',
    STORE       = 'S',
} action_t;

/*******************************************************************************
 * STATE MACHINE
 ******************************************************************************/

diminuto_framer_state_t diminuto_framer_machine(diminuto_framer_t * that, int token)
{
    char ch = '\0';
    action_t action = SKIP;
    uint16_t crc = 0;
    diminuto_framer_state_t prior = DIMINUTO_FRAMER_STATE_INITIALIZE;

    prior = that->state;

    if (token == EOF) {
        that->state = DIMINUTO_FRAMER_STATE_FINAL;
    } else {
        ch = token;
    }

    switch (that->state) {

    case DIMINUTO_FRAMER_STATE_INITIALIZE:
        that->here = (uint8_t *)&(that->length);
        that->limit = sizeof(that->length);
        that->length = 0;
        that->a = 0;
        that->b = 0;
        that->crc = 0;
        that->state = DIMINUTO_FRAMER_STATE_FLAG;
        /* Fall through. */

    case DIMINUTO_FRAMER_STATE_FLAG:
        if (ch == FLAG) {
            that->state = DIMINUTO_FRAMER_STATE_FLAGS;
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLAGS:
        if (ch == FLAG) {
            /* Do nothing. */
        } else if (ch == ESCAPE) {
            that->state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
        } else {
            action = STORE;
            that->state = DIMINUTO_FRAMER_STATE_LENGTH;
        }
        break;

    case DIMINUTO_FRAMER_STATE_LENGTH:
        if (ch == ESCAPE) {
            that->state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
        } else if (that->limit > 1) {
            action = STORE;
        } else {
            action = LENGTH;
            that->state = DIMINUTO_FRAMER_STATE_FLETCHER;
        }
        break;

    case DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED:
        if (ch == FLAG) {
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
        } else {
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_LENGTH;
            } else {
                action = LENGTH;
                that->state = DIMINUTO_FRAMER_STATE_FLETCHER;
            }
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLETCHER:
        if (ch == ESCAPE) {
            that->state = DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED;
        } else {
            if (that->limit > 1) {
                action = STORE;
            } else {
                action = FLETCHER;
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            }
        }
        break;

    case DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED:
        if (ch == FLAG) {
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
        } else {
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_FLETCHER;
            } else {
                action = FLETCHER;
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            }
        }
        break;

    case DIMINUTO_FRAMER_STATE_PAYLOAD:
        if (ch == ESCAPE) {
            that->state = DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED;
        } else if (that->limit > 1) {
            action = STORE;
        } else {
            action = PAYLOAD;
            that->state = DIMINUTO_FRAMER_STATE_KERMIT;
        }
        break;

    case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:
        if (ch == FLAG) {
            that->state = DIMINUTO_FRAMER_STATE_ABORT;
        } else {
            ch ^= MASK;
            if (that->limit > 1) {
                action = STORE;
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            } else {
                action = PAYLOAD;
                that->state = DIMINUTO_FRAMER_STATE_KERMIT;
            }
        }
        break;

    case DIMINUTO_FRAMER_STATE_KERMIT:
        if ((that->limit == 3) && diminuto_kermit_firstisvalid(ch)) {
            action = STORE;
        } else if ((that->limit == 2) && diminuto_kermit_secondisvalid(ch)) {
            action = STORE;
        } else if ((that->limit == 1) && diminuto_kermit_thirdisvalid(ch)) {
            action = KERMIT;
            that->state = DIMINUTO_FRAMER_STATE_COMPLETE;
        } else {
            that->state = DIMINUTO_FRAMER_STATE_FAILED;
        }
        break;

    case DIMINUTO_FRAMER_STATE_COMPLETE:
    case DIMINUTO_FRAMER_STATE_FINAL:
    case DIMINUTO_FRAMER_STATE_ABORT:
    case DIMINUTO_FRAMER_STATE_FAILED:
    case DIMINUTO_FRAMER_STATE_OVERFLOW:
        break;

    case DIMINUTO_FRAMER_STATE_IDLE:
        break;

    }

    switch (action) {

    case STORE:
        *(that->here++) = ch;
        --(that->limit);
        break;

    case LENGTH:
        *(that->here++) = ch;
        --(that->limit);
        (void)diminuto_fletcher_16(&(that->length), sizeof(that->length), &(that->a), &(that->b));
        that->length = ntohl(that->length);
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
        }
        break;

    case SKIP:
        break;

    }

    if (!that->debug) {
        /* Do nothing. */
    } else if (isprint(token)) {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: state (%c)+'%c'[%lu]=(%c)[%zu]\n", that, prior, token, that->length, that->state, that->limit);
    } else {
        DIMINUTO_LOG_DEBUG("diminuto_framer%p: state (%c)+'\\x%x'[%lu]=(%c)[%zu]\n", that, prior, token, that->length, that->state, that->limit);
    }

    return that->state;
}

/*******************************************************************************
 * READERS AND WRITERS
 ******************************************************************************/

ssize_t diminuto_framer_reader(FILE * stream, diminuto_framer_t * that)
{
    ssize_t result = 0;
    diminuto_framer_state_t state = DIMINUTO_FRAMER_STATE_IDLE;
    int fd = -1;
    int ch = EOF;

    fd = fileno(stream);

    do {

        state = diminuto_framer_machine(that, ch = fgetc(stream));

        switch (state) {

        case DIMINUTO_FRAMER_STATE_COMPLETE:
            result = that->length;
            DIMINUTO_LOG_DEBUG("diminuto_framer@%p(%d): complete [%zd]\n", that, fd, result);
            if (result == 0) {
                that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
            }
            break;

        case DIMINUTO_FRAMER_STATE_FINAL:
            DIMINUTO_LOG_INFORMATION("diminuto_framer@%p(%d): final\n", that, fd);
            result = EOF;
            break;

        case DIMINUTO_FRAMER_STATE_ABORT:
            DIMINUTO_LOG_INFORMATION("diminuto_framer@%p(%d): abort\n", that, fd);
            that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
            break;

        case DIMINUTO_FRAMER_STATE_FAILED:
            DIMINUTO_LOG_WARNING("diminuto_framer@%p(%d): failed\n", that, fd);
            that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
            break;

        case DIMINUTO_FRAMER_STATE_OVERFLOW:
            DIMINUTO_LOG_WARNING("diminuto_framer@%p(%d): overflow\n", that, fd);
            that->state = DIMINUTO_FRAMER_STATE_INITIALIZE;
            break;

        default:
            /* Do nothing. */
            break;

        }

    } while ((result == 0) && ((diminuto_file_ready(stream) > 0) || (diminuto_serial_valid(fd) && (diminuto_serial_available(fd) > 0))));

    return result;
}

ssize_t diminuto_framer_emit(FILE * stream, const void * data, size_t length)
{
    ssize_t result = 0;
    uint8_t * pp = (uint8_t *)data;
    uint8_t ch = '\0';
    int rc = -1;

    while ((length--) > 0) {

        ch = *(pp++);

        switch (ch) {
        case FLAG:
        case ESCAPE:
        case XON:
        case XOFF:
        case RESERVED:
            ch ^= MASK;
            rc = fputc(ESCAPE, stream);
            if (rc == EOF) {
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
            return EOF;
        }
        ++result;

    }

    return result;
}

ssize_t diminuto_framer_writer(FILE * stream, const void * data, size_t length)
{
    ssize_t result = 0;
    diminuto_framer_length_t header = 0;
    uint8_t ab[] = { 0, 0, };
    unsigned char abc[] = { ' ', ' ', ' ', '\0', };
    uint16_t crc = 0;
    ssize_t nn = 0;
    int rc = 0;

    do {

        if (length > diminuto_maximumof(diminuto_framer_length_t)) {
            errno = E2BIG;
            break;
        }

        rc = fputc(FLAG, stream);
        if (rc == EOF) {
            break;
        }
        ++result;

        header = htonl(length);
        nn = diminuto_framer_emit(stream, &header, sizeof(header));
        if (nn == EOF) {
            break;
        }
        result += nn;

        (void)diminuto_fletcher_16(&header, sizeof(header), &(ab[0]), &(ab[1]));
        nn = diminuto_framer_emit(stream, ab, sizeof(ab));
        if (nn == EOF) {
            break;
        }
        result += nn;

        nn = diminuto_framer_emit(stream, data, length);
        if (nn == EOF) {
            break;
        }
        result += nn;

        crc = diminuto_kermit_16(data, length, 0);
        diminuto_kermit_crc2chars(crc, &(abc[0]), &(abc[1]), &(abc[2]));
        rc = fputs((char *)abc, stream);
        if (rc == EOF) {
            break;
        }
        result += sizeof(abc) - 1;

        rc = fflush(stream);
        if (rc == EOF) {
            break;
        }

        return result;

    } while (false);

    return EOF;
}

ssize_t diminuto_framer_abort(FILE * stream)
{
    static const uint8_t data[] = { ESCAPE, FLAG, };
    size_t rc = 0;

    do {

        rc = fwrite(&data, sizeof(data), 1, stream);
        if (rc != 1) {
            break;
        }

        rc = fflush(stream);
        if (rc == EOF) {
            break;
        }

        return sizeof(data);

    } while (false);

    return EOF;
}
