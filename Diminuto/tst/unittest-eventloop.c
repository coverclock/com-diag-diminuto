/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_pool.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

static const char * diminuto_mux_set_name(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    return (setp == &muxp->read) ? "read" : (setp == &muxp->write) ? "write" : (setp == &muxp->accept) ? "accept" : (setp == &muxp->urgent) ? "urgent" : "other";
}

static void diminuto_mux_sigs_dump(sigset_t * sigs)
{
    int signum;

    for (signum = 1; signum < NSIG; ++signum) {
        if (sigismember(sigs, signum)) {
            DIMINUTO_LOG_DEBUG(" %d", signum);
        }
    }
}

static void diminuto_mux_fds_dump(fd_set * fds)
{
    int fd;
    int nfds;

    nfds = diminuto_fd_count();
    for (fd = 0; fd < nfds; ++fd) {
        if (FD_ISSET(fd, fds)) {
            DIMINUTO_LOG_DEBUG(" %d", fd);
        }
    }

}

static void diminuto_mux_set_dump(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    const char * name;

    name = diminuto_mux_set_name(muxp, setp);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.next=%d", muxp, name, setp->next);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.min=%d", muxp, name, setp->min);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.max=%d", muxp, name, setp->max);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.active=<", muxp, name); diminuto_mux_fds_dump(&setp->active); DIMINUTO_LOG_DEBUG(">");
    DIMINUTO_LOG_DEBUG("mux@%p: %s.ready=<", muxp, name); diminuto_mux_fds_dump(&setp->ready); DIMINUTO_LOG_DEBUG(">");
}

static void diminuto_mux_dump(diminuto_mux_t * muxp)
{
    int signum;

    DIMINUTO_LOG_DEBUG("mux@%p: read_or_accept=<", muxp); diminuto_mux_fds_dump(&muxp->read_or_accept); DIMINUTO_LOG_DEBUG(">");
    diminuto_mux_set_dump(muxp, &muxp->read);
    diminuto_mux_set_dump(muxp, &muxp->write);
    diminuto_mux_set_dump(muxp, &muxp->accept);
    diminuto_mux_set_dump(muxp, &muxp->urgent);
    DIMINUTO_LOG_DEBUG("mux@%p: mask=<", muxp); diminuto_mux_sigs_dump(&muxp->mask); DIMINUTO_LOG_DEBUG(">");
}

/*******************************************************************************
 * SERVICE PROVIDER
 ******************************************************************************/

typedef struct Buffer {
    diminuto_list_t link;
    size_t size;
    uint8_t payload[64];
} buffer_t;

