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
        TEST();

        ASSERT(sizeof(diminuto_framer_length_t) == 4);

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
        case DIMINUTO_FRAMER_STATE_LENGTH1:             break;
        case DIMINUTO_FRAMER_STATE_LENGTH1_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_LENGTH2:             break;
        case DIMINUTO_FRAMER_STATE_LENGTH2_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_LENGTH3:             break;
        case DIMINUTO_FRAMER_STATE_LENGTH3_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_LENGTH4:             break;
        case DIMINUTO_FRAMER_STATE_LENGTH4_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_FLETCHERA:           break;
        case DIMINUTO_FRAMER_STATE_FLETCHERA_ESCAPED:   break;
        case DIMINUTO_FRAMER_STATE_FLETCHERB:           break;
        case DIMINUTO_FRAMER_STATE_FLETCHERB_ESCAPED:   break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD:             break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_CRC1:                break;
        case DIMINUTO_FRAMER_STATE_CRC2:                break;
        case DIMINUTO_FRAMER_STATE_CRC3:                break;
        case DIMINUTO_FRAMER_STATE_COMPLETE:            break;
        case DIMINUTO_FRAMER_STATE_FINAL:               break;
        case DIMINUTO_FRAMER_STATE_ABORT:               break;
        case DIMINUTO_FRAMER_STATE_FAILED:              break;
        case DIMINUTO_FRAMER_STATE_IDLE:                break;
        }

        STATUS();
    }

    EXIT();
}
