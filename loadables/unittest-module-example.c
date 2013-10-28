/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_module.h"
#include <stdio.h>

int diminuto_module_example_ctor(void) __attribute__((constructor));
int diminuto_module_example_dtor(void) __attribute__((destructor));

int diminuto_module_example_ctor(void)
{
	printf("%s[%d]\n", __FILE__, __LINE__);
	return 0;
}

int diminuto_module_example_dtor(void)
{
	printf("%s[%d]\n", __FILE__, __LINE__);
	return 0;
}

int diminuto_module_example(int input) {
	return ~input;
}
