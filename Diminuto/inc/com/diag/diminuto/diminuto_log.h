/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOG_
#define _H_COM_DIAG_DIMINUTO_LOG_

/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This header file can legitimately be included more than once in a
 * single translation unit. It can be used in applications, daemons,
 * kernel modules, and device drivers. In applications, it prints
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
 * shorter names from these, for example define DBG(...) to
 * be DIMINUTO_LOG_DEBUG(__VA_ARGS__).
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_platform.h"
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)
#   include <unistd.h>
#endif
#include <stdbool.h>

/******************************************************************************/

/**
 * The log mask has one bit for every message priority that can be enabled or
 * disabled. We define here a bit for every priority that is supported by
 * the native Linux syslog (for daemons) and printk (for kernel and device
 * drivers) interfaces. You might be tempted to define additional mask bits,
 * but the native interfaces won't understand other priorities. We define
 * the mask to be an int, even though only the lowest order eight bits are
 * used, because that makes it easier to use as a parameter for loadable
 * kernel or device driver modules. (Making it anything else except an int
 * will result in a compile-time error unless code elsewhere is changed.)
 */
typedef int diminuto_log_mask_t;

#define DIMINUTO_LOG_MASK_EMERGENCY     (1 << (7 - 0))
#define DIMINUTO_LOG_MASK_ALERT         (1 << (7 - 1))
#define DIMINUTO_LOG_MASK_CRITICAL      (1 << (7 - 2))
#define DIMINUTO_LOG_MASK_ERROR         (1 << (7 - 3))
#define DIMINUTO_LOG_MASK_WARNING       (1 << (7 - 4))
#define DIMINUTO_LOG_MASK_NOTICE        (1 << (7 - 5))
#define DIMINUTO_LOG_MASK_INFORMATION   (1 << (7 - 6))
#define DIMINUTO_LOG_MASK_DEBUG         (1 << (7 - 7))

#define DIMINUTO_LOG_MASK_ALL           (DIMINUTO_LOG_MASK_EMERGENCY | DIMINUTO_LOG_MASK_ALERT | DIMINUTO_LOG_MASK_CRITICAL | DIMINUTO_LOG_MASK_ERROR | DIMINUTO_LOG_MASK_WARNING | DIMINUTO_LOG_MASK_NOTICE | DIMINUTO_LOG_MASK_INFORMATION | DIMINUTO_LOG_MASK_DEBUG)
#define DIMINUTO_LOG_MASK_NONE          (0)
#define DIMINUTO_LOG_MASK_DEFAULT       (DIMINUTO_LOG_MASK_EMERGENCY | DIMINUTO_LOG_MASK_ALERT | DIMINUTO_LOG_MASK_CRITICAL | DIMINUTO_LOG_MASK_ERROR | DIMINUTO_LOG_MASK_WARNING | DIMINUTO_LOG_MASK_NOTICE)

/******************************************************************************/

#if defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)

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

#   if defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)

/*
 * Logging can be done from an application or a daemon, in the former
 * case it prints to standard error, and in the latter it logs to the
 * Android log. Each translation unit either defines its own subsystem
 * log mask (which may be local or external) or uses the global default
 * subsystem log mask.
 */

#include <android/log.h>

#define DIMINUTO_LOG_PRIORITY_EMERGENCY      ANDROID_LOG_FATAL
#define DIMINUTO_LOG_PRIORITY_ALERT          ANDROID_LOG_FATAL
#define DIMINUTO_LOG_PRIORITY_CRITICAL       ANDROID_LOG_FATAL
#define DIMINUTO_LOG_PRIORITY_ERROR          ANDROID_LOG_ERROR
#define DIMINUTO_LOG_PRIORITY_WARNING        ANDROID_LOG_WARN
#define DIMINUTO_LOG_PRIORITY_NOTICE         ANDROID_LOG_INFO
#define DIMINUTO_LOG_PRIORITY_INFORMATION    ANDROID_LOG_DEBUG
#define DIMINUTO_LOG_PRIORITY_DEBUG          ANDROID_LOG_VERBOSE

