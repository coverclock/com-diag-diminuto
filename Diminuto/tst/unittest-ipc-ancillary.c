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
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
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

typedef int datum_t;

diminuto_ipv4_t serveraddress = 0;
diminuto_port_t serverport = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int sn = 0;

static void * client(void * arg)
{
    int count = 0;
    int streamsocket = -1;
    diminuto_ipv4_buffer_t buffer = { '\0', };
    int ii = 0;
    datum_t request = -1;
    datum_t reply = -1;
    ssize_t length = 0;

    count = (int)(intptr_t)arg;

    ASSERT(serverport != 0);
    ASSERT((streamsocket = diminuto_ipc4_stream_consumer(serveraddress, serverport)) >= 0);
    COMMENT("client %d connected %s:%d for %d\n", streamsocket, diminuto_ipc4_address2string(serveraddress, buffer, sizeof(buffer)), serverport, count);

    for (ii = 0; ii < count; ++ii) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            request = sn++;
        DIMINUTO_CRITICAL_SECTION_END;
        reply = ~request;

        ASSERT((length = diminuto_ipc4_stream_write_generic(streamsocket, &request, sizeof(request), sizeof(request))) == sizeof(request));
        COMMENT("client %d wrote %zd\n", streamsocket, length);

        ASSERT((length = diminuto_ipc4_stream_read_generic(streamsocket, &reply, sizeof(reply), sizeof(reply))) == sizeof(reply));
        COMMENT("client %d read %zd\n", streamsocket, length);

        ASSERT(request == reply);

    }

    ASSERT(diminuto_ipc_close(streamsocket) >= 0);

    return (void *)(intptr_t)ii;
}

