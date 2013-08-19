/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_WELL_
#define _H_COM_DIAG_DIMINUTO_WELL_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * A well is like a Diminuto pool except the linked lists and allocated user
 * objects are kept in separately allocated sections of memory. Both sections
 * are page aligned, and objects in the well can have any specified alignment
 * in memory instead of the usual eight-byte alignment. Also, a well is of
 * fixed size; additional objects are never allocated after initialization.
 *
 * This header file can be included in both C and C++ translation units. If
 * included in C++, it will include additional C++ specific source code to
 * support the integration and use of wells in C++ applications.
 */

#if defined(__cplusplus)
extern "C" {
#endif

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_list.h"

/**
 * This is the type of a well. It is in fact an array of lists, the first
 * containing nodes pointing to free objects, the second containing unused
 * nodes, and the remainder the nodes themselves.
 */
typedef diminuto_list_t diminuto_well_t;

/**
 * These are handy indices that expose the composition of the list array.
 * Note that the data field of nodes on the USED list have no meaning.
 */
enum DiminutoWellIndex {
	DIMINUTO_WELL_FREE = 0,
	DIMINUTO_WELL_USED = 1,
	DIMINUTO_WELL_NODE = 2,
};

/**
 * This feature likes to know the virtual page size and the cache line size
 * in bytes. If it can't figure it out from looking the system, these are the
 * values it uses.
 */
enum DiminutoWellDefault {
	DIMINUTO_WELL_PAGESIZE = 4096, /* Or specify via -DDIMINUTO_PAGESIZE */
	DIMINUTO_WELL_LINESIZE = 64,   /* Or specify via -DDIMINUTO_LINESIZE */
};

/**
 * Return the size of virtual memory pages of the underlying platform.
 * @return the size of virtual memory pages in bytes or <0 if error.
 */
extern size_t diminuto_well_pagesize(void);

/**
 * Return the size of level 1 cache lines of the underlying platform.
 * @return the size of cache lines in bytes or <0 of error.
 */
extern size_t diminuto_well_linesize(void);

/**
 * Compute the smallest power of two that is greater than or equal to the
 * specified alignment.
 * @param alignment is any arbitrary value greater than or equal to zero.
 * @return a power of two (for example: 1, 2, 4, 8, etc.).
 */
extern size_t diminuto_well_power(size_t alignment);

/**
 * Return true if the specified alignment is a power of two (for example: 1, 2,
 * 4, 8, etc.), false otherwise.
 * @param alignment is any arbitrary value greater than or equal to zero.
 * @return true if alignment is a power of two, false otherwise.
 */
extern int diminuto_well_is_power(size_t alignment);

/**
 * Compute the effective size of an object given a specified alignment.
 * @param size is the desired size of an object..
 * @param alignment is the alignment of each object in the well; it MUST be
 * greater than zero AND a power of two (including one).
  */
extern size_t diminuto_well_alignment(size_t size, size_t alignment);

/**
 * Allocate a fixed-size well for objects of the specified size. The well is
 * initially full and never grows. Each object in the well is memory aligned on
 * the specified alignment. The alignment should be at least that required for
 * the first field in the object. Most memory allocators (including Diminuto
 * pool) align on (at least) an eight-byte boundary. However, when smaller
 * alignments can be used, they can lead to fitting more data in a page and in
 * a cache line. So field packing or careful ordering of fields in an object
 * can lead to more efficient memory use (but possible less efficient run-time).
 * Another approach is to align objects on a cache line, which may waste space
 * but avoids word tearing. As usual, life is a series of trade-offs. A zero
 * alignment can be used to specify the cache line size returned by
 * diminuto_well_cacheline(); values greater than zero will be adjusted to the
 * smallest power of two greater than or equal to the specified value.
 * @param size is the desired size of objects to be kept in the well.
 * @param count is the desired number of objects in the well.
 * @param alignment is the alignment of each object in the well.
 * @return a pointer to the well.
 */
extern diminuto_well_t * diminuto_well_init(size_t size, size_t count, size_t alignment);

/**
 * Release all of memory associated with the well. This includes not just
 * the objects in the well, but the well itself.
 * @param wellp points to a well.
 */
extern void diminuto_well_fini(diminuto_well_t * wellp);

/**
 * Allocate an object from the well. If an object is in the well, it
 * is removed from the well and a pointer to it is returned. If the well
 * is empty, a null pointer is returned.
 * @param wellp points to the well.
 * @return a pointer to an object from the well or NULL for failure.
 */
extern void * diminuto_well_alloc(diminuto_well_t * wellp);

/**
 * Free an object back to the well. The object is returned to the well.
 * @param wellp points to the well.
 * @return 0 for success or <0 for failure.
 */
extern int diminuto_well_free(diminuto_well_t * wellp, void * pointer);

/**
 * Return true if the well is full.
 * @param wellp points to the well.
 * @return !0 if the well is full, 0 otherwise.
 */
static inline int diminuto_well_isfull(const diminuto_well_t * wellp) {
	return diminuto_list_isempty(&wellp[DIMINUTO_WELL_USED]);
}

/**
 * Return true if the well is empty.
 * @param wellp points to the well.
 * @return !0 if the well is empty, 0 otherwise.
 */
static inline int diminuto_well_isempty(const diminuto_well_t * wellp) {
	return diminuto_list_isempty(&wellp[DIMINUTO_WELL_FREE]);
}

#if defined(__cplusplus)
}

