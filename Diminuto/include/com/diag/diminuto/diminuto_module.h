/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MODULE_
#define _H_COM_DIAG_DIMINUTO_MODULE_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * This is the type of variable returned when a dynamically linked object,
 * also known as a shared object (so) or a dynamic link library (DLL), is
 * loaded. Diminuto call these kinds of things modules, mostly because that's
 * what Asterisk calls them, and that is from whence I got the idea.
 */
typedef void * diminuto_module_handle_t;

/**
 * Check to see if a module is already loaded.
 * @param filename is the path to the module.
 * @return a handle if the module is loaded, NULL otherwise.
 */
extern diminuto_module_handle_t diminuto_module_handle(const char * filename);

/**
 * Load a module.
 * @param filename is the path to the module.
 * @return a handle to the module if it was successfully loaded, NULL otherwise.
 */
extern diminuto_module_handle_t diminuto_module_load(const char * filename);

/**
 * Resolve an external symbol inside a loaded module.
 * @param handle identifies a specific loaded module.
 * @param symbol is the (possibly mangled) name of the symbol.
 * @param version is an optional version string or NULL if version doesn't matter.
 * @return a pointer to the symbol inside the loaded module of successful or NULL otherwise.
 */
extern void * diminuto_module_symbol(diminuto_module_handle_t handle, const char * symbol, const char * version);

/**
 * Unload a loaded module.
 * @param handle identifies a specific loaded module.
 * @return NULL is the unload was successful, the value of handle otherwise.
 */
extern diminuto_module_handle_t diminuto_module_unload(diminuto_module_handle_t handle);

/**
 * @def COM_DIAG_DIMINUTO_MODULE_CTOR
 * When used inside the translation unit for a loadable module, generates the
 * appropriate declaration for a function that will be automatically called
 * by the run-time library after that loadable module is loaded. @a __NAME__
 * is the name of the constructor function, which must be implemented inside th
 * loadable module.
 */
#define COM_DIAG_DIMINUTO_MODULE_CTOR(__NAME__) int __NAME__ (void) __attribute__((constructor))

/**
 * @def COM_DIAG_DIMINUTO_MODULE_DTOR
 * When used inside the translation unit for a loadable module, generates the
 * appropriate declaration for a function that will be automatically called
 * by the run-time library before that loadable module is unloaded. @a __NAME__
 * is the name of the destructor function, which must be implemented inside the
 * loadable module.
 */
#define COM_DIAG_DIMINUTO_MODULE_DTOR(__NAME__) int __NAME__ (void) __attribute__((destructor))


#endif
