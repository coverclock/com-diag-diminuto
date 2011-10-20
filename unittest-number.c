/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto/diminuto_unittest.h"
#include "diminuto/diminuto_number.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
	const char * p;
	diminuto_unsigned_t n;

    errno = 0;

    EXPECT((*diminuto_number("00", &n) == '\0') && (n == 0) && (errno == 0));
    EXPECT((*diminuto_number("01", &n) == '\0') && (n == 1) && (errno == 0));
    EXPECT((*diminuto_number("02", &n) == '\0') && (n == 2) && (errno == 0));
    EXPECT((*diminuto_number("03", &n) == '\0') && (n == 3) && (errno == 0));
    EXPECT((*diminuto_number("04", &n) == '\0') && (n == 4) && (errno == 0));
    EXPECT((*diminuto_number("05", &n) == '\0') && (n == 5) && (errno == 0));
    EXPECT((*diminuto_number("06", &n) == '\0') && (n == 6) && (errno == 0));
    EXPECT((*diminuto_number("07", &n) == '\0') && (n == 7) && (errno == 0));
    EXPECT((*diminuto_number("044", &n) == '\0') && (n == 36) && (errno == 0));
    EXPECT((*diminuto_number("0377", &n) == '\0') && (n == 255) && (errno == 0));
    EXPECT((*diminuto_number("0177777", &n) == '\0') && (n == 65535) && (errno == 0));
    EXPECT((*diminuto_number("0", &n) == '\0') && (n == 0) && (errno == 0));
    EXPECT((*diminuto_number("1", &n) == '\0') && (n == 1) && (errno == 0));
    EXPECT((*diminuto_number("2", &n) == '\0') && (n == 2) && (errno == 0));
    EXPECT((*diminuto_number("3", &n) == '\0') && (n == 3) && (errno == 0));
    EXPECT((*diminuto_number("4", &n) == '\0') && (n == 4) && (errno == 0));
    EXPECT((*diminuto_number("5", &n) == '\0') && (n == 5) && (errno == 0));
    EXPECT((*diminuto_number("6", &n) == '\0') && (n == 6) && (errno == 0));
    EXPECT((*diminuto_number("7", &n) == '\0') && (n == 7) && (errno == 0));
    EXPECT((*diminuto_number("8", &n) == '\0') && (n == 8) && (errno == 0));
    EXPECT((*diminuto_number("9", &n) == '\0') && (n == 9) && (errno == 0));
    EXPECT((*diminuto_number("25", &n) == '\0') && (n == 25) && (errno == 0));
    EXPECT((*diminuto_number("4294967295", &n) == '\0') && (n == 4294967295UL) && (errno == 0));
    EXPECT((*diminuto_number("0x0", &n) == '\0') && (n == 0) && (errno == 0));
    EXPECT((*diminuto_number("0x1", &n) == '\0') && (n == 1) && (errno == 0));
    EXPECT((*diminuto_number("0x2", &n) == '\0') && (n == 2) && (errno == 0));
    EXPECT((*diminuto_number("0x3", &n) == '\0') && (n == 3) && (errno == 0));
    EXPECT((*diminuto_number("0x4", &n) == '\0') && (n == 4) && (errno == 0));
    EXPECT((*diminuto_number("0x5", &n) == '\0') && (n == 5) && (errno == 0));
    EXPECT((*diminuto_number("0x6", &n) == '\0') && (n == 6) && (errno == 0));
    EXPECT((*diminuto_number("0x7", &n) == '\0') && (n == 7) && (errno == 0));
    EXPECT((*diminuto_number("0x8", &n) == '\0') && (n == 8) && (errno == 0));
    EXPECT((*diminuto_number("0x9", &n) == '\0') && (n == 9) && (errno == 0));
    EXPECT((*diminuto_number("0xa", &n) == '\0') && (n == 10) && (errno == 0));
    EXPECT((*diminuto_number("0xb", &n) == '\0') && (n == 11) && (errno == 0));
    EXPECT((*diminuto_number("0xc", &n) == '\0') && (n == 12) && (errno == 0));
    EXPECT((*diminuto_number("0xd", &n) == '\0') && (n == 13) && (errno == 0));
    EXPECT((*diminuto_number("0xe", &n) == '\0') && (n == 14) && (errno == 0));
    EXPECT((*diminuto_number("0xf", &n) == '\0') && (n == 15) && (errno == 0));
    EXPECT((*diminuto_number("0X0", &n) == '\0') && (n == 0) && (errno == 0));
    EXPECT((*diminuto_number("0X1", &n) == '\0') && (n == 1) && (errno == 0));
    EXPECT((*diminuto_number("0X2", &n) == '\0') && (n == 2) && (errno == 0));
    EXPECT((*diminuto_number("0X3", &n) == '\0') && (n == 3) && (errno == 0));
    EXPECT((*diminuto_number("0X4", &n) == '\0') && (n == 4) && (errno == 0));
    EXPECT((*diminuto_number("0X5", &n) == '\0') && (n == 5) && (errno == 0));
    EXPECT((*diminuto_number("0X6", &n) == '\0') && (n == 6) && (errno == 0));
    EXPECT((*diminuto_number("0X7", &n) == '\0') && (n == 7) && (errno == 0));
    EXPECT((*diminuto_number("0X8", &n) == '\0') && (n == 8) && (errno == 0));
    EXPECT((*diminuto_number("0X9", &n) == '\0') && (n == 9) && (errno == 0));
    EXPECT((*diminuto_number("0Xa", &n) == '\0') && (n == 10) && (errno == 0));
    EXPECT((*diminuto_number("0Xb", &n) == '\0') && (n == 11) && (errno == 0));
    EXPECT((*diminuto_number("0Xc", &n) == '\0') && (n == 12) && (errno == 0));
    EXPECT((*diminuto_number("0Xd", &n) == '\0') && (n == 13) && (errno == 0));
    EXPECT((*diminuto_number("0Xe", &n) == '\0') && (n == 14) && (errno == 0));
    EXPECT((*diminuto_number("0Xf", &n) == '\0') && (n == 15) && (errno == 0));
    EXPECT((*diminuto_number("0xA", &n) == '\0') && (n == 10) && (errno == 0));
    EXPECT((*diminuto_number("0xB", &n) == '\0') && (n == 11) && (errno == 0));
    EXPECT((*diminuto_number("0xC", &n) == '\0') && (n == 12) && (errno == 0));
    EXPECT((*diminuto_number("0xD", &n) == '\0') && (n == 13) && (errno == 0));
    EXPECT((*diminuto_number("0xE", &n) == '\0') && (n == 14) && (errno == 0));
    EXPECT((*diminuto_number("0xF", &n) == '\0') && (n == 15) && (errno == 0));
    EXPECT((*diminuto_number("0XA", &n) == '\0') && (n == 10) && (errno == 0));
    EXPECT((*diminuto_number("0XB", &n) == '\0') && (n == 11) && (errno == 0));
    EXPECT((*diminuto_number("0XC", &n) == '\0') && (n == 12) && (errno == 0));
    EXPECT((*diminuto_number("0XD", &n) == '\0') && (n == 13) && (errno == 0));
    EXPECT((*diminuto_number("0XE", &n) == '\0') && (n == 14) && (errno == 0));
    EXPECT((*diminuto_number("0XF", &n) == '\0') && (n == 15) && (errno == 0));
    EXPECT((*diminuto_number("0x7fffffff", &n) == '\0') && (n == 2147483647UL) && (errno == 0));
    EXPECT((*diminuto_number("0xffffffff", &n) == '\0') && (n == 4294967295UL) && (errno == 0));
    EXPECT((*diminuto_number("0xffffffffffffffff", &n) == '\0') && (n == 0xffffffffffffffffULL) && (errno == 0));

	--argc;
	++argv;
	while ((argc--) > 0) {
		p = diminuto_number(*(argv++), &n);
		printf("0%llo 0x%llx %llu %lld 0x%x\n", n, n, n, n, *p);
	}

	return 0;
}
