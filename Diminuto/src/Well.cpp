/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_well.h"

namespace com {
 namespace diag {
  namespace diminuto {

/*******************************************************************************
 * BASE WELL
 ******************************************************************************/

BaseWell::BaseWell(size_t ss, size_t cc, bool pp, size_t aa)
: size(ss)
, cardinality(cc)
, alignment(aa)
, wellp(static_cast<diminuto_well_t *>(0))
{
	if (cc == 0) {
		/* Do nothing. */
	} else if (!pp) {
		/* Do nothing. */
	} else {
		init(size, cardinality, alignment);
	}
}

BaseWell::~BaseWell() {
	fini();
}

void BaseWell::init(size_t ss, size_t cc, size_t aa) {
	if (wellp == static_cast<diminuto_well_t *>(0)) {
		wellp = diminuto_well_init(ss, cc, aa);
	}
}

void BaseWell::init() {
	init(size, cardinality, alignment);
}

void BaseWell::fini() {
	if (wellp == static_cast<diminuto_well_t *>(0)) {
		/* Do nothing. */
	} else if (diminuto_well_isfull(wellp)) {
		diminuto_well_fini(wellp);
		wellp = (diminuto_well_t *)0;
	} else {
		/* Do nothing. */
	}
}

void * BaseWell::alloc() {
	void * that;
	init();
	that = diminuto_well_alloc(wellp);
	return that;
}

void BaseWell::free(void * pointer) {
	diminuto_well_free(wellp, pointer);
}

bool BaseWell::isFull() const {
	return (diminuto_well_isfull(wellp) != 0);
}

bool BaseWell::isEmpty() const {
	return (diminuto_well_isempty(wellp) != 0);
}

/*******************************************************************************
 * SAFE BASE WELL
 ******************************************************************************/

SafeBaseWell::SafeBaseWell(size_t ss, size_t cc, bool pp, size_t aa)
: BaseWell(ss, cc, pp, aa)
{
	pthread_mutex_init(&mutex, (pthread_mutexattr_t *)0);
}

SafeBaseWell::~SafeBaseWell() {
	pthread_mutex_destroy(&mutex);
}

void SafeBaseWell::init() {
	pthread_mutex_lock(&mutex);
		BaseWell::init();
	pthread_mutex_unlock(&mutex);
}

void SafeBaseWell::init(size_t ss, size_t cc, size_t aa) {
	pthread_mutex_lock(&mutex);
		BaseWell::init(ss, cc, aa);
	pthread_mutex_unlock(&mutex);
}

void SafeBaseWell::fini() {
	pthread_mutex_lock(&mutex);
		BaseWell::fini();
	pthread_mutex_unlock(&mutex);
}

void * SafeBaseWell::alloc() {
	void * that;
	pthread_mutex_lock(&mutex);
		that = BaseWell::alloc();
	pthread_mutex_unlock(&mutex);
	return that;
}

void SafeBaseWell::free(void * pointer) {
	pthread_mutex_lock(&mutex);
		BaseWell::free(pointer);
	pthread_mutex_unlock(&mutex);
}

  }
 }
}