#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>

namespace com {
 namespace diag {
  namespace diminuto {

	template <class _TYPE_, size_t _ALIGNMENT_ = sizeof(uint64_t)>
	/**
	 * This is a templated Well class for objects of type _TYPE_. When a new()
	 * operator is called for an object of type _TYPE_, the memory for the
	 * object is allocated from the well. However, the object is not pre-
	 * constructed. Instead, C++ calls the _TYPE_ constructor at allocation
	 * time. The well is allocated lazily when the first object of type _TYPE_
	 * is allocated. This prevents memory from being allocation for objects
	 * that are never used.
	 */
	class Well {

	public:

		/**
		 * Constructor.
		 * [1] If cardinality is zero, memory for the well must be explicitly
		 * allocated by the application by calling the
		 * init(cardinality, alignment) method with appropriate values sometime
		 * after construction and prior to the first attempt at allocating an
		 * object of type _TYPE_ from the well.
		 * [2] If cardinality is greater than zero but preallocation is not
		 * enabled, memory for the well will be allocated lazily on the first
		 * attempt at calling the _TYPE_::new() operator. Hence if the well is
		 * never used, memory is never allocated for it. However, that first
		 * call to _TYPE_::new() is going to be more expensive than subsequent
		 * calls.
		 * [3] If cardinality is greater than zero and preallocation is enabled,
		 * memory for the well is allocated at contruction.
		 * @param cc is the fixed number of items of type _TYPE_ in the well.
		 * @param pp if true causes memory to be allocated during construction.
		 */
		explicit Well(size_t cc = 0, bool pp = true, size_t aa = _ALIGNMENT_)
		: cardinality(cc)
		, alignment(aa)
		, wellp(static_cast<diminuto_well_t *>(0))
		{
			pthread_mutex_init(&mutex, (pthread_mutexattr_t *)0);
			if (cc == 0) {
				/* Do nothing. */
			} else if (!pp) {
				/* Do nothing. */
			} else {
				init();
			}
		}

		/**
		 * Destructor.
		 * If the well was lazily allocated, it is freed at this time. Any
		 * objects of type _TYPE_ allocated from the well will have their
		 * memory deallocated as well.
		 */
		virtual ~Well() {
			pthread_mutex_lock(&mutex);
				fini();
			pthread_mutex_unlock(&mutex);
			pthread_mutex_destroy(&mutex);
		}

		/**
		 * Allocate memory for the well.
		 * @param cc is the count of objects of type _TYPE_ in the well.
		 * @param aa is the alignment of objects of type _TYPE_ in the well.
		 */
		void init(size_t cc, size_t aa = _ALIGNMENT_) {
			if (wellp == static_cast<diminuto_well_t *>(0)) {
				wellp = diminuto_well_init(sizeof(_TYPE_), cc, aa);
			}
		}

