/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
        diminuto_cue_init(&state, 0);
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
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        diminuto_cue_init(&state, !0);
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
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        diminuto_cue_init(&state, 0);
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
        STATUS();
    }

    {
        diminuto_cue_state_t state;
        TEST();
        diminuto_cue_init(&state, !0);
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
        STATUS();
    }

    EXIT();
}
