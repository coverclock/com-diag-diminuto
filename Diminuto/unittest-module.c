/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_module.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <stdio.h>
#include <errno.h>

const char MODULE[] = "./loadables/unittest-module-example.so";

int main(int argc, char ** argv)
{
	diminuto_module_handle_t module;
	diminuto_module_handle_t check;
	void * functionp;
	int value = 0xa5a5a5a5;
	int rc;

    diminuto_core_enable();

    check = diminuto_module_handle(MODULE);
    EXPECT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load(MODULE);
    ASSERT(module != (diminuto_module_handle_t)0);

    check = diminuto_module_handle(MODULE);
    EXPECT(check == module);

    functionp = diminuto_module_symbol(module, "diminuto_module_example", (const char *)0);
    ASSERT(functionp != (void *)0);

    rc = (*(int (*)(int))functionp)(value);
    printf("0x%8.8x=(*%p)(0x%8.8x)\n", rc, functionp, value);
    EXPECT(rc == ~value);


    module = diminuto_module_unload(module);
    EXPECT(module == (diminuto_module_handle_t)0);

    /*
    check = diminuto_module_handle(MODULE);
    EXPECT(check == (diminuto_module_handle_t)0);
    */

    return 0;
}
