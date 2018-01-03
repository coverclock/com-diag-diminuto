/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * Copyright 2012-2018 Digital Aggregates Corporation, Colorado, USA.
 * Author: Chip Overclock <mailto:coverclock@diag.com>.
 * Licensed under the terms in LICENSE.txt.
 */

#include "diminuto_hamming.h"
#include "com/diag/diminuto/diminuto_hamming.h"

uint8_t diminuto_hamming84_nib2sym(uint8_t nib) {
	uint8_t sym;
	/* Encode D1..D4 into S1..S4. */
	sym = diminuto_hamming84_dat2sym(nib);
	/* Encode P1, P2, and P3. */
	sym |= diminuto_hamming84_sym2par1(sym) | diminuto_hamming84_sym2par2(sym) | diminuto_hamming84_sym2par3(sym);
	/* Encode P4. */
	return sym | diminuto_hamming84_sym2par4(sym);
}

uint8_t diminuto_hamming84_sym2nib(uint8_t sym, uint8_t * errorsp) {
	uint8_t fix;
	if (diminuto_hamming84_sym_is_valid(sym)) {
		/* No detectable bit errors. */
		*errorsp = 0;
		return diminuto_hamming84_sym2dat(sym);
	}
	fix = sym ^ (1 << diminuto_hamming84_sym2off(sym));
	if (diminuto_hamming84_sym_is_valid(fix)) {
		/* One correctable bit error in S1..S4 or P1..P3. */
		*errorsp = 1;
		return diminuto_hamming84_sym2dat(fix);
	}
	fix = sym ^ H84_P4_BIT;
	if (diminuto_hamming84_sym_is_valid(fix)) {
		/* One correctable bit error in P4. */
		*errorsp = 1;
		return diminuto_hamming84_sym2dat(fix);
	}
	/* Two or more detectable bit errors. */
	*errorsp = 2;
	return 0;
}
