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
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#if defined(__arm__)
static const size_t TOTAL = 1024 * 1024 * 100;
#else
static const size_t TOTAL = 1024 * 1024 * 1024;
#endif

typedef struct Buffer {
    diminuto_list_t link;
    size_t size;
    uint8_t payload[64];
} buffer_t;

static pid_t provider(diminuto_port_t * portp)
{
    pid_t pid;
    int listener;
    diminuto_mux_t mux;
    size_t nfds;
    size_t nready;
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

    diminuto_mux_init(&mux);
    diminuto_mux_register_accept(&mux, listener);
    diminuto_pool_init(&pool, sizeof(*bufferp));
    timeout = diminuto_frequency() * 10;
    nfds = diminuto_fd_count();
    list = (diminuto_list_t *)malloc(sizeof(diminuto_list_t) * nfds);
    pointer = -1;
    for (fd = 0; fd < nfds; ++fd) {
        diminuto_list_datainit(&list[fd], (void *)pointer);
    }
    done = 0;

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d running.\n", pid);

    while (!0) {
        nready = diminuto_mux_wait(&mux, timeout);
        if (nready > 0) {

            while ((fd = diminuto_mux_ready_urgent(&mux)) >= 0) {
                size = diminuto_ipc_datagram_receive_flags(fd, &control, sizeof(control), (diminuto_ipv4_t *)0, (diminuto_port_t *)0, MSG_OOB | MSG_DONTWAIT);
                if (size > 0) {
                    done = !0;
                    break;
                } else if (size == 0) {
                    diminuto_mux_close(&mux, fd);
                    while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
                        bufferp = containerof(buffer_t, link, nodep);
                        diminuto_pool_free(bufferp);
                    }
                    diminuto_list_dataset(&list[fd], (void *)(pointer = -1));
                } else {
                    /* Do nothing. */
                }
            }

            if (done) {
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "PROVIDER %d exiting.\n", pid);
                break;
            }

            while ((fd = diminuto_mux_ready_accept(&mux)) >= 0) {
                fd = diminuto_ipc_stream_accept(fd, (diminuto_ipv4_t *)0, (diminuto_port_t *)0);
                if (fd >= 0) {
                    diminuto_mux_register_read(&mux, fd);
                    diminuto_mux_register_write(&mux, fd);
                    diminuto_mux_register_urgent(&mux, fd);
                } else {
                    diminuto_perror("diminuto_ipc_stream_accept");
                }
            }

            while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {
                bufferp = (buffer_t *)diminuto_pool_alloc(&pool);
                if (bufferp != (buffer_t *)0) {
                    bufferp->size = diminuto_fd_read(fd, bufferp->payload, 1, sizeof(bufferp->payload));
                    if (size > 0) {
                        diminuto_list_init(&bufferp->link);
                        diminuto_list_enqueue(&list[fd], &bufferp->link);
                        pointer = fd;
                        diminuto_list_dataset(&list[fd], (void *)pointer);
                    } else if (size == 0) {
                        diminuto_mux_close(&mux, fd);
                        diminuto_pool_free(bufferp);
                        while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
                            bufferp = containerof(buffer_t, link, nodep);
                            diminuto_pool_free(bufferp);
                        }
                        diminuto_list_dataset(&list[fd], (void *)(pointer = -1));
                    } else {
                        diminuto_perror("diminuto_fd_read");
                        diminuto_pool_free(bufferp);
                    }
                }
            }

            while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {
                nodep = diminuto_list_dequeue(&list[fd]);
                if (nodep != (diminuto_list_t *)0) {
                    bufferp = containerof(buffer_t, link, nodep);
                    size = diminuto_fd_write(fd, bufferp->payload, 1, sizeof(bufferp->payload));
                    if (size > 0) {
                    	diminuto_pool_free(bufferp);
                    } else if (size == 0) {
                        diminuto_mux_close(&mux, fd);
                        diminuto_pool_free(bufferp);
                        while ((nodep = diminuto_list_dequeue(&list[fd])) != (diminuto_list_t *)0) {
                            bufferp = containerof(buffer_t, link, nodep);
                            diminuto_pool_free(bufferp);
                        }
                        diminuto_list_dataset(&list[fd], (void *)(pointer = -1));
                    } else {
                        diminuto_perror("diminuto_fd_write");
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

    exit(0);
}

static void consumer(diminuto_port_t service, size_t sockets, size_t messages)
{

}

int main(int argc, char ** argv)
{
    diminuto_port_t port;

    SETLOGMASK();

    provider(&port);
    diminuto_delay(diminuto_frequency() * 60, !0);

    EXIT();
}