#define DIMINUTO_LOG_IDENT_DEFAULT           "Diminuto"
#define DIMINUTO_LOG_OPTION_DEFAULT          0
#define DIMINUTO_LOG_FACILITY_DEFAULT        0

#   else

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
#define DIMINUTO_LOG_OPTION_DEFAULT          (LOG_CONS | LOG_PID)
#define DIMINUTO_LOG_FACILITY_DEFAULT        LOG_LOCAL7

#   endif

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define DIMINUTO_LOG_DESCRIPTOR_DEFAULT      STDERR_FILENO
#define DIMINUTO_LOG_STREAM_DEFAULT          ((FILE *)0)
#define DIMINUTO_LOG_MASK_NAME_DEFAULT       "COM_DIAG_DIMINUTO_LOG_MASK"
#define DIMINUTO_LOG_MASK_VALUE_ALL          "~0"

#define DIMINUTO_LOG_BUFFER_MAXIMUM          (1024)

/******************************************************************************/

/**
 * This is the log identity string used when openlog(3) is called. If the
 * application changes this before the first log function is called, this is
 * the identity that will be used.
 */
extern const char * diminuto_log_ident;

/**
 * This is the log option bit mask used when openlog(3) is called. If the
 * application changes this before the first log function is called, these are
 * the options that will be used.
 */
extern int diminuto_log_option;

/**
 * This is the log facility identifier used when openlog(3) is called. If the
 * application changes this before the first log function is called, this is
 * the facility that will be used.
 */
extern int diminuto_log_facility;

/**
 * This is the file descriptor to which log messages will be written if the
 * caller is not running as a daemon.
 */
extern int diminuto_log_descriptor;

/**
 * This is the file stream object which corresponds to the file descriptor.
 * (Some platforms have underlying logging mechanisms that need this.)
 */
extern FILE * diminuto_log_file;

/**
 * This is the name of the environmental variable assumed to contain a number
 * that is the log mask.
 */
extern const char * diminuto_log_mask_name;

/**
 * This is the log mask: a one bit means the messages at that log level are
 * emitted. The DEBUG level is the least significant bit (even though it is
 * numerically the highest log level), with successively higher levels
 * proceeding to the greater significant bits.
 */
extern diminuto_log_mask_t diminuto_log_mask;

/**
 * If this variable is true (!0), log output is forced to syslog. Otherwise,
 * log output will be written to stderr if the process is interactive, or to
 * syslog if it is not. The default is false (0).
 */
extern bool diminuto_log_forced;

/**
 * Calling getpid() and getsid() every time we log something is not
 * without overhead. But once a process becomes daemonized, and hence
 * becomes a session leader, it's not likely to go back (in fact, I'm
 * not even sure how that might be done). So we cache the result of the
 * comparison between the process identifier and the session identifier,
 * and once they are the same, we don't check it again. Since this is
 * checking a process-level quality, the result of the comparison should
 * be the same for all threads. So if this variable is true (!0), log
 * output will go to syslog; otherwise it will be written to stderr.
 * This variable is initialized to false (0), which causes it to be recomputed
 * with every log statement until it becomes true.
 */
extern bool diminuto_log_cached;

/******************************************************************************/

/**
 * Returns true if the calling process is not the session leader (this suggests
 * it has a controlling terminal) and its parent is not INIT (this suggests
 * it is not a daemon).
 * @return !0 if interactive, 0 otherwise.
 */
static inline int diminuto_log_interactive()
{
    return ((getpid() != getsid(0)) && (getppid() != 1));
}

/**
 * Set the global default log mask from the environment. By default, the name
 * of the log mask environmental variable is "COM_DIAG_DIMINUTO_LOG_MASK" and
 * its value is a string number in decimal, octal, or hexadecimal format as
 * understood by strtoul(3), e.g. 255, 0x377, 0xff, 0xFF.
 */
extern void diminuto_log_setmask(void);

/**
 * Open the underlying system log communication channel (whatever that is)
 * if it is not already open, and (if possible) provide it with an identifying
 * name, along with options, and a facility.
 * @param name points to a identifying name string.
 * @param option are ORed syslog options, like (LOG_CONS | LOG_PID).
 * @param facility is the syslog facility, like LOG_LOCAL7.
 */
