/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_discrete.h"

void diminuto_discrete_init(diminuto_discrete_state_t * statep, int initial) {
	initial = !!initial;
	statep->a = initial;
	statep->b = initial;
	statep->c = initial;
	statep->r = initial;
	statep->p = initial;
}

int diminuto_discrete_debounce(diminuto_discrete_state_t * statep, int input) {
	statep->c = statep->b;
	statep->b = statep->a;
	statep->a = !!input;
	statep->p = statep->r;
	return statep->r = (statep->p && (statep->a || statep->b || statep->c)) || (statep->a && statep->b && statep->c);
}

diminuto_discrete_edge_t diminuto_discrete_edge(const diminuto_discrete_state_t * statep) {
	if (!statep->p) {
		if (!statep->r) {
			return DIMINUTO_DISCRETE_EDGE_LOW;
		} else {
			return DIMINUTO_DISCRETE_EDGE_RISING;
		}
	} else {
		if (!statep->r) {
			return DIMINUTO_DISCRETE_EDGE_FALLING;
		} else {
			return DIMINUTO_DISCRETE_EDGE_HIGH;
		}
	}
}
