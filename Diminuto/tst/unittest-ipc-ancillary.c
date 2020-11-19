/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the sendmsg(2) ancillary data capability.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the use of sendmsg(2) to send ancillary data
 * using Diminutos IPC4 and IPCL features. The test is complex enough
 * and uses multiple IPC features that it merited its own unit test.
 *
 * REFERENCES
 *
 * C. Sridharan, "File Descriptor Transfer over Unix Domain Sockets",
 * CopyConstruct, August 2020
 *
 * unix(7) man page
 *
 * socket(7) man page
 *
 * sendmsg(2) man page
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int sn = 0;
int serverport = 0;

static void * client(void * arg)
{
    uintptr_t count = 0;
    diminuto_ipv4_t address = 0;
    int streamsocket = -1;
    diminuto_ipv4_buffer_t buffer = { '\0', };
    uintptr_t ii = 0;
    int request = -1;
    int reply = -1;

    count = (uintptr_t)arg;

    ASSERT(serverport != 0);

    address = diminuto_ipc4_address("localhost");
    ASSERT(!diminuto_ipc4_is_unspecified(&address));

    ASSERT((streamsocket = diminuto_ipc4_stream_consumer(address, serverport)) >= 0);

    COMMENT("client %d connected %s:%d for %d\n",
        streamsocket,
        diminuto_ipc4_address2string(address, buffer, sizeof(buffer)),
        serverport,
        count);

    for (ii = 0; ii < count; ++ii) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            request = sn++;
        DIMINUTO_CRITICAL_SECTION_END;
        reply = ~request;

        ASSERT(diminuto_ipc4_stream_write(streamsocket, &request, sizeof(request)) == sizeof(request));
        ASSERT(diminuto_ipc4_stream_read(streamsocket, &reply, sizeof(reply)) == sizeof(reply));
        ASSERT(request == reply);

    }

    ASSERT(diminuto_ipc_close(streamsocket) >= 0);

    return (void *)ii;
}

static void * server(void * arg)
{
    int streamsocket = -1;
    diminuto_mux_t mux;
    int ready = 0;
    int fd = -1;
    ssize_t length = -1;
    diminuto_sticks_t datum = 0;
    int count = 0;
    size_t total = 0;

    ASSERT((streamsocket = (intptr_t)arg) >= 0);

    ASSERT(diminuto_mux_init(&mux) == &mux);
    ASSERT(diminuto_mux_register_read(&mux, streamsocket) >= 0);

    while (!0) {

        if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            FATAL("server: diminuto_mux_wait: timeout");
        } else if (errno == EINTR) {
            continue;
        } else {
            FATAL("server: diminuto_mux_wait: error");
        }

        while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

            ASSERT(fd == streamsocket);
            ASSERT((length = diminuto_ipc4_stream_read(streamsocket, &datum, sizeof(datum))) >= 0);
            if (length == 0) { break; }
            ASSERT(length == sizeof(datum));
            ASSERT(diminuto_ipc4_stream_write(streamsocket, &datum, length) ==  length);
            count += 1;
            total += length;

        }

    }

    COMMENT("server %d processed %d among %d\n",
        streamsocket,
        total,
        count);

    ASSERT(diminuto_mux_close(&mux, streamsocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    return (void *)(uintptr_t)total;
}

static void * listener(void * arg)
{
    int listensocket = -1;
    diminuto_mux_t mux;
    int pending = 0;
    int ready = -1;
    diminuto_thread_t thread;
    int fd = -1;
    diminuto_ipv4_t address = 0;
    diminuto_port_t port = 0;
    int streamsocket = -1;;
    diminuto_ipv4_buffer_t buffer = { '\0', };

    ASSERT((listensocket = (intptr_t)arg) >= 0);

    ASSERT(diminuto_mux_init(&mux) == &mux);
    ASSERT(diminuto_mux_register_accept(&mux, listensocket) >= 0);

    while (diminuto_thread_notifications() == 0) {

        if (pending) {
            ASSERT(diminuto_thread_wait(&thread) == 0);
            ASSERT(diminuto_thread_fini(&thread) == (diminuto_thread_t *)0);
            pending = 0;
        }

        if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            FATAL("listener: diminuto_mux_wait: timeout");
        } else if (errno == EINTR) {
            continue;
        } else {
            FATAL("listener: diminuto_mux_wait: error");
        }

        ASSERT((fd = diminuto_mux_ready_accept(&mux)) == listensocket);

        ASSERT((streamsocket = diminuto_ipc4_stream_accept_generic(listensocket, &address, &port)) >= 0);

        COMMENT("listener %d accepted %s:%d on %d\n",
            listensocket,
            diminuto_ipc4_address2string(address, buffer, sizeof(buffer)),
            port,
            streamsocket);

        ASSERT(diminuto_thread_init(&thread, server) == &thread);
        ASSERT(diminuto_thread_start(&thread, (void *)(intptr_t)streamsocket) == 0);
        pending = !0;

    }

    if (pending) {
        ASSERT(diminuto_thread_wait(&thread) == 0);
        ASSERT(diminuto_thread_fini(&thread) == (diminuto_thread_t *)0);
    }

    ASSERT(diminuto_thread_fini(&thread) == (diminuto_thread_t *)0);

    ASSERT(diminuto_mux_unregister_accept(&mux, listensocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    return 0;
}

int main(int argc, char argv[])
{
    SETLOGMASK();

    EXIT();
}
