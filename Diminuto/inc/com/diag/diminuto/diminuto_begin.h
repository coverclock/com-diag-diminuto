/* vi: set ts=4 expandtab shiftwidth=4: */

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief When included starts a C linkage section.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This header file can included from both C++ and C translation units.
 * If included in a C++ program, begins a C linkage section.
 * If included in a C program, does nothing.
 * This file may be included more than once.
 *
 * REFERENCES
 * 
 *  M. Cline et al., <I>C++ FAQs</I>, 2nd edition,
 *  Addision-Wessley, 1999, pp. 538, "FAQ 36.05 How
 *  can an object of a C++ class be passed to or from
 *  a C function?"
 *
 *  M. Cline, "C++ FAQ Lite", 2001-08-15, 29.8
 */

#if defined(__cplusplus)
extern "C" {
#endif
