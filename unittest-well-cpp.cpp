/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * It is really helpful to run this with the valgrind tool.
 */

#include "com/diag/diminuto/diminuto_well.h"

extern "C" {
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_unittest.h"
}

#include <stdio.h>

enum {
	CARDINALITY = 10,
	LIMIT = 4,
};

class Framistat {

public:

	int skoshification;

	Framistat(int skosh)
	: skoshification(skosh)
	{
		printf("%p->Framistat(%d)\n", this, skosh);
	}

	~Framistat() {
		printf("%p->~Framistat() skoshification=%d\n", this, skoshification);
	}

	int discombobulate() {
		return skoshification;
	}

	COM_DIAG_DIMINUTO_WELL_DECLARATION(Framistat);

};

COM_DIAG_DIMINUTO_WELL_DEFINITION(Framistat, CARDINALITY);

int main(int argc, char ** argv) {
	Framistat * framistat[CARDINALITY];
	Framistat * fail;
	size_t ii;
	size_t jj;

	printf("sizeof(Framistat)=%zu\n", sizeof(Framistat));

	for (jj = 0; jj < LIMIT; ++jj) {

		ASSERT(Framistat::well.isFull());
		for (ii = 0; ii < countof(framistat); ++ii) {
			framistat[ii] = new Framistat(ii);
			ASSERT(framistat[ii] != (Framistat *)0);
			ASSERT(!Framistat::well.isFull());
		}
		ASSERT(Framistat::well.isEmpty());

		for (ii = 0; ii < countof(framistat); ++ii) {
			ASSERT(framistat[ii]->discombobulate() == ii);
		}

		for (ii = 0; ii < LIMIT; ++ii) {
			try {
				fail = new Framistat(countof(framistat));
				ASSERT(false);
			} catch (std::bad_alloc oom) {
				/* Do nothing. */
			} catch (...) {
				ASSERT(false);
			}
		}

		for (ii = 0; ii < LIMIT; ++ii) {
			fail = new(std::nothrow) Framistat(countof(framistat));
			ASSERT(fail == (Framistat *)0);
		}

		ASSERT(Framistat::well.isEmpty());
		for (ii = 0; ii < countof(framistat); ++ii) {
			delete framistat[countof(framistat) - ii - 1];
			ASSERT(!Framistat::well.isEmpty());
		}
		ASSERT(Framistat::well.isFull());

	}

	EXIT();
}
