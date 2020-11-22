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
 * It is intended to simulate an actual application albeit on a small
 * scale. It may "simulate" but all of the processes, threads, and
 * sockets are the real thing.
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
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_countof.h"
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

static const char INSTANCEPATH[] = "/tmp/unittest-ipc-ancillary.sock";

static diminuto_ipv4_t serveraddress = 0;
static diminuto_port_t serverport = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int sn = 0;

/*
 * This thread represents the client who exchanges several requests and
 * replies with a server over a single IPv4 stream socket. The exact number
 * of exchanges is passed in as an argument. The thread exits when all of
 * the exchanges have been performed.
 */
static void * client(void * arg /* limit */)
{
    int limit = 0;
    int streamsocket = -1;
    diminuto_ipv4_t nearendaddress = 0;
    diminuto_port_t nearendport = 0;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_t farendaddress = 0;
    diminuto_port_t farendport = 0;
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
    int ii = 0;
    datum_t request = -1;
    datum_t reply = -1;
    ssize_t length = 0;
    int count = 0;
    size_t total = 0;

    limit = (int)(intptr_t)arg;

    ASSERT(serverport != 0);
    ASSERT((streamsocket = diminuto_ipc4_stream_consumer(serveraddress, serverport)) >= 0);
    ASSERT(diminuto_ipc4_nearend(streamsocket, &nearendaddress, &nearendport) >= 0);
    ASSERT(diminuto_ipc4_farend(streamsocket, &farendaddress, &farendport) >= 0);
    CHECKPOINT("client %d nearend %s:%d farend %s:%d\n", streamsocket, diminuto_ipc4_address2string(nearendaddress, nearendbuffer, sizeof(nearendbuffer)), nearendport, diminuto_ipc4_address2string(farendaddress, farendbuffer, sizeof(farendbuffer)), farendport);

    for (ii = 0; ii < limit; ++ii) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            request = sn++;
        DIMINUTO_CRITICAL_SECTION_END;
        reply = ~request;

        ASSERT((length = diminuto_ipc4_stream_write_generic(streamsocket, &request, sizeof(request), sizeof(request))) == sizeof(request));
        COMMENT("client %d wrote %zd after %d\n", streamsocket, length, ii);

        ASSERT((length = diminuto_ipc4_stream_read_generic(streamsocket, &reply, sizeof(reply), sizeof(reply))) == sizeof(reply));
        COMMENT("client %d read %zd after %d\n", streamsocket, length, ii);

        count += 1;
        total += length;

        ASSERT(request == reply);

    }

    CHECKPOINT("client %d finished %zu after %d\n", streamsocket, total, count);

    ASSERT(count == limit);

    ASSERT(diminuto_ipc_close(streamsocket) >= 0);

    return (void *)(intptr_t)ii;
}

/*
 * This thread represents the server that replies to the requests of a 
 * single client over a IPv4 stream socket that is already connected
 * to the client. The stream socket is passed in as an argument. The
 * thread exits when the far end closes the stream socket.
 */