static pid_t provider(diminuto_port_t * portp)
{
    pid_t pid;
    int xc;
    int listener;
    diminuto_mux_t mux;
    size_t nfds;
    int nready;
    diminuto_ticks_t timeout;
    int fd;
    ssize_t size;
    diminuto_pool_t pool;
    buffer_t * bufferp;
    uint8_t control;
    diminuto_list_t * list;
    diminuto_list_t * nodep;
    int done;
    intptr_t pointer;
    diminuto_ipv4_t address;
    diminuto_port_t port;
    int connects;
    int reads;
    int writes;
    int urgents;
    int disconnects;
    size_t input;
    size_t output;
    char dotnotation[sizeof("255.255.255.255")];


    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER starting.\n");

    listener = diminuto_ipc_stream_provider(0);
    if (listener < 0) {
         return -2;
    }
    diminuto_ipc_nearend(listener, (diminuto_ipv4_t *)0, portp);

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER listening %u.\n", *portp);

    pid = fork();
    if (pid != 0) {
        diminuto_ipc_close(listener);
        return pid;
    }
    pid = getpid();
    xc = 0;

    diminuto_mux_init(&mux);
    diminuto_mux_register_accept(&mux, listener);
    diminuto_pool_init(&pool, sizeof(*bufferp));
    timeout = diminuto_frequency() * 10;
    nfds = diminuto_fd_count();
    list = (diminuto_list_t *)diminuto_heap_malloc(sizeof(diminuto_list_t) * nfds);
    pointer = -1;
    for (fd = 0; fd < nfds; ++fd) {
        diminuto_list_datainit(&list[fd], (void *)pointer);
    }
    done = 0;
    connects = 0;
    disconnects = 0;
    reads = 0;
    writes = 0;
    urgents = 0;
    input = 0;
    output = 0;

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d running.\n", pid);

    while (!0) {
        nready = diminuto_mux_wait(&mux, timeout);
        if (nready > 0) {

            while ((fd = diminuto_mux_ready_urgent(&mux)) >= 0) {
                size = diminuto_ipc_datagram_receive_flags(fd, &control, sizeof(control), (diminuto_ipv4_t *)0, (diminuto_port_t *)0, MSG_OOB | MSG_DONTWAIT);
                if (size > 0) {
                    ++urgents;
                    done = !0;
                    break;
                } else if (size == 0) {
                    ++disconnects;
                    diminuto_ipc_farend(fd, &address, &port);
                    diminuto_mux_close(&mux, fd);
                    while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
                        bufferp = containerof(buffer_t, link, nodep);
                        diminuto_pool_free(bufferp);
                    }
                    diminuto_list_dataset(&list[fd], (void *)(pointer = -1));
                    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d disconnected %d %s:%u.\n", pid, fd, diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port);
                } else {
                    /* Do nothing. */
                }
            }

            if (done) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d exiting.\n", pid);
                break;
            }

            while ((fd = diminuto_mux_ready_accept(&mux)) >= 0) {
                fd = diminuto_ipc_stream_accept(fd, &address, &port);
                if (fd >= 0) {
                    ++connects;
                    diminuto_mux_register_read(&mux, fd);
                    diminuto_mux_register_write(&mux, fd);
                    diminuto_mux_register_urgent(&mux, fd);
                    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d connected %d %s:%u.\n", pid, fd, diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port);
                }
            }

            while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {
                bufferp = (buffer_t *)diminuto_pool_alloc(&pool);
                if (bufferp != (buffer_t *)0) {
                    size = diminuto_fd_read(fd, bufferp->payload, 1, sizeof(bufferp->payload));
                    if (size > 0) {
                        ++reads;
                        input += size;
                        bufferp->size = size;
                        diminuto_list_init(&bufferp->link);
                        diminuto_list_enqueue(&list[fd], &bufferp->link);
                        pointer = fd;
                        diminuto_list_dataset(&list[fd], (void *)pointer);
                    } else if (size == 0) {
                        ++disconnects;
                        diminuto_ipc_farend(fd, &address, &port);
                        diminuto_mux_close(&mux, fd);
                        diminuto_pool_free(bufferp);
                        while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
                            bufferp = containerof(buffer_t, link, nodep);
                            diminuto_pool_free(bufferp);
                        }
                        diminuto_list_dataset(&list[fd], (void *)(pointer = -1));
                        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d disconnected %d %s:%u.\n", pid, fd, diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port);
                    } else {
                        diminuto_pool_free(bufferp);
                    }
                }
            }

            while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {
                nodep = diminuto_list_dequeue(&list[fd]);
                if (nodep != (diminuto_list_t *)0) {
                    bufferp = containerof(buffer_t, link, nodep);
                    size = diminuto_fd_write(fd, bufferp->payload, bufferp->size, bufferp->size);
                    if (size > 0) {
                        ++writes;
                        output += size;
                        diminuto_pool_free(bufferp);
                    } else if (size == 0) {
                        ++disconnects;
                        diminuto_ipc_farend(fd, &address, &port);
                        diminuto_mux_close(&mux, fd);
                        diminuto_pool_free(bufferp);
                        while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
                            bufferp = containerof(buffer_t, link, nodep);
                            diminuto_pool_free(bufferp);
                        }
                        diminuto_list_dataset(&list[fd], (void *)(pointer = -1));
                        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d disconnected %d %s:%u.\n", pid, fd, diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port);
                    } else {
                        diminuto_pool_free(bufferp);
                    }
                }
            }

        } else if (nready == 0) {

            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d waiting.\n", pid);

        } else if (errno == EINTR) {

            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d resuming.\n", pid);

        } else {

            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d failing!\n", pid);
            diminuto_mux_dump(&mux);
            xc = 1;
            break;

        }
    }

    for (fd = 0; fd < nfds; ++fd) {
        while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
            bufferp = containerof(buffer_t, link, nodep);
            diminuto_pool_free(bufferp);
        }
        pointer = (intptr_t)diminuto_list_data(&list[fd]);
        if (pointer == fd) {
            diminuto_mux_close(&mux, fd);
        }
    }
    diminuto_pool_fini(&pool);
    diminuto_mux_unregister_accept(&mux, listener);
    diminuto_heap_free(list);

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d connects=%d disconnects=%d reads=%d input=%zu writes=%d output=%zu.\n", pid, connects, disconnects, reads, input, writes, output);

    exit(xc);
}

/*******************************************************************************
 * SERVICE CONSUMER
 ******************************************************************************/

