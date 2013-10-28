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

typedef void * diminuto_module_handle_t;

extern diminuto_module_handle_t diminuto_module_handle(const char * filename);

extern diminuto_module_handle_t diminuto_module_load(const char * filename);

extern void * diminuto_module_symbol(diminuto_module_handle_t handle, const char * symbol, const char * version);

extern diminuto_module_handle_t diminuto_module_unload(diminuto_module_handle_t handle);

#endif
