/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_REALTIME_
#define _H_COM_DIAG_DIMINUTO_REALTIME_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines the public API for support for the RT_PREEMPT kernel feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Provides a public API for the Real-Time Preemption feature in the Linux
 * kerne 6.8 and beyond.
 */

/**
 * This is the SYSFS file that, if it exists, indicates that the running
 * Linux kernel supports the Real-Time Preemptive scheduling feature.
 */
static const char DIMINUTO_REALTIME_PATH[] = "/sys/kernel/realtime";

/**
 * Tests the specified path to see if it indicates support for RT_PREEMPT.
 * @param path points to the path name.
 * @return >0 if supported, 0 if not supported, <0 if an error occurred.
 */
extern int diminuto_realtime_supported_path(const char * path);

/**
 * Tests the default path to see if it indicates support for RT_PREEMPT.
 * @param path points to the path name.
 * @return >0 if supported, 0 if not supported, <0 if an error occurred.
 */
static inline int diminuto_realtime_supported(void) {
    return diminuto_realtime_supported_path(DIMINUTO_REALTIME_PATH);
}

#endif
