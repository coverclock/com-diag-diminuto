/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_path.h"
#include "com/diag/diminuto/diminuto_module.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <stdio.h>
#include <errno.h>

const char KEYWORD[] = "LD_MODULE_PATH";
const char NAME[] = "unittest-module-example.so";

int main(int argc, char ** argv)
{
	char * file;
	diminuto_module_handle_t module;
	diminuto_module_handle_t check;
	void * functionp;
	void * variablep;
	int value = 0xa5a5a5a5;
	int rc;

	SETLOGMASK();

    diminuto_core_enable();

    file = diminuto_path_find(KEYWORD, NAME);
    if (file == (const char *)0) {
    	DIMINUTO_LOG_ERROR("%s not found amongst %s!\n", NAME, KEYWORD);
    }
    ASSERT(file != (const char *)0);

    /*
     * Test basic load and unload.
     */

    check = diminuto_module_handle(file);
    ASSERT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

    /*
     * N.B: On i386, check will equal module.
     *      On ARM, check will be NULL.
     */
#if defined(__arm__)
#	warning Suppressing module handle check.
#elif 1
    check = diminuto_module_handle(file);
    EXPECT(check != (diminuto_module_handle_t)0);
    EXPECT(check == module);
#else
#	warning Suppressing module handle check.
#endif

    module = diminuto_module_unload(module, !0);
    ASSERT(module == (diminuto_module_handle_t)0);

    check = diminuto_module_handle(file);
    ASSERT(check == (diminuto_module_handle_t)0);

    /*
     * Test function and variable access.
     */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

    functionp = diminuto_module_symbol(module, "diminuto_module_example_function", (const char *)0);
    ASSERT(functionp != (void *)0);

    rc = (*(int (*)(int))functionp)(value);
    DIMINUTO_LOG_DEBUG("0x%8.8x=(*%p)(0x%8.8x)\n", rc, functionp, value);
    ASSERT(rc == ~value);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable", (const char *)0);
    ASSERT(variablep != (void *)0);

    DIMINUTO_LOG_DEBUG("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    ASSERT(*(int *)variablep == 0xc0edbabe);

    *(int *)variablep = 0xdeadbeef;
    DIMINUTO_LOG_DEBUG("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    ASSERT(*(int *)variablep == 0xdeadbeef);

    functionp = diminuto_module_symbol(module, "diminuto_module_example_function_notfound", (const char *)0);
    ASSERT(functionp == (void *)0);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable_notfound", (const char *)0);
    ASSERT(variablep == (void *)0);

	module = diminuto_module_unload(module, !0);
	ASSERT(module == (diminuto_module_handle_t)0);

   /*
    * Test reinitialization of static variables upon reload.
    */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable", (const char *)0);
    DIMINUTO_LOG_DEBUG("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    ASSERT(*(int *)variablep == 0xc0edbabe);

    module = diminuto_module_unload(module, !0);
    ASSERT(module == (diminuto_module_handle_t)0);

    /*
     * Test module reference counting.
     */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

    check = diminuto_module_load(file);
    ASSERT(check != (diminuto_module_handle_t)0);

    check = diminuto_module_unload(check, 0);
    ASSERT(check == (diminuto_module_handle_t)0);

    check = module;
    ASSERT(check != (diminuto_module_handle_t)0);

    module = diminuto_module_unload(module, 0);
    ASSERT(module == (diminuto_module_handle_t)0);

    module = diminuto_module_unload(check, 0);
    ASSERT(module == check);

    check = diminuto_module_handle(file);
    ASSERT(check == (diminuto_module_handle_t)0);

    free(file);

    /*
     * Test non-existent and other error legs.
     */

    check = diminuto_module_handle("COM-DIAG-DIMINUTO-MODULE-NO-SUCH-FILE-OR-DIRECTORY");
    ASSERT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load("COM-DIAG-DIMINUTO-MODULE-NO-SUCH-FILE-OR-DIRECTORY");
    ASSERT(module == (diminuto_module_handle_t)0);

    check = diminuto_module_handle("/dev/null");
    ASSERT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load("/dev/null");
    ASSERT(module == (diminuto_module_handle_t)0);

    check = diminuto_module_handle("/dev/zero");
    ASSERT(check == (diminuto_module_handle_t)0);

    module = diminuto_module_load("/dev/zero");
    ASSERT(module == (diminuto_module_handle_t)0);

    EXIT();
}
