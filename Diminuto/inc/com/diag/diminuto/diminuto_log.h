/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOG_
#define _H_COM_DIAG_DIMINUTO_LOG_

/**
 * @file
 * @copyright Copyright 2009-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a common logging API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Log feature provides a logging capabilility that uses the
 * system log if the caller is a daemon, standard error if the
 * caller is not a daemon, or the Linux kernel printk facility if
 * the code is compiled using the kernel build system. This makes it
 * easy to use logging from a reusable piece of code.
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
 *
 * Some of the internal variables used by the Log feature are exposed
 * via a private API header file to facilitate integrating the feature
 * into legacy systems, including with an existing logging system.
 *
 * N.B. The Log functions that are used to print error messages
 * from the value of errno, as well as those macros in the Unit Test
 * framework that uses the Log functions, display the values of the
 * pre-defined preprocessor symbols __FILE__ and __LINE__ as part of
 * the logged message. This is useful - sometimes vital - for debugging
 * and troubleshooting. But it can represent a major security hole in
 * that it may reveal internals of the application, for example through
 * the names of source files. The contents of the system log, and for
 * that matter of standard error output stream on the user terminal,
 * should be treated with the same level of security as that of the
 * application itself.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_platform.h"
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)
#   include <unistd.h>
#endif
#include <stdbool.h>
#include <limits.h>

/*******************************************************************************
 * MASKS
 ******************************************************************************/

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

/**
 * This enumerates the bit masks in the log mask that can be used to
 * represent the eight different levels of log output (from highest to
 * lowest: Emercency, Alert, Critical, Error, Warning, Notice, Information,
 * Debug) or some combination (All, None, Default).
 */
enum DiminutoLogMask {
    DIMINUTO_LOG_MASK_EMERGENCY     = (1 << (7 - 0)),
    DIMINUTO_LOG_MASK_ALERT         = (1 << (7 - 1)),
    DIMINUTO_LOG_MASK_CRITICAL      = (1 << (7 - 2)),
    DIMINUTO_LOG_MASK_ERROR         = (1 << (7 - 3)),
    DIMINUTO_LOG_MASK_WARNING       = (1 << (7 - 4)),
    DIMINUTO_LOG_MASK_NOTICE        = (1 << (7 - 5)),
    DIMINUTO_LOG_MASK_INFORMATION   = (1 << (7 - 6)),
    DIMINUTO_LOG_MASK_DEBUG         = (1 << (7 - 7)),
    DIMINUTO_LOG_MASK_ALL           = (DIMINUTO_LOG_MASK_EMERGENCY | DIMINUTO_LOG_MASK_ALERT | DIMINUTO_LOG_MASK_CRITICAL | DIMINUTO_LOG_MASK_ERROR | DIMINUTO_LOG_MASK_WARNING | DIMINUTO_LOG_MASK_NOTICE | DIMINUTO_LOG_MASK_INFORMATION | DIMINUTO_LOG_MASK_DEBUG),
    DIMINUTO_LOG_MASK_ANY           = DIMINUTO_LOG_MASK_ALL,
    DIMINUTO_LOG_MASK_NONE          = (0),
    DIMINUTO_LOG_MASK_DEFAULT       = (DIMINUTO_LOG_MASK_EMERGENCY | DIMINUTO_LOG_MASK_ALERT | DIMINUTO_LOG_MASK_CRITICAL | DIMINUTO_LOG_MASK_ERROR | DIMINUTO_LOG_MASK_WARNING | DIMINUTO_LOG_MASK_NOTICE),
};

/*******************************************************************************
 * PLATFORM: LINUX KERNEL
 ******************************************************************************/

#if defined(COM_DIAG_DIMINUTO_PLATFORM_KERNEL)

/*
 * Logging can be done from the kernel or a device driver, in which case
 * it uses the <code>printk()</code> function and the kernel priorities.
 * Each translation unit has its own local subsystem log mask which is
 * implicity defined and declared.
 */

#include <linux/kernel.h>

/**
 * This enumerates the log priority values that map from the log level
 * to one of the log priority values used by the Linux kernel.
 */
