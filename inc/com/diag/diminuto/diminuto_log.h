/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOG_
#define _H_COM_DIAG_DIMINUTO_LOG_

/**
 * @file
 *
 * Copyright 2009-2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This header file can legitimately be included more than once in a
 * single translation unit. It can be used in applications, daemons,
 * the kernel modules, and device drivers. In applications, it prints
 * to standard error. In daemons (processes who have been inherited
 * by process ID 1 as their parent), it logs to the system log. For
 * kernel modules and device drivers, it uses the printk kernel function.
 *
 * Define the preprocessor symbol DIMINUTO_LOG_x_DISABLE to disable
 * the generation of the appropriate log statements, where x is replaced
 * with EMERGENCY, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFORMATION,
 * or DEBUG. By default all are enabled.
 *
 * Define the preprocessor symbol DIMINUTO_LOG_MASK to be the name of
 * a variable of type diminuto_log_mask_t to use a subsystem-specific
 * log mask other than the default of diminuto_log_mask. This could,
 * for example, be the name of a specific element in an array of
 * masks to allow an arbitrary number of subsystems, the logging for
 * each of which can be managed independently.
 *
 * The long preprocessor macro and symbol names were designed to make
 * conflicts with other code unlikely. Feel free to define your own
 * shorter names from these, for example define DBG(_FORMAT_, ...) to
 * be DIMINUTO_LOG_DEBUG(_FORMAT_, __VA_ARGS__).
 */

/******************************************************************************/

/*
 * The log mask has one bit for every message priority that can be enabled or
 * disabled. We define here a bit for every priority that is supported by
 * the native Linux syslog (for daemons) and printk (for kernel and device
 * drivers) interfaces. You might be tempted to define additional mask bits,
 * but the native interfaces won't understand other priorities. We define
 * the mask to be an int, even though only the lowest order eight bits are
 * used, because that makes it easier to use as a parameter for loadable
 * kernel or device driver modules.
 */

typedef int diminuto_log_mask_t;

#define DIMINUTO_LOG_MASK_EMERGENCY     (1<<0)
#define DIMINUTO_LOG_MASK_ALERT         (1<<1)
#define DIMINUTO_LOG_MASK_CRITICAL      (1<<2)
#define DIMINUTO_LOG_MASK_ERROR         (1<<3)
#define DIMINUTO_LOG_MASK_WARNING       (1<<4)
#define DIMINUTO_LOG_MASK_NOTICE        (1<<5)
#define DIMINUTO_LOG_MASK_INFORMATION   (1<<6)
#define DIMINUTO_LOG_MASK_DEBUG         (1<<7)

#define DIMINUTO_LOG_MASK_ALL           (DIMINUTO_LOG_MASK_EMERGENCY | DIMINUTO_LOG_MASK_ALERT | DIMINUTO_LOG_MASK_CRITICAL | DIMINUTO_LOG_MASK_ERROR | DIMINUTO_LOG_MASK_WARNING | DIMINUTO_LOG_MASK_NOTICE | DIMINUTO_LOG_MASK_INFORMATION | DIMINUTO_LOG_MASK_DEBUG)

#define DIMINUTO_LOG_MASK_NONE          (0)

#define DIMINUTO_LOG_MASK_DEFAULT       (DIMINUTO_LOG_MASK_EMERGENCY | DIMINUTO_LOG_MASK_ALERT | DIMINUTO_LOG_MASK_CRITICAL | DIMINUTO_LOG_MASK_ERROR | DIMINUTO_LOG_MASK_WARNING | DIMINUTO_LOG_MASK_NOTICE)

/******************************************************************************/

#if defined(__KERNEL__) || defined(MODULE)

/*
 * Logging can be done from the kernel or a device driver, in which case
 * it uses the <code>printk()</code> function and the kernel priorities.
 * Each translation unit has its own local subsystem log mask which is
 * implicity defined and declared.
 */

#include <linux/kernel.h>