extern void diminuto_log_open_syslog(const char * name, int option, int facility);

/**
 * Open the underlying system log communication channel (whatever that is)
 * if it is not already open, and (if possible) provide it with an identifying
 * name. This just calls diminuto_log_open_syslog() with the default options and
 * facility.
 * @param name points to a identifying name string or null to use the default.
 */
extern void diminuto_log_open(const char * name);

/**
 * Close the underlying system log communication channel (whatever that is)
 * if it is not already closed.
 */
extern void diminuto_log_close(void);

/**
 * Return a pointer to a buffered FILE object corresponding to the file
 * descriptor to which log messages are written when they are not written to
 * the system log (as, when the caller is not a daemon). This is useful for
 * logging output of diminuto_log_dump() from non-daemon applications, not so
 * useful for use in daemons.
 * @return a FILE pointer.
 */
extern FILE * diminuto_log_stream(void);

/**
 * Format and log the argument list to syslog.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 * @param ap is a variable length argument list.
 */
extern void diminuto_log_vsyslog(int priority, const char * format, va_list ap);

/**
 * Format and log to syslog.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_syslog(int priority, const char * format, ...);

/**
 * Format and log the argument list to a file descriptor.
 * @param fd is the file descriptor.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 * @param ap is a variable length argument list.
 */
extern void diminuto_log_vwrite(int fd, int priority, const char * format, va_list ap);

/**
 * Format and print to a file descriptor.
 * the argument list to
 * @param fd is the file descriptor.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_write(int fd, int priority, const char * format, ...);

/**
 * If the calling process is interactive, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog. This behavior is useful when unit testing daemons.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 * @param ap is a variable length argument list.
 */
extern void diminuto_log_vlog(int priority, const char * format, va_list ap);

/**
 * If the calling process is interactive, format and print
 * to stderr; if it is, format and log to syslog. This behavior is useful when
 * unit testing daemons.
 * @param priority is the log priority level.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_log(int priority, const char * format, ...);

/**
 * If the calling process is interactive, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog. The message is logged at the default priority.
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_emit(const char * format, ...);

/**
 * Emulate perror() but instead of writing the prefixed error string to
 * standard error, log it to the system log.
 * @param s points to a nul-terminated string prepended to the error string.
 * @see perror(3).
 */
extern void diminuto_serror(const char * s);

/**
 * If the calling process is interactive, emulate the stdio
 * function perror(); if it is, still emulate perror() but instead of writing
 * the prefixed error string to standard error, log it to the system log.
 * @param s points to a nul-terminated string prepended to the error string.
 * @see perror(3).
 */
extern void diminuto_perror(const char * s);

#endif

/******************************************************************************/

#define DIMINUTO_LOG_PRIORITY_DEFAULT DIMINUTO_LOG_PRIORITY_NOTICE

#include "com/diag/diminuto/diminuto_token.h"

#define DIMINUTO_LOG_HERE __FILE__ "@" DIMINUTO_TOKEN_TOKEN(__LINE__) ": "

#endif

/******************************************************************************/

#if !defined(DIMINUTO_LOG_MASK)
#   define DIMINUTO_LOG_MASK diminuto_log_mask
#endif

#if defined(DIMINUTO_LOG_ENABLED)
#   undef DIMINUTO_LOG_ENABLED
#endif

#define DIMINUTO_LOG_ENABLED(_MASK_) (((DIMINUTO_LOG_MASK) & (_MASK_)) != 0)

/******************************************************************************/

#if defined(DIMINUTO_LOG)
#   undef DIMINUTO_LOG
#endif

#if defined(DIMINUTO_LOG_IF)
#   undef DIMINUTO_LOG_IF
#endif

