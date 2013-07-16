/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_discrete.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char ** argv)
{
	{
		diminuto_discrete_state_t state;
		diminuto_discrete_init(&state, 0);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_RISING);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_FALLING);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(!diminuto_discrete_state(&state));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
	}

	{
		diminuto_discrete_state_t state;
		diminuto_discrete_init(&state, 1);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_FALLING);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_RISING);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
	}

	{
		diminuto_discrete_state_t state;
		diminuto_discrete_init(&state, 0);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_LOW);
	}

	{
		diminuto_discrete_state_t state;
		diminuto_discrete_init(&state, 1);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == DIMINUTO_DISCRETE_EDGE_HIGH);
	}

	EXIT();
}