typedef enum DiminutoLogPriority {
    DIMINUTO_LOG_PRIORITY_EMERGENCY     = KERN_EMERG,
    DIMINUTO_LOG_PRIORITY_ALERT         = KERN_ALERT,
    DIMINUTO_LOG_PRIORITY_CRITICAL      = KERN_CRIT,
    DIMINUTO_LOG_PRIORITY_ERROR         = KERN_ERR,
    DIMINUTO_LOG_PRIORITY_WARNING       = KERN_WARNING,
    DIMINUTO_LOG_PRIORITY_NOTICE        = KERN_NOTICE,
    DIMINUTO_LOG_PRIORITY_INFORMATION   = KERN_INFO,
    DIMINUTO_LOG_PRIORITY_DEBUG         = KERN_DEBUG,
    DIMINUTO_LOG_PRIORITY_DEFAULT       = KERN_NOTICE,
    DIMINUTO_LOG_PRIORITY_PERROR        = KERN_ERR,
} diminuto_log_priority_t;

/**
 * This is the current log mask when compiling for the Linux kernel.
 * Note that it is a local private variable. One possible use for it
 * it to set its value using features of the Linux kernel, like at
 * run-time using the /proc file system, or at load-time using a
 * kernel module paramter.
 */
static diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

#else

/*******************************************************************************
 * PLATFORM: ANDROID BIONIC
 ******************************************************************************/

#   if defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)

/*
 * Logging can be done from an application or a daemon, in the former
 * case it prints to standard error, and in the latter it logs to the
 * Android log. Each translation unit either defines its own subsystem
 * log mask (which may be local or external) or uses the global default
 * subsystem log mask.
 */

#include <android/log.h>

/**
 * This enumerates the log priority values that map from the log level
 * to one of the log priority values used by the Android Bionic library.
 */
typedef enum DiminutoLogPriority {
    DIMINUTO_LOG_PRIORITY_EMERGENCY     = ANDROID_LOG_FATAL,
    DIMINUTO_LOG_PRIORITY_ALERT         = ANDROID_LOG_FATAL,
    DIMINUTO_LOG_PRIORITY_CRITICAL      = ANDROID_LOG_FATAL,
    DIMINUTO_LOG_PRIORITY_ERROR         = ANDROID_LOG_ERROR,
    DIMINUTO_LOG_PRIORITY_WARNING       = ANDROID_LOG_WARN,
    DIMINUTO_LOG_PRIORITY_NOTICE        = ANDROID_LOG_INFO,
    DIMINUTO_LOG_PRIORITY_INFORMATION   = ANDROID_LOG_VERBOSE,
    DIMINUTO_LOG_PRIORITY_DEBUG         = ANDROID_LOG_DEBUG,
    DIMINUTO_LOG_PRIORITY_DEFAULT       = ANDROID_LOG_INFO,
    DIMINUTO_LOG_PRIORITY_PERROR        = ANDROID_LOG_ERROR,
} diminuto_log_priority_t;

/**
 * This is the default log identifier used by the Android Bionic library.
 */
static const char DIMINUTO_LOG_IDENT_DEFAULT[] = "Diminuto";

/**
 * This enumerates the log priority values that map from the log level
 * to one of the log priority values used by the Android Bionic Library.
 */
enum DiminutoLogDefaults {
    DIMINUTO_LOG_OPTION_DEFAULT         = 0,
    DIMINUTO_LOG_FACILITY_DEFAULT       = 0,
};

#   else

/*******************************************************************************
 * PLATFORM: GNU SYSLOG
 ******************************************************************************/

/*
 * Logging can be done from an application or a daemon, in the former
 * case it prints to standard error, and in the latter it logs to the
 * system log. Each translation unit either defines its own subsystem
 * log mask (which may be local or external) or uses the global default
 * subsystem log mask.
 */

#include <syslog.h>

/**
 * This enumerates the log priority values that map from the log level
 * to one of the log priority values used by the Linux/GNU system logger.
 */
typedef enum DiminutoLogPriority {
    DIMINUTO_LOG_PRIORITY_EMERGENCY     = LOG_EMERG,
    DIMINUTO_LOG_PRIORITY_ALERT         = LOG_ALERT,
    DIMINUTO_LOG_PRIORITY_CRITICAL      = LOG_CRIT,
    DIMINUTO_LOG_PRIORITY_ERROR         = LOG_ERR,
    DIMINUTO_LOG_PRIORITY_WARNING       = LOG_WARNING,
    DIMINUTO_LOG_PRIORITY_NOTICE        = LOG_NOTICE,
    DIMINUTO_LOG_PRIORITY_INFORMATION   = LOG_INFO,
    DIMINUTO_LOG_PRIORITY_DEBUG         = LOG_DEBUG,
    DIMINUTO_LOG_PRIORITY_DEFAULT       = LOG_NOTICE,
    DIMINUTO_LOG_PRIORITY_PERROR        = LOG_ERR,
} diminuto_log_priority_t;

