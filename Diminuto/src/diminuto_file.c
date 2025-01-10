/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the File feature. EXPERIMENTAL
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the File feature. Since the File feature
 * is already somewhat sketchy (because it depends on the exact internal
 * implementation of standard I/O), it seemed appropriate to put this
 * experimental (and not all that efficient) function here.
 */

#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include <errno.h>

ssize_t diminuto_file_poll(FILE * fp) {
    int fd = -1;
    ssize_t bytes = -1;
    diminuto_mux_t mux = DIMINUTO_MUX_INIT;
    int ready = -1;
    int rc = -1;

    do {

        if (fp == (FILE *)0) {
            errno = EINVAL;
            break;
        }

        if (feof(fp)) {
            bytes = 1;
            break;
        }

        if ((bytes = diminuto_file_ready(fp)) > 0) {
            break;
        }

        if ((fd = fileno(fp)) < 0) {
            errno = EBADF;
            break;
        }

        if (!diminuto_serial_valid(fd)) {
            /* Do nothing. */
        } else  if ((bytes = diminuto_serial_available(fd)) <= 0) {
            /* Do nothing. */
        } else {
            break;
        }

        if (diminuto_mux_init(&mux) == (diminuto_mux_t *)0) {
            break;
        }

        if (diminuto_mux_register_read(&mux, fd) < 0) {
            break;
        }

        while (!0) {
            if ((ready = diminuto_mux_wait(&mux, 0 /* POLL */)) == 0) {
                bytes = 0;
                break;
            } else if (ready > 0) {
                rc = diminuto_mux_ready_read(&mux);
                diminuto_contract(rc == fd);
                bytes = 1;
                /* May be EOF. */
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                break;
            }
        }

        (void)diminuto_mux_fini(&mux);

    } while (0);

    if (bytes < 0) {
        diminuto_perror("diminuto_file_poll");
    }

    return bytes;
}