static void * server(void * arg /* streamsocket */)
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
            CHECKPOINT("server %d interrupted\n", streamsocket);
            break;
        } else {
            FATAL("server: diminuto_mux_wait: error");
        }

        while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

            ASSERT(fd == streamsocket);
            ASSERT((length = diminuto_ipc4_stream_read_generic(streamsocket, &datum, sizeof(datum), sizeof(datum))) >= 0);
            COMMENT("server %d read %zd after %d\n", streamsocket, length, count);

            if (length == 0) {
                break;
            }

            ASSERT(length == sizeof(datum));
            ASSERT((length = diminuto_ipc4_stream_write_generic(streamsocket, &datum, length, length)) ==  sizeof(datum));
            COMMENT("server %d wrote %zd after %d\n", streamsocket, length, count);

            count += 1;
            total += length;

        }

        if (length == 0) {
            break;
        }

    }

    CHECKPOINT("server %d finished %zu after %d\n", streamsocket, total, count);

    ASSERT(diminuto_mux_close(&mux, streamsocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    return (void *)(uintptr_t)total;
}

/*
 * This thread represents the dispatcher that listens on a IPv4 socket
 * in the listen state, creates a new stream socket for every connection
 * request from a client that it accepts, and dispatches a new server
 * thread to handle the request. The listen socket is passed in as an
 * argument. The thread exits when it is notified to do so.
 */
static void * dispatcher(void * arg /* listensocket */)
{
    int listensocket = -1;
    diminuto_mux_t mux;
    int pending = 0;
    int ready = -1;
    diminuto_thread_t serverthread;
    int fd = -1;
    int count = 0;
    int streamsocket = -1;
    diminuto_ipv4_t nearendaddress = 0;
    diminuto_port_t nearendport = 0;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_t farendaddress = 0;
    diminuto_port_t farendport = 0;
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
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
            CHECKPOINT("dispatcher %d interrupted\n", listensocket);
            continue;
        } else {
            FATAL("dispatcher: diminuto_mux_wait: error");
        }

        ASSERT((fd = diminuto_mux_ready_accept(&mux)) == listensocket);
        count += 1;

        ASSERT((streamsocket = diminuto_ipc4_stream_accept(listensocket)) >= 0);
        ASSERT(diminuto_ipc4_nearend(streamsocket, &nearendaddress, &nearendport) >= 0);
        ASSERT(diminuto_ipc4_farend(streamsocket, &farendaddress, &farendport) >= 0);
        CHECKPOINT("dispatcher %d nearend %s:%d farend %s:%d\n", streamsocket, diminuto_ipc4_address2string(nearendaddress, nearendbuffer, sizeof(nearendbuffer)), nearendport, diminuto_ipc4_address2string(farendaddress, farendbuffer, sizeof(farendbuffer)), farendport);

        ASSERT(diminuto_thread_start(&serverthread, (void *)(intptr_t)streamsocket) == 0);
        pending = !0;

    }

    CHECKPOINT("dispatcher %d notified\n", listensocket);

    if (pending) {
        ASSERT(diminuto_thread_join(&serverthread, &result) == 0);
    }

    ASSERT(diminuto_mux_unregister_accept(&mux, listensocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    ASSERT(diminuto_thread_fini(&serverthread) == (diminuto_thread_t *)0);

    return (void *)(intptr_t)count;
}

/*
 * This process represents the workload for the system, creating and
 * starting client threads running in parallel to make requests of the
 * of the server. The number of client threads in concurrent play at
 * any one time is passed in as an argument. The process exits when
 * it receives a SIGINT signal.
 */
static void workload(int count)
{
    diminuto_thread_t * clientthreads = (diminuto_thread_t *)0;
    int ii = 0;
    void * result = (void *)0;

    CHECKPOINT("workload starting\n");

    ASSERT((clientthreads = (diminuto_thread_t *)calloc(count, sizeof(*clientthreads))) != (diminuto_thread_t *)0);

    ASSERT(diminuto_interrupter_install(0) >= 0);

    for (ii = 0; ii < count; ++ii) {
        /*
         * The first client will disconnect without ever doing any
         * requests because its count will be zero; we want to test
         * that too.
         */
        ASSERT(diminuto_thread_init(&clientthreads[ii], client) == &clientthreads[ii]);
        ASSERT(diminuto_thread_start(&clientthreads[ii], (void *)(intptr_t)ii) == 0);
        diminuto_delay(diminuto_frequency()/100, !0);
    }

    while (diminuto_interrupter_check() <= 0) {
        for (ii = 0; ii < count; ++ii) {
            ASSERT(diminuto_thread_join(&clientthreads[ii], &result) == 0);
            ASSERT((intptr_t)result == ii);
            ASSERT(diminuto_thread_start(&clientthreads[ii], (void *)(intptr_t)ii) == 0);
        }
    }

    CHECKPOINT("workload interrupted\n");

    for (ii = 0; ii < count; ++ii) {
        ASSERT(diminuto_thread_join(&clientthreads[ii], &result) == 0);
        ASSERT((intptr_t)result == ii);
        ASSERT(diminuto_thread_fini(&clientthreads[ii]) == (diminuto_thread_t *)0);
    }

    free(clientthreads);

    CHECKPOINT("workload exiting\n");
}

/*
 * This process represents the web server instance. It creates the
 * dispatcher thread. The listen socket is passed in as an argument.
 * When the instance receives a SIGINT signal, it notifies the dispatcher
 * to terminate, it joins with the dispatcher, and then exits.
 */
static void instance(int listensocket) 
{
    diminuto_local_t path = { '\0', };
    diminuto_local_t nearendpath = { '\0', };
    diminuto_local_t farendpath = { '\0', };
    int instancesocket = -1;
    diminuto_thread_t dispatcherthread;
    diminuto_mux_t mux;
    void * result = (void *)0;

    CHECKPOINT("instance starting\n");

    ASSERT(diminuto_interrupter_install(0) >= 0);
    ASSERT(diminuto_mux_init(&mux) == &mux);
    ASSERT(diminuto_thread_init(&dispatcherthread, dispatcher) == &dispatcherthread);

    ASSERT(diminuto_ipcl_path(INSTANCEPATH, path, sizeof(path)) == (char *)&path);
    ASSERT((instancesocket = diminuto_ipcl_packet_consumer(path)) >= 0);
    ASSERT(diminuto_ipcl_nearend(instancesocket, nearendpath, sizeof(nearendpath)) >= 0);
    ASSERT(diminuto_ipcl_farend(instancesocket, farendpath, sizeof(farendpath)) >= 0);
    CHECKPOINT("main %d nearend \"%s\" farend \"%s\"\n", instancesocket, diminuto_ipcl_path2string(nearendpath), diminuto_ipcl_path2string(farendpath));

    ASSERT(diminuto_ipc_close(instancesocket) >= 0);
    ASSERT(diminuto_thread_start(&dispatcherthread, (void *)(intptr_t)listensocket) == 0);

    while (diminuto_interrupter_check() <= 0) {
        diminuto_mux_wait(&mux, -1);
    }

    CHECKPOINT("instance %d interrupted\n", listensocket);

    ASSERT(diminuto_thread_notify(&dispatcherthread) == 0);
    ASSERT(diminuto_thread_join(&dispatcherthread, &result) == 0);
    ASSERT(diminuto_thread_fini(&dispatcherthread) == (diminuto_thread_t *)0);

    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    CHECKPOINT("instance %d exiting\n", listensocket);
}

int main(int argc, char argv[])
{
    int listensocket = -1;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
    diminuto_local_t path = { '\0', };
    diminuto_local_t nearendpath = { '\0', };
    diminuto_local_t farendpath = { '\0', };
    int instancesocket = -1;
    int activationsocket = -1;
    pid_t workloadpid = -1;
    pid_t instancepid = -1;
    int status = -1;

    SETLOGMASK();

    /*
     * In any real application we would have a fixed port number,
     * probably defined as a service, and would have an address
     * that was resolved from a Fully Qualified Domain Name
     * (FQDN). But this unit test is running on one computer,
     * and we'll let the IP stack choose an ephemeral port for
     * us to use.
     */

    ASSERT((listensocket = diminuto_ipc4_stream_provider(0)) >= 0);
    ASSERT(diminuto_ipc4_nearend(listensocket, &serveraddress, &serverport) >= 0);
    CHECKPOINT("listen %d nearend %s:%d farend %s:%d\n", listensocket, diminuto_ipc4_address2string(serveraddress, nearendbuffer, sizeof(nearendbuffer)), serverport, diminuto_ipc4_address2string(0xffffffff, farendbuffer, sizeof(farendbuffer)), 0);

    /*
     * Workload will queue up clients on the listen socket requesting a
     * connection to a server.
     */

    ASSERT((workloadpid = fork()) >= 0);
    if (workloadpid == 0) {
        (void)diminuto_ipc_close(listensocket);
        workload(10);
        EXIT();
    }

    /*
     * When an instance is ready to get a listen socket on which to
     * service clients, it will contact us via this UNIX domain (local)
     * socket. Just like clients will queue up on the listen socket,
     * instances will queue up on the instance socket.
     */

    ASSERT(diminuto_ipcl_path(INSTANCEPATH, path, sizeof(path)) == (char *)&path);
    ADVISE(diminuto_ipcl_remove(path) >= 0);
    ASSERT((instancesocket = diminuto_ipcl_packet_provider(path)) >= 0);
    ASSERT(diminuto_ipcl_nearend(instancesocket, nearendpath, sizeof(nearendpath)) >= 0);
    CHECKPOINT("instance %d nearend \"%s\" farend \"%s\"\n", instancesocket, diminuto_ipcl_path2string(nearendpath), diminuto_ipcl_path2string((const char *)0));


    ASSERT((instancepid = fork()) >= 0);
    if (instancepid == 0) {
        instance(listensocket);
        EXIT();
    }

    ASSERT((activationsocket = diminuto_ipcl_packet_accept(instancesocket)) >= 0);
    ASSERT(diminuto_ipcl_nearend(activationsocket, nearendpath, sizeof(nearendpath)) >= 0);
    ASSERT(diminuto_ipcl_farend(activationsocket, farendpath, sizeof(farendpath)) >= 0);
    CHECKPOINT("activation %d nearend \"%s\" farend \"%s\"\n", activationsocket, diminuto_ipcl_path2string(nearendpath), diminuto_ipcl_path2string(farendpath));

    /*
     * It is during this delay in which all the work gets done.
     */

    diminuto_delay(diminuto_frequency() * 10, !0);

    /*
     * Tell workload to exit.
     */

    ASSERT(kill(workloadpid, SIGINT) == 0);
    ASSERT(waitpid(workloadpid, &status, 0) == workloadpid);
    CHECKPOINT("main %d workload %d status %d\n", listensocket, workloadpid, status);
    ASSERT(WIFEXITED(status));
    ASSERT(WEXITSTATUS(status) == 0);

    /*
     * Tell instance to exit.
     */

    ASSERT(kill(instancepid, SIGINT) == 0);
    ASSERT(waitpid(instancepid, &status, 0) == instancepid);
    CHECKPOINT("main %d instance %d status %d\n", listensocket, instancepid, status);
    ASSERT(WIFEXITED(status));
    ASSERT(WEXITSTATUS(status) == 0);

    /*
     * Clean up.
     */

    CHECKPOINT("main exiting\n");

    ASSERT(diminuto_ipc_close(activationsocket) >= 0);
    ASSERT(diminuto_ipc_close(instancesocket) >= 0);
    ASSERT(diminuto_ipcl_remove(path) >= 0);
    ASSERT(diminuto_ipc_close(listensocket) >= 0);

    EXIT();
}