/**
 * @def DIMINUTO_LOG_IDENT_DEFAULT
 * This is the default log identifier when compiling for the Linux/GNU
 * system.
 */
#define DIMINUTO_LOG_IDENT_DEFAULT "Diminuto"

/**
 * These are the default log options and facility when using the Linux/GNU
 * system logger.
 */
enum DiminutoLogDefaults {
    DIMINUTO_LOG_OPTION_DEFAULT          = (LOG_CONS | LOG_PID),
    DIMINUTO_LOG_FACILITY_DEFAULT        = LOG_LOCAL7,
};

#   endif

/*******************************************************************************
 * PLATFORM: NOT KERNEL
 ******************************************************************************/

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

#include <stdio.h>
#include <unistd.h>

/**
 * This enumeration defines some manifest constants used by the Log
 * feature.
 */
enum DiminutoLogConstants {
    /* 
     * The defines the default file descriptor used when not using any other
     * system logging mechanism.
     */
    DIMINUTO_LOG_DESCRIPTOR_DEFAULT = STDERR_FILENO,
    /**
     * This defines the maximum size of the log buffer.
     */
    DIMINUTO_LOG_BUFFER_MAXIMUM = 1024,
    /**
     * This defines the maximum size of a host name.
     * RFC1035 and RFC2181 restrict a fully qualified domain name
     * (FQDN) to 253 characters, and each element between the dots
     * to 63 characters. The actual hostname isn't quite either of
     * of these things, but this is a reasonable rationale. This
     * value includes the terminating NUL, and must be at least long
     * enough to hold the string "localhost", the default value if
     * no hostname is available.
     */
    DIMINUTO_LOG_HOSTNAME_MAXIMUM = HOST_NAME_MAX,
};

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_LOG_STREAM_DEFAULT
 * This is the default value used for the log output stream FILE pointer.
 * Its fileno(3) must match the value of the default file descriptor. This
 * is enforced by the diminuto_log_stream() function. Note that this manifest
 * constant cannot be set to, for example, stderr because it is a FILE pointer
 * variable dynamically set by the C run-time system.
 */
#define DIMINUTO_LOG_STREAM_DEFAULT ((FILE *)0)

/**
 * @def DIMINUTO_LOG_MASK_NAME_DEFAULT
 * This is the default value used for the log mask name in the environment.
 */
#define DIMINUTO_LOG_MASK_NAME_DEFAULT "COM_DIAG_DIMINUTO_LOG_MASK"

/**
 * @def DIMINUTO_LOG_PATH_NAME_DEFAULT
 * This is the default value used for the log mask path in the home directory.
 */
#define DIMINUTO_LOG_MASK_PATH_DEFAULT ".com_diag_diminuto_log_mask"

/**
 * @def DIMINUTO_LOG_MASK_VALUE_ALL
 * This is the default value ised for the value of the log mask in the
 * environment.
 */
#define DIMINUTO_LOG_MASK_VALUE_ALL "~0"

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * These are the strategies the log feature may use to determine to where
 * to send logging output. The AUTOMATIC option makes the choice depending
 * on whether the caller has a controlling terminal, and is the default.
 * The SUPPRESS option suppress all log output regardless of its destination.
 * The log strategy variable is part of the log private API.
 */
typedef enum DiminutoLogStrategy {
    DIMINUTO_LOG_STRATEGY_AUTOMATIC = 'A',
    DIMINUTO_LOG_STRATEGY_STDERR    = 'E',
    DIMINUTO_LOG_STRATEGY_SYSLOG    = 'S',
    DIMINUTO_LOG_STRATEGY_SUPPRESS  = 'X',
} diminuto_log_strategy_t;

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

