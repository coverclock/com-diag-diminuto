/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Module feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Module feature.
 *
 * This unit tests requires a "make most" which makes *more* than
 * make all.
 *
 * uClibc doesn't implement all of the underlying mechanisms used by this
 * feature. Worse, bionic (Android) seems to implement quite different
 * semantics.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_path.h"
#include "com/diag/diminuto/diminuto_module.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

const char KEYWORD[] = "LD_MODULE_PATH";
const char NAME[] = "unittest-module-example.so";
const char ALTERNATIVE[] = "unittest-module-example.dll";

int main(int argc, char ** argv)
{
    char * file;
    char * alternative;
    const char * paths;
    diminuto_module_handle_t module;
    diminuto_module_handle_t check;
    void * functionp;
    void * variablep;
    int value = 0xa5a5a5a5;
    int rc;

    SETLOGMASK();

    diminuto_core_enable();

    paths = getenv(KEYWORD);
    file = diminuto_path_find(KEYWORD, NAME);
    alternative = diminuto_path_find(KEYWORD, ALTERNATIVE);
    CHECKPOINT("NAME=\"%s\" ALTERNATIVE=\"%s\" KEYWORD=\"%s\" paths=\"%s\" file=\"%s\" alternative=\"%s\"\n", NAME, ALTERNATIVE, KEYWORD, (paths != (const char *)0) ? paths : "", (file != (const char *)0) ? file : "", (alternative != (const char *)0) ? alternative : "");
    ASSERT((file != (char *)0) || (alternative != (char *)0));
    if (file == (char *)0) {
        file = alternative;
    }

    /*
     * Make sure the module is unloaded, possibly from a prior unit test.
     */

#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)

    if ((module = diminuto_module_handle(file)) != (diminuto_module_handle_t)0) {
        check = diminuto_module_unload(module, !0);
        ASSERT(check == (diminuto_module_handle_t)0);
    }

#endif

    /*
     * Test module reference counting.
     */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

        check = diminuto_module_load(file);
        ASSERT(check != (diminuto_module_handle_t)0);
        ASSERT(check == module);

        check = diminuto_module_unload(module, 0);
        ASSERT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_unload(module, 0);
    ASSERT(check == (diminuto_module_handle_t)0);

#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)

    check = diminuto_module_unload(module, 0);
    ADVISE(check != (diminuto_module_handle_t)0);
    ADVISE(check == module);

#endif

    /*
     * Test forced unload.
     */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

        check = diminuto_module_load(file);
        ASSERT(check != (diminuto_module_handle_t)0);
        ASSERT(check == module);

        check = diminuto_module_unload(module, !0);
        ASSERT(check == (diminuto_module_handle_t)0);

#if defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC) || defined(COM_DIAG_DIMINUTO_PLATFORM_CYGWIN)

    check = diminuto_module_unload(module, 0);
    ASSERT(check == (diminuto_module_handle_t)0);

#else

    check = diminuto_module_unload(module, 0);
    ASSERT(check != (diminuto_module_handle_t)0);
    ASSERT(check == module);

#endif

    /*
     * Test function and variable access.
     */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

    functionp = diminuto_module_symbol(module, "diminuto_module_example_function", (const char *)0);
    ASSERT(functionp != (void *)0);

    rc = (*(int (*)(int))functionp)(value); /* Produces -pedantic warning. */
    CHECKPOINT("0x%8.8x=(*%p)(0x%8.8x)\n", rc, functionp, value);
    ASSERT(rc == ~value);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable", (const char *)0);
    ASSERT(variablep != (void *)0);

    CHECKPOINT("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    ASSERT(*(int *)variablep == 0xc0edbabe);

    *(int *)variablep = 0xdeadbeef;
    CHECKPOINT("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    ASSERT(*(int *)variablep == 0xdeadbeef);

    functionp = diminuto_module_symbol(module, "diminuto_module_example_function_notfound", (const char *)0);
    ASSERT(functionp == (void *)0);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable_notfound", (const char *)0);
    ASSERT(variablep == (void *)0);

    check = diminuto_module_unload(module, 0);
    ASSERT(check == (diminuto_module_handle_t)0);

   /*
    * Test reinitialization of static variables upon reload.
    */

    module = diminuto_module_load(file);
    ASSERT(module != (diminuto_module_handle_t)0);

    variablep = diminuto_module_symbol(module, "diminuto_module_example_variable", (const char *)0);
    CHECKPOINT("*%p=0x%8.8x\n", variablep, *(int *)variablep);
    ASSERT(*(int *)variablep == 0xc0edbabe);

    check = diminuto_module_unload(module, 0);
    ASSERT(check == (diminuto_module_handle_t)0);

    /*
     * Test non-existent and other error legs.
     */

    check = diminuto_module_handle("COM-DIAG-DIMINUTO-MODULE-NO-SUCH-FILE-OR-DIRECTORY");
    ASSERT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_load("COM-DIAG-DIMINUTO-MODULE-NO-SUCH-FILE-OR-DIRECTORY");
    ASSERT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_handle("/dev/null");
    ASSERT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_load("/dev/null");
    ASSERT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_handle("/dev/zero");
    ASSERT(check == (diminuto_module_handle_t)0);

    check = diminuto_module_load("/dev/zero");
    ASSERT(check == (diminuto_module_handle_t)0);

    /*
     * Free dynamically allocated file name.
     */

    free(file);

    EXIT();
}
