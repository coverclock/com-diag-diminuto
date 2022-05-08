/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Cue feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Cue feature.
 */

#include "com/diag/diminuto/diminuto_cue.h"

diminuto_cue_state_t * diminuto_cue_init(diminuto_cue_state_t * statep, int initial)
{
    initial = !!initial;
    statep->a = initial;
    statep->b = initial;
    statep->c = initial;
    statep->r = initial;
    statep->p = initial;
    return statep;
}

int diminuto_cue_debounce(diminuto_cue_state_t * statep, int input)
{
    statep->c = statep->b;
    statep->b = statep->a;
    statep->a = !!input;
    statep->p = statep->r;
    return statep->r = (statep->p & (statep->a | statep->b | statep->c)) | (statep->a & statep->b & statep->c);
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
