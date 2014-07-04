/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BARRIER_
#define _H_COM_DIAG_DIMINUTO_BARRIER_

/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
#           define diminuto_barrier() __sync_synchronize()
#       endif
#   endif
#endif

#if !defined(diminuto_barrier)
#   define diminuto_barrier() ((void)0)
#   warning diminuto_barrier() is a no-op!
#endif

#endif
