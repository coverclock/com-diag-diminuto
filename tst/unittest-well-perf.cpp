/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * With -O3 optimization, run on a
 *
 * vendor_id       : GenuineIntel
 * cpu family      : 6
 * model           : 15
 * model name      : Intel(R) Core(TM)2 Quad CPU    Q6600  @ 2.40GHz
 * cpu MHz         : 1600.000
 * cache size      : 4096 KB
 *
 * this program yields
 *
 * TEST 0: BEGIN
 * TEST 0: END 538965us
 * TEST 1: BEGIN
 * TEST 1: END 417154us
 * TEST 2: BEGIN
 * TEST 2: END 418591us
 * TEST 3: BEGIN
 * TEST 3: END 1303391us
 * TEST 4: BEGIN
 * TEST 4: END 1300946us
 */

#include "com/diag/diminuto/diminuto_well.h"

extern "C" {
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_time.h"
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
	int mask;
	int bit;
	diminuto_ticks_t time;

	mask = (argc < 2) ? ~0 : atoi(argv[1]);

	/* No well. */

	bit = 0;
	if ((mask & (1 << bit)) != 0) {
		printf("TEST %d: BEGIN\n", bit);
		time = diminuto_time_thread();
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(framistat); ++jj) {
				framistat[jj] = new Framistat(jj);
				ASSERT(framistat[jj] != (Framistat *)0);
			}
			for (jj = 0; jj < countof(framistat); ++jj) {
				delete framistat[jj];
			}
		}
		printf("TEST %d: END %lluus\n", bit, diminuto_time_thread() - time);
	}

	/* Thread-unsafe well. */

	bit = 1;
	if ((mask & (1 << bit)) != 0) {
		printf("TEST %d: BEGIN\n", bit);
		time = diminuto_time_thread();
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(doohickey); ++jj) {
				doohickey[jj] = new Doohickey(jj);
				ASSERT(doohickey[jj] != (Doohickey *)0);
			}
			for (jj = 0; jj < countof(doohickey); ++jj) {
				delete doohickey[jj];
			}
		}
		printf("TEST %d: END %lluus\n", bit, diminuto_time_thread() - time);
	}

	/* Thread-unsafe well from base class. */

	bit = 2;
	if ((mask & (1 << bit)) != 0) {
		printf("TEST %d: BEGIN\n", bit);
		time = diminuto_time_thread();
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(framistat); ++jj) {
				framistat[jj] = new Doohickey(jj);
				ASSERT(framistat[jj] != (Framistat *)0);
			}
			for (jj = 0; jj < countof(framistat); ++jj) {
				delete framistat[jj];
			}
		}
		printf("TEST %d: END %lluus\n", bit, diminuto_time_thread() - time);
	}

	/* Thread-safe well. */

	bit = 3;
	if ((mask & (1 << bit)) != 0) {
		printf("TEST %d: BEGIN\n", bit);
		time = diminuto_time_thread();
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(thingamajig); ++jj) {
				thingamajig[jj] = new Thingamajig(jj);
				ASSERT(thingamajig[jj] != (Thingamajig *)0);
			}
			for (jj = 0; jj < countof(thingamajig); ++jj) {
				delete thingamajig[jj];
			}
		}
		printf("TEST %d: END %lluus\n", bit, diminuto_time_thread() - time);
	}

	/* Thread-safe well from base class. */

	bit = 4;
	if ((mask & (1 << bit)) != 0) {
		printf("TEST %d: BEGIN\n", bit);
		time = diminuto_time_thread();
		for (ii = 0; ii < ITERATIONS; ++ii) {
			for (jj = 0; jj < countof(framistat); ++jj) {
				framistat[jj] = new Thingamajig(jj);
				ASSERT(framistat[jj] != (Framistat *)0);
			}
			for (jj = 0; jj < countof(framistat); ++jj) {
				delete framistat[jj];
			}
		}
		printf("TEST %d: END %lluus\n", bit, diminuto_time_thread() - time);
	}

	EXIT();
}
