/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_REALTIME_
#define _H_COM_DIAG_DIMINUTO_REALTIME_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

static const char DIMINUTO_REALTIME_PATH[] = "/sys/kernel/realtime";

extern int diminuto_realtime_supported_path(const char * path);

static inline int diminuto_realtime_supported(void) {
    return diminuto_realtime_supported_path(DIMINUTO_REALTIME_PATH);
}

#endif
