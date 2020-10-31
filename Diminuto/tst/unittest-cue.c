/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Cue feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Cue feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_cue.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        TEST();
        ASSERT( !(DIMINUTO_CUE_EDGE_LOW     & DIMINUTO_CUE_MASK_CURRENT ));
        ASSERT( !(DIMINUTO_CUE_EDGE_LOW     & DIMINUTO_CUE_MASK_PRIOR   ));
        ASSERT( !(DIMINUTO_CUE_EDGE_LOW     & DIMINUTO_CUE_MASK_CHANGING));
        ASSERT(!!(DIMINUTO_CUE_EDGE_RISING  & DIMINUTO_CUE_MASK_CURRENT ));
        ASSERT( !(DIMINUTO_CUE_EDGE_RISING  & DIMINUTO_CUE_MASK_PRIOR   ));
        ASSERT(!!(DIMINUTO_CUE_EDGE_RISING  & DIMINUTO_CUE_MASK_CHANGING));
        ASSERT(!!(DIMINUTO_CUE_EDGE_HIGH    & DIMINUTO_CUE_MASK_CURRENT ));
        ASSERT(!!(DIMINUTO_CUE_EDGE_HIGH    & DIMINUTO_CUE_MASK_PRIOR   ));
        ASSERT( !(DIMINUTO_CUE_EDGE_HIGH    & DIMINUTO_CUE_MASK_CHANGING));
        ASSERT( !(DIMINUTO_CUE_EDGE_FALLING & DIMINUTO_CUE_MASK_CURRENT ));
        ASSERT(!!(DIMINUTO_CUE_EDGE_FALLING & DIMINUTO_CUE_MASK_PRIOR   ));
        ASSERT(!!(DIMINUTO_CUE_EDGE_FALLING & DIMINUTO_CUE_MASK_CHANGING));
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        ASSERT(diminuto_cue_init(&state, 0) == &state);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_RISING);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_FALLING);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(!diminuto_cue_is_rising(&state));
        ASSERT(!diminuto_cue_is_falling(&state));
        ASSERT(!diminuto_cue_state(&state));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(diminuto_cue_fini(&state) == (diminuto_cue_state_t *)0);
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        ASSERT(diminuto_cue_init(&state, !0) == &state);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_FALLING);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_RISING);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_fini(&state) == (diminuto_cue_state_t *)0);
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        ASSERT(diminuto_cue_init(&state, 0) == &state);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(!diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_LOW);
        ASSERT(diminuto_cue_fini(&state) == (diminuto_cue_state_t *)0);
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        ASSERT(diminuto_cue_init(&state, !0) == &state);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, 0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_debounce(&state, !0));
        ASSERT(diminuto_cue_edge(&state) == DIMINUTO_CUE_EDGE_HIGH);
        ASSERT(diminuto_cue_fini(&state) == (diminuto_cue_state_t *)0);
        STATUS();
    }

    EXIT();
}
