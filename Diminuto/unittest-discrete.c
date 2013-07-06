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
		diminuto_discrete_initialize(&state, 0);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == RISING);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == FALLING);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
	}

	{
		diminuto_discrete_state_t state;
		diminuto_discrete_initialize(&state, 1);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == FALLING);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == RISING);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
	}

	{
		diminuto_discrete_state_t state;
		diminuto_discrete_initialize(&state, 0);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
		ASSERT(!diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == LOW);
	}

	{
		diminuto_discrete_state_t state;
		diminuto_discrete_initialize(&state, 1);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, 0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
		ASSERT(diminuto_discrete_debounce(&state, !0));
		ASSERT(diminuto_discrete_edge(&state) == HIGH);
	}

	EXIT();
}
