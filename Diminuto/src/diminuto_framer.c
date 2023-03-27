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
#if !defined(_BSD_SOURCE)
#define _BSD_SOURCE
#endif
#include <endian.h>

ssize_t diminuto_framer_writer(FILE * fp, const void * buffer, size_t length)
{
    return -1;
}

ssize_t diminuto_framer_cancel(FILE * fp)
{
    return -1;
}

typedef enum Action {
    PASS        = 'P',
    STORE       = 'S',
    LENGTH      = 'L',
    DONE        = 'D',
} action_t;

diminuto_framer_state_t diminuto_framer_reader(FILE * fp, void * buffer, size_t size, diminuto_framer_state_t state, uint8_t ** herep, size_t * lengthp, uint16_t * crcp)
{
    int rc = 0;
    char ch = '\0';
    int fd = -1;
    action_t action = PASS;
    diminuto_framer_length_t length = 0;

    do {

        if ((rc = fgetc(fp)) == EOF) {
            state = DIMINUTO_FRAMER_STATE_FINAL;
        } else {
            ch = rc;
        }

        switch (state) {
    
        case DIMINUTO_FRAMER_STATE_INIT:
            *herep = (uint8_t *)lengthp;
            *lengthp = 0;
            *crcp = 0;
            state = DIMINUTO_FRAMER_STATE_FLAG;
            /* Fall through. */
    
        case DIMINUTO_FRAMER_STATE_FLAG:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                state = DIMINUTO_FRAMER_STATE_LENGTH1;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH1:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                /* Do nothing. */
            } else if (ch == DIMINUTO_FRAMER_TOKEN_ESCAPE) {
                state = DIMINUTO_FRAMER_STATE_LENGTH1_ESCAPED;
            } else {
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH2;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH1_ESCAPED:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                action = DONE;
                state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= DIMINUTO_FRAMER_CONSTANT_XOR;
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH2;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH2:
            if (ch == DIMINUTO_FRAMER_TOKEN_ESCAPE) {
                state = DIMINUTO_FRAMER_STATE_LENGTH2_ESCAPED;
            } else {
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH3;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH2_ESCAPED:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                action = DONE;
                state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= DIMINUTO_FRAMER_CONSTANT_XOR;
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH3;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH3:
            if (ch == DIMINUTO_FRAMER_TOKEN_ESCAPE) {
                state = DIMINUTO_FRAMER_STATE_LENGTH4_ESCAPED;
            } else {
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH4;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH3_ESCAPED:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                action = DONE;
                state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= DIMINUTO_FRAMER_CONSTANT_XOR;
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH4;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH4:
            if (ch == DIMINUTO_FRAMER_TOKEN_ESCAPE) {
                state = DIMINUTO_FRAMER_STATE_LENGTH4_ESCAPED;
            } else {
                action = STORE;
/*
ntoh32
hton32
*/
                state = DIMINUTO_FRAMER_STATE_PAYLOAD;
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_LENGTH4_ESCAPED:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                action = DONE;
                state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
                ch ^= DIMINUTO_FRAMER_CONSTANT_XOR;
                action = STORE;
                state = DIMINUTO_FRAMER_STATE_LENGTH2;
            }
            break;

        case DIMINUTO_FRAMER_STATE_FLETCHERA:
            break;

        case DIMINUTO_FRAMER_STATE_FLETCHERA_ESCAPED:
            break;

        case DIMINUTO_FRAMER_STATE_FLETCHERB:
            break;

        case DIMINUTO_FRAMER_STATE_FLETCHERB_ESCAPED:
            break;
    
        case DIMINUTO_FRAMER_STATE_PAYLOAD:
            break;
    
        case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:
            if (ch == DIMINUTO_FRAMER_TOKEN_FLAG) {
                action = DONE;
                state = DIMINUTO_FRAMER_STATE_ABORT;
            } else {
            }
            break;
    
        case DIMINUTO_FRAMER_STATE_CRC1:
            break;
    
        case DIMINUTO_FRAMER_STATE_CRC2:
            break;
    
        case DIMINUTO_FRAMER_STATE_CRC3:
            break;
    
        case DIMINUTO_FRAMER_STATE_COMPLETE:
            action = DONE;
            break;
    
        case DIMINUTO_FRAMER_STATE_FINAL:
            action = DONE;
            break;
    
        case DIMINUTO_FRAMER_STATE_ABORT:
            action = DONE;
            break;
    
        case DIMINUTO_FRAMER_STATE_IDLE:
            action = DONE;
            break;
    
        }

    } while ((action != DONE) && ((diminuto_file_ready(fp) > 0) || (diminuto_serial_valid(fd = fileno(fp)) && (diminuto_serial_available(fd) > 0))));

    return state;
}
