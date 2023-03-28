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
#include "com/diag/diminuto/diminuto_kermit.h"
#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include <arpa/inet.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum Token {
    FLAG        = '~',      /* 0x7e */
    ESCAPE      = '}',      /* 0x7d */
    XON         = '\x11',   /* 0x11 */
    XOFF        = '\x13',   /* 0x13 */
    RESERVED    = '\xf8',   /* 0xf8 */
};

enum Mask {
    MASK        = ' ',      /* 0x20 */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef enum Action {
    DONE        = 'D',
    FLETCHER    = 'F',
    KERMIT      = 'K',
    LENGTH      = 'L',
    SKIP        = 'X',
    PAYLOAD     = 'P',
    STORE       = 'S',
} action_t;

/*******************************************************************************
 * IMPLEMENTATIONS
 ******************************************************************************/

ssize_t diminuto_framer_writer(diminuto_framer_t * that, const void * data, size_t length)
{
    return -1;
}

ssize_t diminuto_framer_cancel(diminuto_framer_t * that)
{
    return -1;
}

diminuto_framer_state_t diminuto_framer_reader(diminuto_framer_t * that)
{
    int rc = 0;
    char ch = '\0';
    int fd = -1;
    action_t action = SKIP;
    uint16_t crc = 0;

    do {

        if ((rc = getc(that->stream)) == EOF) {
            that->state = DIMINUTO_FRAMER_STATE_FINAL;
        } else {
            ch = rc;
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
                that->state = DIMINUTO_FRAMER_STATE_LENGTH;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH:
            if (ch == FLAG) {
                /* Do nothing. */
            } else if (ch == ESCAPE) {
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
                action = DONE;
                that->state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= MASK;
                if (that->limit > 1) {
                    action = STORE;
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
                action = DONE;
                that->state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= MASK;
                if (that->limit > 1) {
                    action = STORE;
                } else {
                    action = FLETCHER;
                    that->state = DIMINUTO_FRAMER_STATE_PAYLOAD;
                }
            }
            break;

        case DIMINUTO_FRAMER_STATE_PAYLOAD:
            if (ch == ESCAPE) {
                that->state = DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED;
            } else {
                action = STORE;
                if (that->limit <= 1) {
                    that->state = DIMINUTO_FRAMER_STATE_KERMIT;
                }
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:
            if (ch == FLAG) {
                action = DONE;
                that->state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= MASK;
                action = STORE;
                if (that->limit <= 1) {
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
                action = DONE;
                that->state = DIMINUTO_FRAMER_STATE_FAILED;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_COMPLETE:
        case DIMINUTO_FRAMER_STATE_FINAL:
        case DIMINUTO_FRAMER_STATE_ABORT:
        case DIMINUTO_FRAMER_STATE_FAILED:
        case DIMINUTO_FRAMER_STATE_OVERFLOW:
            action = DONE;
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
                action = DONE;
            } else if (that->length > that->size) {
                that->state = DIMINUTO_FRAMER_STATE_OVERFLOW;
                action = DONE;
            } else {
                that->here = (uint8_t *)&(that->buffer);
                that->limit = that->size;
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
                action = DONE;
            }
            break;

        case DONE:
            break;

        case SKIP:
            break;

        }

    } while ((action != DONE) && ((diminuto_file_ready(that->stream) > 0) || (diminuto_serial_valid(fd = fileno(that->stream)) && (diminuto_serial_available(fd) > 0))));

    return that->state;
}