#define DIMINUTO_LOG_PRIORITY_EMERGENCY      KERN_EMERG
#define DIMINUTO_LOG_PRIORITY_ALERT          KERN_ALERT
#define DIMINUTO_LOG_PRIORITY_CRITICAL       KERN_CRIT
#define DIMINUTO_LOG_PRIORITY_ERROR          KERN_ERR
#define DIMINUTO_LOG_PRIORITY_WARNING        KERN_WARNING
#define DIMINUTO_LOG_PRIORITY_NOTICE         KERN_NOTICE
#define DIMINUTO_LOG_PRIORITY_INFORMATION    KERN_INFO
#define DIMINUTO_LOG_PRIORITY_DEBUG          KERN_DEBUG

static diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

#else

/*
 * Logging can be done from an application or a daemon, in the former
 * case it prints to standard error, and in the latter it logs to the
 * system log. Each translation unit either defines its own subsystem
 * log mask (which may be local or external) or uses the global default
 * subsystem log mask.
 */

#include <syslog.h>

#define DIMINUTO_LOG_PRIORITY_EMERGENCY      LOG_EMERG
#define DIMINUTO_LOG_PRIORITY_ALERT          LOG_ALERT
#define DIMINUTO_LOG_PRIORITY_CRITICAL       LOG_CRIT
#define DIMINUTO_LOG_PRIORITY_ERROR          LOG_ERR
#define DIMINUTO_LOG_PRIORITY_WARNING        LOG_WARNING
#define DIMINUTO_LOG_PRIORITY_NOTICE         LOG_NOTICE
#define DIMINUTO_LOG_PRIORITY_INFORMATION    LOG_INFO
#define DIMINUTO_LOG_PRIORITY_DEBUG          LOG_DEBUG

#define DIMINUTO_LOG_IDENT_DEFAULT           "Diminuto"
#define DIMINUTO_LOG_OPTION_DEFAULT          LOG_CONS
#define DIMINUTO_LOG_FACILITY_DEFAULT        LOG_LOCAL0

extern char * diminuto_log_ident;
extern int diminuto_log_option;
extern int diminuto_log_facility;
extern diminuto_log_mask_t diminuto_log_mask;

#include <stdarg.h>

/**
 * Format and log the argument list to syslog.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 * @param ap is a variable length argument list.
 */
extern void diminuto_log_vsyslog(int priority, const char * format, va_list ap);

/**
 * Format and log the argument list to syslog.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_syslog(int priority, const char * format, ...);

/**
 * Emulate perror() but instead of writing the prefixed error string to
 * standard error, log it to the system log.
 * @param s points to a nul-terminated string prepended to the error string.
 * @see perror(3).
 */
extern void diminuto_serror(const char * s);

/**
 * If the parent PID of the calling process is not 1, emulate the stdio
 * function perror(); if it is, still emulate perror() but instead of writing
 * the prefixed error string to standard error, log it to the system log.
 * @param s points to a nul-terminated string prepended to the error string.
 * @see perror(3).
 */
extern void diminuto_perror(const char * s);

/**
 * If the parent PID of the calling process is not 1, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog. This behavior is useful when unit testing daemons.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 * @param ap is a variable length argument list.
 */
extern void diminuto_log_vlog(int priority, const char * format, va_list ap);

/**
 * If the parent PID of the calling process is not 1, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog. This behavior is useful when unit testing daemons.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_log(int priority, const char * format, ...);

/**
 * If the parent PID of the calling process is not 1, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog. The message is logged at the default priority.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_emit(const char * format, ...);

#endif

/******************************************************************************/

#define DIMINUTO_LOG_PRIORITY_DEFAULT       DIMINUTO_LOG_PRIORITY_NOTICE

#define DIMINUTO_LOG_STRINGIFY(_STRING_)    #_STRING_
#define DIMINUTO_LOG_TOKENIZE(_TOKEN_)      DIMINUTO_LOG_STRINGIFY(_TOKEN_)
#define DIMINUTO_LOG_HERE                   __FILE__ "@" DIMINUTO_LOG_TOKENIZE(__LINE__) ": "

#endif

/******************************************************************************/

#if !defined(DIMINUTO_LOG_MASK)
#	define DIMINUTO_LOG_MASK diminuto_log_mask
#endif

#if defined(DIMINUTO_LOG_ENABLED)
#	undef DIMINUTO_LOG_ENABLED
#endif

#define DIMINUTO_LOG_ENABLED(_MASK_) (((DIMINUTO_LOG_MASK) & (_MASK_)) != 0)

