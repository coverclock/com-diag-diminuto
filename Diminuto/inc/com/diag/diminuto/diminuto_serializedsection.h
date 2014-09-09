/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_
#define _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * This is largely experimental.
 */

#define DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) \
    do { \
        volatile int * _diminuto_spin_lock_intp_ = (_INTP_); \
        while (__sync_lock_test_and_set(_diminuto_spin_lock_intp_, 1)); \
        do { \
            do {} while (0)

#define DIMINUTO_SERIALIZED_SECTION_END \
        } while (0); \
        __sync_lock_release(_diminuto_spin_lock_intp_); \
    } while (0)

#endif
