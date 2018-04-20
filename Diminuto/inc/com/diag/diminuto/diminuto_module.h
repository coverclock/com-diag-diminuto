/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MODULE_
#define _H_COM_DIAG_DIMINUTO_MODULE_

/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Some of the underlying functionality used by this feature is not implemented
 * in uClibc, and implemented with different semantics in bionic (Android).
 */

/**
 * This is the type of variable returned when a dynamically linked object,
 * also known as a shared object (so) or a dynamic link library (DLL), is
 * loaded. Diminuto call these kinds of things modules, mostly because that's
 * what Asterisk calls them, and that is from whence I got the idea.
 */
typedef void * diminuto_module_handle_t;

/**
 * Check to see if a module is already loaded. N.B. the underlying functionality
 * is not implemented on all platforms. For example, in bionic used by Android,
 * this always appears to load the module.
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
 * Resolve an external symbol inside a loaded module. The underlying version
 * capability is not implemented on all platforms, in which case the version
 * parameter is ignored.
 * @param handle identifies a specific loaded module.
 * @param symbol is the (possibly mangled) name of the symbol.
 * @param version is an optional version string or NULL if version doesn't matter.
 * @return a pointer to the symbol inside the loaded module of successful or NULL otherwise.
 */
extern void * diminuto_module_symbol(diminuto_module_handle_t handle, const char * symbol, const char * version);

/**
 * Unload a loaded module. The module is not actually unloaded unless the
 * number of unloads matches the number of loads, reducing the reference count
 * to zero. The only time this function appears to return an error is if the
 * module is already actually unloaded. This implementation was inspired by
 * the loadable module manager in Asterisk. N.B. The bionic library used by
 * Android appears to have different semantics: the unload always succeeds, even
 * if the module was not loaded. This makes it impossible to use the force
 * option, and indeed, to even tell if the module was actually ever unloaded.
 * @param handle identifies a specific loaded module.
 * @param force if true iterates on the unload until the reference count is zero.
 * @return NULL is the unload was successful, the value of handle otherwise.
 */
extern diminuto_module_handle_t diminuto_module_unload(diminuto_module_handle_t handle, int force);

/**
 * @def DIMINUTO_MODULE_CTOR
 * When used inside the translation unit for a loadable module, generates the
 * appropriate declaration for a function that will be automatically called
 * by the run-time library after that loadable module is loaded. @a __NAME__
 * is the name of the constructor function, which must be implemented inside th
 * loadable module.
 */
#define DIMINUTO_MODULE_CTOR(__NAME__) int __NAME__ (void) __attribute__((constructor))

/**
 * @def DIMINUTO_MODULE_DTOR
 * When used inside the translation unit for a loadable module, generates the
 * appropriate declaration for a function that will be automatically called
 * by the run-time library before that loadable module is unloaded. @a __NAME__
 * is the name of the destructor function, which must be implemented inside the
 * loadable module.
 */
#define DIMINUTO_MODULE_DTOR(__NAME__) int __NAME__ (void) __attribute__((destructor))


#endif