/******************************************************************************/

#if defined(DIMINUTO_LOG)
#	undef DIMINUTO_LOG
#endif

#if defined(DIMINUTO_LOG_IF)
#	undef DIMINUTO_LOG_IF
#endif

#if defined(DIMINUTO_LOG_DISABLE)

#	define DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ...) ((void)0)
#	define DIMINUTO_LOG_IF(_MASK_, _PRIORITY_, _FORMAT_, ...) ((void)0)

#elif defined(__KERNEL__) || defined(MODULE)

#	define DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ...) (void)printk(_PRIORITY_ _FORMAT_, ## __VA_ARGS__)
#	define DIMINUTO_LOG_IF(_MASK_, _PRIORITY_, _FORMAT_, ...) (DIMINUTO_LOG_ENABLED(_MASK_) ? (void)printk(_PRIORITY_ _FORMAT_, ## __VA_ARGS__) : ((void)0))

#else

#	define DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ...) diminuto_log_log(_PRIORITY_, _FORMAT_, ## __VA_ARGS__)
#	define DIMINUTO_LOG_IF(_MASK_, _PRIORITY_, _FORMAT_, ...) (DIMINUTO_LOG_ENABLED(_MASK_) ? diminuto_log_log(_PRIORITY_, _FORMAT_, ## __VA_ARGS__) : ((void)0))

#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_EMERGENCY)
#	undef DIMINUTO_LOG_EMERGENCY
#endif

#if defined(DIMINUTO_LOG_EMERGENCY_DISABLE)
#	define DIMINUTO_LOG_EMERGENCY(_FORMAT_, ...) ((void)0)
#else 
#	define DIMINUTO_LOG_EMERGENCY(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_EMERGENCY, DIMINUTO_LOG_PRIORITY_EMERGENCY, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_ALERT)
#	undef DIMINUTO_LOG_ALERT
#endif

#if defined(DIMINUTO_LOG_ALERT_DISABLE)
#	define DIMINUTO_LOG_ALERT(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_ALERT(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_ALERT, DIMINUTO_LOG_PRIORITY_ALERT, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_CRITICAL)
#	undef DIMINUTO_LOG_CRITICAL
#endif

#if defined(DIMINUTO_LOG_CRITICAL_DISABLE)
#	define DIMINUTO_LOG_CRITICAL(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_CRITICAL(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_CRITICAL, DIMINUTO_LOG_PRIORITY_CRITICAL, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_ERROR)
#	undef DIMINUTO_LOG_ERROR
#endif

#if defined(DIMINUTO_LOG_ERROR_DISABLE)
#	define DIMINUTO_LOG_ERROR(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_ERROR(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_ERROR, DIMINUTO_LOG_PRIORITY_ERROR, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_WARNING)
#	undef DIMINUTO_LOG_WARNING
#endif

#if defined(DIMINUTO_LOG_WARNING_DISABLE)
#	define DIMINUTO_LOG_WARNING(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_WARNING(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_WARNING, DIMINUTO_LOG_PRIORITY_WARNING, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_NOTICE)
#	undef DIMINUTO_LOG_NOTICE
#endif

#if defined(DIMINUTO_LOG_NOTICE_DISABLE)
#	define DIMINUTO_LOG_NOTICE(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_NOTICE(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_NOTICE, DIMINUTO_LOG_PRIORITY_NOTICE, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_INFORMATION)
#	undef DIMINUTO_LOG_INFORMATION
#endif

#if defined(DIMINUTO_LOG_INFORMATION_DISABLE)
#	define DIMINUTO_LOG_INFORMATION(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_INFORMATION(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_INFORMATION, DIMINUTO_LOG_PRIORITY_INFORMATION, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_DEBUG)
#	undef DIMINUTO_LOG_DEBUG
#endif

#if defined(DIMINUTO_LOG_DEBUG_DISABLE)
#	define DIMINUTO_LOG_DEBUG(_FORMAT_, ...) ((void)0)
#else
#	define DIMINUTO_LOG_DEBUG(_FORMAT_, ...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_DEBUG, DIMINUTO_LOG_PRIORITY_DEBUG, _FORMAT_, ## __VA_ARGS__)
#endif

/******************************************************************************/