/**
 * This is the log mask: a one bit means the messages at that log level are
 * emitted. The DEBUG level is the least significant bit (even though it is
 * numerically the highest log level), with successively higher levels
 * proceeding to the greater significant bits. The application has read/write
 * access to this external global variable.
 */
extern diminuto_log_mask_t diminuto_log_mask;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

#include <stdarg.h>

/**
 * Map a specified priority into a mask in which the single corresponding bit
 * is set. The Log feature makes use of the actual priority values of the
 * different platforms - GNU syslog, Android Bionic, Linux Kernel - which may
 * differ from platform to platform. So we can't just do a simple bit shift to
 * map from the priority to the log mask. The macros defined in the Log header
 * file avoid this overhead by having a different macro for every priority.
 * Note that an unknown priority maps to ALL mask bits instead of NONE.
 * @param priority is the specified priority.
 * @return the corresponding mask.
 */
extern diminuto_log_mask_t diminuto_log_priority2mask(diminuto_log_priority_t priority);

/**
 * Return true if the specified mask and the log mask and to non-zero,
 * indicating that ANY bit in the specified mask is set in the log mask.
 * @param mask is the specified mask.
 * @return true if enabled, false otherwise.
 */
static inline bool diminuto_log_mask_isenabled(diminuto_log_mask_t mask)
{
    return ((diminuto_log_mask & mask) != 0);
}

/**
 * Return true if the corresponding bit in the log mask for the specified
 * priority is set. 
 * @param priority is the specified priority.
 * @return true if enabled, false otherwise.
 */
static inline bool diminuto_log_priority_isenabled(diminuto_log_priority_t priority)
{
    return diminuto_log_mask_isenabled(diminuto_log_priority2mask(priority));
}

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
 * its value is a number string in decimal, octal, or hexadecimal format as
 * understood by strtoul(3), e.g. "255", "0x377", "0xff", "0xFF", etc. As an
 * extension, if the value is "~0", all eight log levels are enabled.
 * @return the log mask value.
 */
extern diminuto_log_mask_t diminuto_log_setmask(void);

/**
 * Set the global default log mask from a file. The file must contain as its
 * first line of text a number in octal, decimal, or hexadecimal, in C-style
 * format, e.g. 0377, 255, of 0xff. If the path name is NULL,
 * the name ${HOME}/.com_diag_diminuto_log_mask will be used. If the file
 * does not exist, cannot be read, or has a syntax error, the unchanged log
 * mask will be returned.
 * @param is the path name or NULL.
 * @return the log mask value.
 */
extern diminuto_log_mask_t diminuto_log_importmask(const char * path);

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
 * facility. The system log channel is otherwise opened automatically the first
 * time it is used.
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
extern void diminuto_log_syslog(int priority, const char * format, ...) __attribute__ ((format (printf, 2, 3)));

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
extern void diminuto_log_write(int fd, int priority, const char * format, ...) __attribute__ ((format (printf, 3, 4)));

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
extern void diminuto_log_log(int priority, const char * format, ...)  __attribute__ ((format (printf, 2, 3)));

/**
 * If the calling process is interactive, format and print
 * the argument list to stderr; if it is, format and log the argument
 * list to syslog. The message is logged at the default priority and
 * is emitted unconditionally regardless of the current value of the
 * log mask (i.e. it is emitted at the default priority even if that
 * priority is not enabled in the log mask).
 * @param format points to a printf-style format string.
 */
extern void diminuto_log_emit(const char * format, ...)  __attribute__ ((format (printf, 1, 2)));

/*
 * Legacy applications expect the Log error functions to be included as
 * part of the Log feature.
 */
#include "com/diag/diminuto/diminuto_error.h"

#endif

/*******************************************************************************
 * PLATFORM: ANY
 ******************************************************************************/

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

#include "com/diag/diminuto/diminuto_token.h"

#define DIMINUTO_LOG_HERE __FILE__ "@" DIMINUTO_TOKEN_TOKEN(__LINE__) ": "

#endif

/*******************************************************************************
 * END OF IDEMPOTENT HEADER
 ******************************************************************************/

/*
 * The Log header file may be included more than once. The preprocessor macros
 * below may be redefined upon subsequent inclusions.
 */

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

#if !defined(DIMINUTO_LOG_MASK)
#   define DIMINUTO_LOG_MASK diminuto_log_mask
#endif

