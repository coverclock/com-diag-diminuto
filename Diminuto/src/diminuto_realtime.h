/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_REALTIME_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_REALTIME_PRIVATE_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines the private API for support for the PREEMPT_RT kernel feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Provides a private API for the Real-Time Preemption feature in the Linux
 * kerne 6.8 and beyond.
 */

#include <stdio.h>

/**
 * @def COM_DIAG_DIMINUTO_REALTIME_PATH
 * Defines the path in the file system for the realtime variable.
 */
#define COM_DIAG_DIMINUTO_REALTIME_PATH "/sys/kernel/realtime"

/**
 * This is the SYSFS file that, if it exists, indicates that the running
 * Linux kernel supports the Real-Time Preemptive scheduling feature.
 */
static const char DIMINUTO_REALTIME_PATH[] = COM_DIAG_DIMINUTO_REALTIME_PATH;

/**
 * Tests the specified path to see if it indicates support for PREEMPT_RT.
 * @param path points to the path name.
 * @return >0 if supported, 0 if not supported, <0 if an error occurred.
 */
extern int diminuto_realtime_is_supported_path(const char * path);

/**
 * Tests the specified file to see if it indicates support for PREEMPT_RT.
 * @param fp points to the FILE structure referencing the file.
 * @param path points to the path name.
 * @return >0 if supported, 0 if not supported, <0 if an error occurred.
 */
extern int diminuto_realtime_is_supported_fp(FILE * fp, const char * path);

#endif
