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
#include "com/diag/diminuto/diminuto_log.h"
#define __USE_GNU
#include <dlfcn.h>

/*
 * This is deliberately global but not in the header file. So it is not part
 * of the public API, but could be called from an application with the
 * addition of a suitable external declaration.
 */
diminuto_module_handle_t diminuto_module_load_generic(const char * filename, int flags)
{
	diminuto_module_handle_t handle;

	if ((handle = dlopen(filename, flags)) != (diminuto_module_handle_t)0) {
		/* Do nothing: successful. */
	} else if ((flags & RTLD_NOLOAD) == RTLD_NOLOAD) {
		/* Do nothing: nominal. */
	} else {
		diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "diminuto_module_load_generic: dlopen: %s\n", dlerror());
	}

	return handle;
}

diminuto_module_handle_t diminuto_module_handle(const char * filename)
{
	return diminuto_module_load_generic(filename, RTLD_NOLOAD);
}

diminuto_module_handle_t diminuto_module_load(const char * filename)
{
	return diminuto_module_load_generic(filename, RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND);
}

void * diminuto_module_symbol(diminuto_module_handle_t handle, const char * symbol, const char * version)
{
	void * resolved;
	const char * label;

	if (version == (const char *)0) {
		label = "dlsym";
		resolved = dlsym(handle, symbol);
	} else {
		label = "dlvsym";
		resolved = dlvsym(handle, symbol, version);
	}
	if (resolved == (void *)0) {
		diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, "diminuto_module_symbol: %s: %s\n", label, dlerror());
	}

	return resolved;
}


diminuto_module_handle_t diminuto_module_unload(diminuto_module_handle_t handle, int force)
{
	int rc;

	/*
	 * Forcing a dlclose() causes this code to iterate to reduce the reference
	 * count until the dlclose() returns an error, indicating that it failed
	 * because the module wasn't open. I got this idea from the Asterisk
	 * module code. It is kind of counter-intuitive.
	 */

	do {
		rc = dlclose(handle);
	} while (force && (rc == 0));

	if (force) {
		handle = (diminuto_module_handle_t)0;
	} else if (rc == 0) {
		handle = (diminuto_module_handle_t)0;
	} else {
		diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, "diminuto_module_unload: dlclose: %s\n", dlerror());
	}

	return handle;
}