#if defined(DIMINUTO_LOG_ENABLED)
#   undef DIMINUTO_LOG_ENABLED
#endif

#define DIMINUTO_LOG_ENABLED(_MASK_) (((DIMINUTO_LOG_MASK) & (_MASK_)) != 0)

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

/*******************************************************************************
 * PRIORITY: EMERGENCY
 ******************************************************************************/

#if defined(DIMINUTO_LOG_EMERGENCY)
#   undef DIMINUTO_LOG_EMERGENCY
#endif

#if defined(DIMINUTO_LOG_EMERGENCY_DISABLE)
#   define DIMINUTO_LOG_EMERGENCY(...) ((void)0)
#else 
#   define DIMINUTO_LOG_EMERGENCY(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_EMERGENCY, DIMINUTO_LOG_PRIORITY_EMERGENCY, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: ALERT
 ******************************************************************************/

#if defined(DIMINUTO_LOG_ALERT)
#   undef DIMINUTO_LOG_ALERT
#endif

#if defined(DIMINUTO_LOG_ALERT_DISABLE)
#   define DIMINUTO_LOG_ALERT(...) ((void)0)
#else
#   define DIMINUTO_LOG_ALERT(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_ALERT, DIMINUTO_LOG_PRIORITY_ALERT, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: CRITICAL
 ******************************************************************************/

#if defined(DIMINUTO_LOG_CRITICAL)
#   undef DIMINUTO_LOG_CRITICAL
#endif

#if defined(DIMINUTO_LOG_CRITICAL_DISABLE)
#   define DIMINUTO_LOG_CRITICAL(...) ((void)0)
#else
#   define DIMINUTO_LOG_CRITICAL(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_CRITICAL, DIMINUTO_LOG_PRIORITY_CRITICAL, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: ERROR
 ******************************************************************************/

#if defined(DIMINUTO_LOG_ERROR)
#   undef DIMINUTO_LOG_ERROR
#endif

#if defined(DIMINUTO_LOG_ERROR_DISABLE)
#   define DIMINUTO_LOG_ERROR(...) ((void)0)
#else
#   define DIMINUTO_LOG_ERROR(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_ERROR, DIMINUTO_LOG_PRIORITY_ERROR, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: WARNING
 ******************************************************************************/

#if defined(DIMINUTO_LOG_WARNING)
#   undef DIMINUTO_LOG_WARNING
#endif

#if defined(DIMINUTO_LOG_WARNING_DISABLE)
#   define DIMINUTO_LOG_WARNING(...) ((void)0)
#else
#   define DIMINUTO_LOG_WARNING(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_WARNING, DIMINUTO_LOG_PRIORITY_WARNING, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: NOTICE
 ******************************************************************************/

#if defined(DIMINUTO_LOG_NOTICE)
#   undef DIMINUTO_LOG_NOTICE
#endif

#if defined(DIMINUTO_LOG_NOTICE_DISABLE)
#   define DIMINUTO_LOG_NOTICE(...) ((void)0)
#else
#   define DIMINUTO_LOG_NOTICE(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_NOTICE, DIMINUTO_LOG_PRIORITY_NOTICE, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: INFORMATION
 ******************************************************************************/

#if defined(DIMINUTO_LOG_INFORMATION)
#   undef DIMINUTO_LOG_INFORMATION
#endif

#if defined(DIMINUTO_LOG_INFORMATION_DISABLE)
#   define DIMINUTO_LOG_INFORMATION(...) ((void)0)
#else
#   define DIMINUTO_LOG_INFORMATION(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_INFORMATION, DIMINUTO_LOG_PRIORITY_INFORMATION, __VA_ARGS__)
#endif

/*******************************************************************************
 * PRIORITY: DEBUG
 ******************************************************************************/

#if defined(DIMINUTO_LOG_DEBUG)
#   undef DIMINUTO_LOG_DEBUG
#endif

#if defined(DIMINUTO_LOG_DEBUG_DISABLE)
#   define DIMINUTO_LOG_DEBUG(...) ((void)0)
#else
#   define DIMINUTO_LOG_DEBUG(...) DIMINUTO_LOG_IF(DIMINUTO_LOG_MASK_DEBUG, DIMINUTO_LOG_PRIORITY_DEBUG, __VA_ARGS__)
#endif

/*******************************************************************************
 * END
 ******************************************************************************/
