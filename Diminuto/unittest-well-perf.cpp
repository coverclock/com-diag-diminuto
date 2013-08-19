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
#include <stdlib.h>

enum {
	ITERATIONS = 10000,
	CARDINALITY = 1000,
};

class Framistat {

public:

	int skoshification;

	Framistat()
	: skoshification(-1)
	{}

	Framistat(int skosh)
	: skoshification(skosh)
	{}

	virtual ~Framistat() {}

	int discombobulate() {
		return skoshification;
	}

};

class Doohickey : public Framistat {

public:

	Doohickey()
	: Framistat()
	{}

	Doohickey(int skosh)
	: Framistat(skosh)
	{}

	virtual ~Doohickey() {}

	COM_DIAG_DIMINUTO_WELL_DECLARATION(Doohickey);

};

COM_DIAG_DIMINUTO_WELL_DEFINITION(Doohickey, CARDINALITY);

class Thingamajig : public Framistat {

public:

	Thingamajig()
	: Framistat()
	{}

	Thingamajig(int skosh)
	: Framistat(skosh)
	{}

	virtual ~Thingamajig() {}

	COM_DIAG_DIMINUTO_SAFEWELL_DECLARATION(Thingamajig);

};

COM_DIAG_DIMINUTO_SAFEWELL_DEFINITION(Thingamajig, CARDINALITY);

int main(int argc, char ** argv) {
	Framistat * framistat[CARDINALITY];
	Doohickey * doohickey[countof(framistat)];
	Thingamajig * thingamajig[countof(framistat)];
	size_t ii;
	size_t jj;
	int test;

	test = (argc < 2) ? 0 : atoi(argv[1]);
	ASSERT((0 <= test) && (test <= 4));
	printf("TEST %d: BEGIN\n", test);

	switch (test) {
	case 0:
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(framistat); ++jj) {
				framistat[jj] = new Framistat(jj);
				ASSERT(framistat[jj] != (Framistat *)0);
			}
			for (jj = 0; jj < countof(framistat); ++jj) {
				delete framistat[jj];
			}
		}
		break;
	case 1:
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(doohickey); ++jj) {
				doohickey[jj] = new Doohickey(jj);
				ASSERT(doohickey[jj] != (Doohickey *)0);
			}
			for (jj = 0; jj < countof(doohickey); ++jj) {
				delete doohickey[jj];
			}
		}
		break;
	case 2:
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(framistat); ++jj) {
				framistat[jj] = new Doohickey(jj);
				ASSERT(framistat[jj] != (Framistat *)0);
			}
			for (jj = 0; jj < countof(framistat); ++jj) {
				delete framistat[jj];
			}
		}
		break;
	case 3:
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(thingamajig); ++jj) {
				thingamajig[jj] = new Thingamajig(jj);
				ASSERT(thingamajig[jj] != (Thingamajig *)0);
			}
			for (jj = 0; jj < countof(thingamajig); ++jj) {
				delete thingamajig[jj];
			}
		}
		break;
	case 4:
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(framistat); ++jj) {
				framistat[jj] = new Thingamajig(jj);
				ASSERT(framistat[jj] != (Framistat *)0);
			}
			for (jj = 0; jj < countof(framistat); ++jj) {
				delete framistat[jj];
			}
		}
		break;
	default:
		break;
	}

	printf("TEST %d: END\n", test);
	EXIT();
}