		/**
		 * Allocate memory for the well. The cardinality and alignment of of
		 * objects of type _TYPE_ in the well were specified at construction.
		 */
		void init() {
			init(cardinality, alignment);
		}

		/**
		 * Deallocate memory for the well if and only iff there are no objects
		 * of type _TYPE_ allocated from the well.
		 */
		void fini() {
			if (wellp == static_cast<diminuto_well_t *>(0)) {
				/* Do nothing. */
			} else if (diminuto_well_isfull(wellp)) {
				diminuto_well_fini(wellp);
				wellp = (diminuto_well_t *)0;
			} else {
				/* Do nothing. */
			}
		}

		/**
		 * Allocate memory for an object of type _TYPE_ from the well.
		 * @return a pointer to the memory allocated from the well.
		 */
		_TYPE_ * alloc() {
			_TYPE_ * that;
			pthread_mutex_lock(&mutex);
				init();
				that = static_cast<_TYPE_ *>(diminuto_well_alloc(wellp));
			pthread_mutex_unlock(&mutex);
			return that;
		}

		/**
		 * Free memory for an object of type _TYPE_ back to the well.
		 * @param pointer points to the memory to be freed.
		 */
		void free(_TYPE_ * pointer) {
			pthread_mutex_lock(&mutex);
				diminuto_well_free(wellp, pointer);
			pthread_mutex_unlock(&mutex);
		}

		/**
		 * Return true is the well is full, that is, no objects of type _TYPE_
		 * are currently allocated from it.
		 * @return true if the well is full, false otherwise.
		 */
		inline bool isFull() const {
			return (diminuto_well_isfull(wellp) != 0);
		}

		/**
		 * Return true if the well is empty, that is, the next new operator
		 * will return a null pointer.
		 * @return true if the well is empty, false otherwise.
		 */
		inline bool isEmpty() const {
			return (diminuto_well_isempty(wellp) != 0);
		}

	protected:

		/**
		 * This is a POSIX thread mutex that makes operations on the well
		 * thread-safe.
		 */
		pthread_mutex_t mutex;

		/**
		 * This is the number of objects of type _TYPE_ whose memory will be
		 * allocated in the well.
		 */
		size_t cardinality;

		/**
		 * This is the alignment of the memory for each object of type _TYPE_ in
		 * the well. For example, if each object is eight-byte aligned, this
		 * value would be eight.
		 */
		size_t alignment;

		/**
		 * This is a pointer to the lazily allocated well for objects of type
		 * _TYPE_. Underneath the hood, this is really an array of Diminuto
		 * list structures.
		 */
		diminuto_well_t * wellp;

	};

  }
 }
}

/**
 * @def COM_DIAG_DIMINUTO_WELL_DECLARACTION
 * Intended to be used inside the class declaration for the class @a _TYPE_.
 * Declares a static well and the static new() and delete() operators used
 * to allocate and free objects of type _CLASS_. You can also just declare
 * these yourself if you choose.
 */
#define COM_DIAG_DIMINUTO_WELL_DECLARATION(_TYPE_) \
	static com::diag::diminuto::Well<_TYPE_> well; \
	static void * operator new(size_t size) { return well.alloc(); } \
	static void operator delete(void * pointer) { well.free(static_cast<_TYPE_ *>(pointer)); }

/**
 * @def COM_DIAG_DIMINUTO_WELL_DEFINITION
 * Intended to be used in the translation unit that defines the class @a _TYPE_
 * and it's well of @a _COUNT_ objects of type _TYPE_. This defines the objects
 * in the well to have the default alignment. But you can define the well
 * yourself if you want and specify any alignment you choose.
 */
#define COM_DIAG_DIMINUTO_WELL_DEFINITION(_TYPE_, _COUNT_) \
	com::diag::diminuto::Well<_TYPE_> _TYPE_::well(_COUNT_)

#endif

#endif
