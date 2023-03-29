/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Framer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Framer feature.
 */

#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <ctype.h>
#include <string.h>

int main(void)
{
    SETLOGMASK();

    {
        diminuto_framer_t framer;

        TEST();

        ASSERT(sizeof(diminuto_framer_length_t) == sizeof(uint32_t));
        ASSERT(sizeof(framer.length) == sizeof(uint32_t));
        ASSERT(sizeof(framer.sum) == 2);
        ASSERT(sizeof(framer.check) == 3);

        STATUS();
    }

    {
        diminuto_framer_state_t state = DIMINUTO_FRAMER_STATE_INITIALIZE;

        TEST();

        /*
         * If I botch the definition of the state enumeration, this
         * should get an error or warning at compile time.
         */

        switch (state) {
        case DIMINUTO_FRAMER_STATE_INITIALIZE:          break;
        case DIMINUTO_FRAMER_STATE_FLAG:                break;
        case DIMINUTO_FRAMER_STATE_LENGTH:              break;
        case DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED:      break;
        case DIMINUTO_FRAMER_STATE_FLETCHER:            break;
        case DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED:    break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD:             break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_KERMIT:              break;
        case DIMINUTO_FRAMER_STATE_COMPLETE:            break;
        case DIMINUTO_FRAMER_STATE_FINAL:               break;
        case DIMINUTO_FRAMER_STATE_ABORT:               break;
        case DIMINUTO_FRAMER_STATE_FAILED:              break;
        case DIMINUTO_FRAMER_STATE_OVERFLOW:            break;
        case DIMINUTO_FRAMER_STATE_IDLE:                break;
        }

        STATUS();
    }

    {
        TEST();

        ASSERT(EOF == -1);

        STATUS();
    }

    {
        diminuto_framer_t framer = { (void*)0, };
        diminuto_framer_t * ff = (diminuto_framer_t *)0;
        char buffer[64];

        TEST();

        ff = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        ASSERT(ff == &framer);
        ASSERT(framer.buffer == buffer);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_INITIALIZE);

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ff = diminuto_framer_reinit(&framer);
        ASSERT(ff == &framer);
        ASSERT(framer.buffer == buffer);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_INITIALIZE);

        ASSERT(framer.length == 0);
        framer.length = sizeof(buffer) / 2;
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
fprintf(stderr, "%u %ld\n", framer.length, diminuto_framer_length(&framer));
        ASSERT(diminuto_framer_length(&framer) == -1);
        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        ASSERT(diminuto_framer_buffer(&framer) == buffer);
        ASSERT(diminuto_framer_length(&framer) == (sizeof(buffer) / 2));

        STATUS();
    }

    EXIT();
}
