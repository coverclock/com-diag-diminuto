/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Telegraph.html<BR>
 */

#include "com/diag/diminuto/diminuto_cue.h"

void diminuto_cue_init(diminuto_cue_state_t * statep, int initial)
{
    initial = !!initial;
    statep->a = initial;
    statep->b = initial;
    statep->c = initial;
    statep->r = initial;
    statep->p = initial;
}

int diminuto_cue_debounce(diminuto_cue_state_t * statep, int input)
{
    statep->c = statep->b;
    statep->b = statep->a;
    statep->a = !!input;
    statep->p = statep->r;
    return statep->r = (statep->p && (statep->a || statep->b || statep->c)) || (statep->a && statep->b && statep->c);
}

diminuto_cue_edge_t diminuto_cue_edge(const diminuto_cue_state_t * statep)
{
    diminuto_cue_edge_t result;

    if (!statep->p) {
        if (!statep->r) {
            result = DIMINUTO_CUE_EDGE_LOW;
        } else {
        	result = DIMINUTO_CUE_EDGE_RISING;
        }
    } else {
        if (!statep->r) {
        	result = DIMINUTO_CUE_EDGE_FALLING;
        } else {
        	result = DIMINUTO_CUE_EDGE_HIGH;
        }
    }

    return result;
}