static void * server(void * arg)
{
    int streamsocket = -1;
    diminuto_mux_t mux;
    int ready = 0;
    int fd = -1;
    ssize_t length = -1;
    datum_t datum = 0;
    int count = 0;
    size_t total = 0;

    ASSERT((streamsocket = (intptr_t)arg) >= 0);

    ASSERT(diminuto_mux_init(&mux) == &mux);
    ASSERT(diminuto_mux_register_read(&mux, streamsocket) >= 0);

    while (!0) {

        if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            continue;
        } else if (errno == EINTR) {
            COMMENT("server %d interrupted\n", streamsocket);
            break;
        } else {
            FATAL("server: diminuto_mux_wait: error");
        }

        while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

            ASSERT(fd == streamsocket);
            ASSERT((length = diminuto_ipc4_stream_read_generic(streamsocket, &datum, sizeof(datum), sizeof(datum))) >= 0);
            COMMENT("server %d read %zd after %d\n", streamsocket, length, count);

            if (length == 0) {
                COMMENT("server %d completed\n", streamsocket);
                break;
            }

            ASSERT(length == sizeof(datum));
            ASSERT((length = diminuto_ipc4_stream_write_generic(streamsocket, &datum, length, length)) ==  sizeof(datum));
            COMMENT("server %d write %zd after %d\n", streamsocket, length, count);

            count += 1;
            total += length;

        }

        if (length == 0) {
            break;
        }

    }

    COMMENT("server %d finished %d after %d\n", streamsocket, total, count);

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
    diminuto_thread_t serverthread;
    int fd = -1;
    int count = 0;
    diminuto_ipv4_t address = 0;
    diminuto_port_t port = 0;
    int streamsocket = -1;;
    diminuto_ipv4_buffer_t buffer = { '\0', };
    void * result = 0;

    ASSERT((listensocket = (intptr_t)arg) >= 0);

    ASSERT(diminuto_thread_init(&serverthread, server) == &serverthread);

    ASSERT(diminuto_mux_init(&mux) == &mux);
    ASSERT(diminuto_mux_register_accept(&mux, listensocket) >= 0);

    while (diminuto_thread_notifications() == 0) {

        if (pending) {
            ASSERT(diminuto_thread_join(&serverthread, &result) == 0);
            pending = 0;
        }

        if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            continue;
        } else if (errno == EINTR) {
            COMMENT("listener %d interrupted\n", listensocket);
            continue;
        } else {
            FATAL("listener: diminuto_mux_wait: error");
        }

        ASSERT((fd = diminuto_mux_ready_accept(&mux)) == listensocket);
        count += 1;

        ASSERT((streamsocket = diminuto_ipc4_stream_accept_generic(listensocket, &address, &port)) >= 0);

        COMMENT("listener %d accepted %s:%d on %d\n", listensocket, diminuto_ipc4_address2string(address, buffer, sizeof(buffer)), port, streamsocket);

        ASSERT(diminuto_thread_start(&serverthread, (void *)(intptr_t)streamsocket) == 0);
        pending = !0;

    }

    COMMENT("listener %d notified\n", listensocket);

    if (pending) {
        ASSERT(diminuto_thread_join(&serverthread, &result) == 0);
    }

    ASSERT(diminuto_mux_unregister_accept(&mux, listensocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    ASSERT(diminuto_thread_fini(&serverthread) == (diminuto_thread_t *)0);

    return (void *)(intptr_t)count;
}

int main(int argc, char argv[])
{
    int listensocket = -1;
    diminuto_ipv4_buffer_t buffer = { '\0', };
    pid_t pid = -1;

    SETLOGMASK();

    ASSERT((listensocket = diminuto_ipc4_stream_provider(0)) >= 0);
    ASSERT(diminuto_ipc4_nearend(listensocket, &serveraddress, &serverport) >= 0);

    COMMENT("main %d listening %s:%d\n", listensocket, diminuto_ipc4_address2string(serveraddress, buffer, sizeof(buffer)), serverport);

    ASSERT((pid = fork()) >= 0);
    if (pid == 0) {
        diminuto_thread_t clientthread;
        void * result = (void *)0;

        ASSERT(diminuto_thread_init(&clientthread, client) == &clientthread);

        ASSERT(diminuto_thread_start(&clientthread, (void *)(intptr_t)5) == 0);
        ASSERT(diminuto_thread_join(&clientthread, &result) == 0);
        ASSERT((intptr_t)result == 5);

        ASSERT(diminuto_thread_start(&clientthread, (void *)(intptr_t)7) == 0);
        ASSERT(diminuto_thread_join(&clientthread, &result) == 0);
        ASSERT((intptr_t)result == 7);

        ASSERT(diminuto_thread_start(&clientthread, (void *)(intptr_t)11) == 0);
        ASSERT(diminuto_thread_join(&clientthread, &result) == 0);
        ASSERT((intptr_t)result == 11);

        ASSERT(diminuto_thread_fini(&clientthread) == (diminuto_thread_t *)0);

        EXIT();

    } else {
        diminuto_thread_t listenerthread;
        void * result = (void *)0;
        int status = -1;

        ASSERT(diminuto_thread_init(&listenerthread, listener) == &listenerthread);

        ASSERT(diminuto_thread_start(&listenerthread, (void *)(intptr_t)listensocket) == 0);

        COMMENT("main %d waiting\n", listensocket);
        diminuto_delay(diminuto_frequency() * 10, !0);

        COMMENT("main %d notifying\n", listensocket);
        ASSERT(diminuto_thread_notify(&listenerthread) == 0);
        ASSERT(diminuto_thread_join(&listenerthread, &result) == 0);
        ASSERT((intptr_t)result == 3);
        ASSERT(diminuto_thread_fini(&listenerthread) == (diminuto_thread_t *)0);

        ASSERT(waitpid(pid, &status, 0) == pid);
        COMMENT("main %d reaped %d status %d\n", listensocket, pid, status);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == 0);

        ASSERT(diminuto_ipc_close(listensocket) >= 0);
        COMMENT("main complete\n");

        EXIT();
    }
}