#if defined(DIMINUTO_LOG_DISABLE)
#   define DIMINUTO_LOG(_PRIORITY_, ...) ((void)0)
#   define DIMINUTO_LOG_IF(_MASK_, _PRIORITY_, ...) ((void)0)
#elif defined(__KERNEL__) || defined(MODULE)
#   define DIMINUTO_LOG(_PRIORITY_, ...) (void)printk(_PRIORITY_ __VA_ARGS__)
#   define DIMINUTO_LOG_IF(_MASK_, _PRIORITY_, ...) (DIMINUTO_LOG_ENABLED(_MASK_) ? (void)printk(_PRIORITY_ __VA_ARGS__) : ((void)0))
#else
#   define DIMINUTO_LOG(_PRIORITY_, ...) diminuto_log_log(_PRIORITY_, __VA_ARGS__)
#   define DIMINUTO_LOG_IF(_MASK_, _PRIORITY_, ...) (DIMINUTO_LOG_ENABLED(_MASK_) ? diminuto_log_log(_PRIORITY_, __VA_ARGS__) : ((void)0))
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_EMERGENCY)
#   undef DIMINUTO_LOG_EMERGENCY
#endif

#if defined(DIMINUTO_LOG_EMERGENCY_DISABLE)
#   define DIMINUTO_LOG_EMERGENCY(...) ((void)0)
#else 
#   define DIMINUTO_LOG_EMERGENCY(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_EMERGENCY, DIMINUTO_LOG_PRIORITY_EMERGENCY, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_ALERT)
#   undef DIMINUTO_LOG_ALERT
#endif

#if defined(DIMINUTO_LOG_ALERT_DISABLE)
#   define DIMINUTO_LOG_ALERT(...) ((void)0)
#else
#   define DIMINUTO_LOG_ALERT(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_ALERT, DIMINUTO_LOG_PRIORITY_ALERT, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_CRITICAL)
#   undef DIMINUTO_LOG_CRITICAL
#endif

#if defined(DIMINUTO_LOG_CRITICAL_DISABLE)
#   define DIMINUTO_LOG_CRITICAL(...) ((void)0)
#else
#   define DIMINUTO_LOG_CRITICAL(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_CRITICAL, DIMINUTO_LOG_PRIORITY_CRITICAL, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_ERROR)
#   undef DIMINUTO_LOG_ERROR
#endif

#if defined(DIMINUTO_LOG_ERROR_DISABLE)
#   define DIMINUTO_LOG_ERROR(...) ((void)0)
#else
#   define DIMINUTO_LOG_ERROR(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_ERROR, DIMINUTO_LOG_PRIORITY_ERROR, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_WARNING)
#   undef DIMINUTO_LOG_WARNING
#endif

#if defined(DIMINUTO_LOG_WARNING_DISABLE)
#   define DIMINUTO_LOG_WARNING(...) ((void)0)
#else
#   define DIMINUTO_LOG_WARNING(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_WARNING, DIMINUTO_LOG_PRIORITY_WARNING, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_NOTICE)
#   undef DIMINUTO_LOG_NOTICE
#endif

#if defined(DIMINUTO_LOG_NOTICE_DISABLE)
#   define DIMINUTO_LOG_NOTICE(...) ((void)0)
#else
#   define DIMINUTO_LOG_NOTICE(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_NOTICE, DIMINUTO_LOG_PRIORITY_NOTICE, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_INFORMATION)
#   undef DIMINUTO_LOG_INFORMATION
#endif

#if defined(DIMINUTO_LOG_INFORMATION_DISABLE)
#   define DIMINUTO_LOG_INFORMATION(...) ((void)0)
#else
#   define DIMINUTO_LOG_INFORMATION(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_INFORMATION, DIMINUTO_LOG_PRIORITY_INFORMATION, __VA_ARGS__)
#endif

/******************************************************************************/

#if defined(DIMINUTO_LOG_DEBUG)
#   undef DIMINUTO_LOG_DEBUG
#endif

#if defined(DIMINUTO_LOG_DEBUG_DISABLE)
#   define DIMINUTO_LOG_DEBUG(...) ((void)0)
#else
#   define DIMINUTO_LOG_DEBUG(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_DEBUG, DIMINUTO_LOG_PRIORITY_DEBUG, __VA_ARGS__)
#endif

/******************************************************************************/