static pid_t consumer(diminuto_ipv4_t provideraddress, diminuto_port_t providerport, size_t connections, size_t messages)
{
    pid_t pid;
    int * connection;
    diminuto_ipv4_t address;
    diminuto_port_t port;
    size_t ii;
    size_t jj;
    diminuto_ticks_t now;
    diminuto_ticks_t then;
    ssize_t output;
    ssize_t input;
    char dotnotation[sizeof("255.255.255.255")];

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER starting.\n");

    pid = fork();
    if (pid != 0) {
        return pid;
    }
    pid = getpid();

    connection = (int *)diminuto_heap_malloc(sizeof(int) * connections);

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d running.\n", pid);

    for (ii = 0; ii < connections; ++ii) {
        connection[ii] = diminuto_ipc_stream_consumer(provideraddress, providerport);
        if (connection[ii] >= 0) {
            diminuto_ipc_nearend(connection[ii], &address, &port);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d connected [%zu] %d %s:%u.\n", pid, ii, connection[ii], diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port);
        } else {
            exit(1);
        }
    }

    for (jj = 0; jj < messages; ++jj) {
        for (ii = 0; ii < connections; ++ii) {
            now = diminuto_time_clock();
            output = diminuto_fd_write(connection[ii], &now, sizeof(now), sizeof(now));
            if (output != sizeof(now)) {
                diminuto_ipc_nearend(connection[ii], &address, &port);
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d write [%zu] %d %s:%u (%zd!=%zu)!\n", pid, ii, connection[ii], diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port, output, sizeof(now));
                exit(2);
            }
            input = diminuto_fd_read(connection[ii], &then, sizeof(then), sizeof(then));
            if (input != sizeof(then)) {
                diminuto_ipc_nearend(connection[ii], &address, &port);
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d read [%zu] %d %s:%u (%zd!=%zu)!\n", pid, ii, connection[ii], diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port, input, sizeof(then));
                exit(3);
            }
            if (now != then) {
                diminuto_ipc_nearend(connection[ii], &address, &port);
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d compare [%zu] %d %s:%u (%lld!=%lld)!\n", pid, ii, connection[ii], diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port, now, then);
                exit(4);
            }
        }
    }

    for (ii = 0; ii < connections; ++ii) {
        diminuto_ipc_nearend(connection[ii], &address, &port);
        diminuto_ipc_close(connection[ii]);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d disconnected [%zu] %d %s:%u.\n", pid, ii, connection[ii], diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)), port);
    }
    diminuto_heap_free(connection);

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "CONSUMER %d exiting.\n", pid);

    exit(0);
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

static const size_t CLIENTS = 1;
static const size_t CONNECTIONS = 1;
static const size_t MESSAGES = 1;

int main(int argc, char ** argv)
{
    diminuto_ipv4_t address;
    diminuto_port_t port;
    pid_t service;
    pid_t * client;
    int control;
    static const uint8_t ACK = '\06';
    int status;
    size_t ii;
    size_t messages;
    size_t connections;
    size_t clients;

    SETLOGMASK();

    messages = MESSAGES;
    connections = CONNECTIONS;
    clients = CLIENTS;

    if (argc > 1) {
        messages = strtoul(argv[1], (char **)0, 0);
        if (argc > 2) {
            connections = strtoul(argv[1], (char **)0, 0);
            if (argc > 3) {
                clients = strtoul(argv[1], (char **)0, 0);
            }
        }
    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "main running %zu %zu %zu.\n", messages, connections, clients);

    address = diminuto_ipc_address("localhost");
    service = provider(&port);
    ASSERT(service >= 0);
    control = diminuto_ipc_stream_consumer(address, port);
    ASSERT(control >= 0);
    client = (pid_t *)diminuto_heap_calloc(clients, sizeof(pid_t));
    ASSERT(client != (pid_t *)0);

    for (ii = 0; ii < clients; ++ii) {
        client[ii] = consumer(address, port, connections, messages);
        ASSERT(client[ii] >= 0);
    }

    for (ii = 0; ii < clients; ++ii) {
        waitpid(client[ii], &status, 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "client %d exited %d %d.\n", client[ii], WIFEXITED(status), WEXITSTATUS(status));
        EXPECT(WIFEXITED(status) && (WEXITSTATUS(status) == 0));
    }

    diminuto_ipc_datagram_send_flags(control, &ACK, sizeof(ACK), 0, 0, MSG_OOB);

    waitpid(service, &status, 0);
    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "service %d exited %d %d.\n", service, WIFEXITED(status), WEXITSTATUS(status));
    EXPECT(WIFEXITED(status) && (WEXITSTATUS(status) == 0));

    diminuto_heap_free(client);

    EXIT();
}
