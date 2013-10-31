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
	void * variablep;
	int value = 0xa5a5a5a5;
	int rc;

    diminuto_core_enable();

    /*
     * Test non-existent and other error legs.
     */

    check = diminuto_module_handle("COM-DIAG-DIMINUTO-MODULE-NO-SUCH-FILE-OR-DIRECTORY");
    EXPECT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load("COM-DIAG-DIMINUTO-MODULE-NO-SUCH-FILE-OR-DIRECTORY");
    EXPECT(module == (diminuto_module_handle_t)0);

    check = diminuto_module_handle("/dev/null");
    EXPECT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load("/dev/null");
    EXPECT(module == (diminuto_module_handle_t)0);

    check = diminuto_module_handle("/dev/zero");
    EXPECT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load("/dev/zero");
    EXPECT(module == (diminuto_module_handle_t)0);

    /*
     * Test basic load and unload.
     */

    check = diminuto_module_handle(MODULE);
    EXPECT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load(MODULE);
    ASSERT(module != (diminuto_module_handle_t)0);

    check = diminuto_module_handle(MODULE);
    EXPECT(check == module);

    module = diminuto_module_unload(module, !0);
    EXPECT(module == (diminuto_module_handle_t)0);

    check = diminuto_module_handle(MODULE);
    EXPECT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_handle(MODULE);
    EXPECT(check == (diminuto_module_handle_t)0);

    /*
     * Test function and variable access.
     */

    module = diminuto_module_load(MODULE);
    ASSERT(module != (diminuto_module_handle_t)0);

    check = diminuto_module_handle(MODULE);
    EXPECT(check == module);

    functionp = diminuto_module_symbol(module, "diminuto_module_example_function", (const char *)0);
    ASSERT(functionp != (void *)0);

    rc = (*(int (*)(int))functionp)(value);
    printf("0x%8.8x=(*%p)(0x%8.8x)\n", rc, functionp, value);
    EXPECT(rc == ~value);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable", (const char *)0);
    ASSERT(variablep != (void *)0);

    printf("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    EXPECT(*(int *)variablep == 0xc0edbabe);

    *(int *)variablep = 0xdeadbeef;
    printf("*%p=0x%8.8x\n", variablep, *(int *)variablep);
   EXPECT(*(int *)variablep == 0xdeadbeef);

   module = diminuto_module_unload(module, !0);
   EXPECT(module == (diminuto_module_handle_t)0);

   /*
    * Test reinitialization of static variables upon reload.
    */

    module = diminuto_module_load(MODULE);
    ASSERT(module != (diminuto_module_handle_t)0);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable", (const char *)0);
    printf("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    EXPECT(*(int *)variablep == 0xc0edbabe);

    module = diminuto_module_unload(module, !0);
    EXPECT(module == (diminuto_module_handle_t)0);

    /*
     * Test module reference counting.
     */

    module = diminuto_module_load(MODULE);
    ASSERT(module != (diminuto_module_handle_t)0);

    check = diminuto_module_load(MODULE);
    ASSERT(check != (diminuto_module_handle_t)0);

    check = diminuto_module_unload(check, 0);
    EXPECT(check == (diminuto_module_handle_t)0);

    check = module;
    ASSERT(check != (diminuto_module_handle_t)0);

    module = diminuto_module_unload(module, 0);
    EXPECT(module == (diminuto_module_handle_t)0);

    module = diminuto_module_unload(check, 0);
    EXPECT(module == check);

    check = diminuto_module_handle(MODULE);
    EXPECT(check == (diminuto_module_handle_t)0);

    return 0;
}
