/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LOG_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_LOG_PRIVATE_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines the Log private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This defines the Log feature's private API.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <pthread.h>

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

/**
 * This is the mutex used to serialize access to shared variables internal
 * to the log feature. It is global so that applications may synchronize
 * read-modify-write operations to, for example, the log mask.
 */
extern pthread_mutex_t diminuto_log_mutex;

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
 * caller is not running as a daemon. It must match the fileno(3) of the
 * default log file stream. This is enforced by the diminuto_log_stream()
 * function.
 */
extern int diminuto_log_descriptor;

/**
 * This is the file stream object which corresponds to the file descriptor.
 * (Some platforms have underlying logging mechanisms that need this.) This
 * is enforced by the diminuto_log_stream() function.
 */
extern FILE * diminuto_log_file;

/**
 * This is the name of the environmental variable assumed to contain a number
 * that is the log mask.
 */
extern const char * diminuto_log_mask_name;

/**
 * This variable determines how log messages are routed: to standard
 * error, to the system log, or automatically based on the characteristics
 * of the caller.
 */
extern diminuto_log_strategy_t diminuto_log_strategy;

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

/**
 * This is the default log priority for those log functions which do not
 * have a priority parameter, like diminuto_log_emit().
 */
extern diminuto_log_priority_t diminuto_log_priority;

/**
 * This is the default log priority for those log functions which print
 * the text associated with an error number, like diminuto_log_perror().
 */
extern diminuto_log_priority_t diminuto_log_error;

/**
 * This counter is incremented every time a log message is believed to
 * have been lost.
 */
extern size_t diminuto_log_lost;

#endif
