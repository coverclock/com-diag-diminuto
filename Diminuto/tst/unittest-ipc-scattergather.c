/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the sendmsg(2) ancillary data capability.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test to evaluate whether Diminuto Lists and the
 * vector read/write/send/recv system calls can be used to implement
 * a scatter/gather I/O like one might use in a protocol stack.
 *
 * REFERENCES
 *
 * unix(7) man page
 *
 * socket(7) man page
 *
 * sendmsg(2) man page
 *
 * recvmsg(2) man page
 *
 * cmsg(3) man page
 *
 * readv(2) man page
 *
 * writev(2) man page
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum {
    NODES = 64,
    /*
     * <linux/uio.h> 5.4.0:
     * UIO_MAXIOV=1024
     * UIO_FASTIOV=8
     * POSIX 1003.1g 5.4.1.1 "UIO_MAXIOV shall be at least 16"
     */
    VECTOR = UIO_MAXIOV,
};

/*******************************************************************************
 * CLASSES
 ******************************************************************************/

typedef struct Node {
    diminuto_list_t list;
    size_t size;
} node_t;

static node_t * node_init(node_t * np)
{
    diminuto_list_t * rp = (diminuto_list_t *)0;

    np->size = 0;
    rp = diminuto_list_init(&(np->list));

    return (node_t *)rp;
}

static size_t node_enumerate(node_t * np)
{
    size_t nn = 0;
    diminuto_list_t * lp = (diminuto_list_t *)0;
    diminuto_list_t * pp = (diminuto_list_t *)0;

    lp = &(np->list);
    for (pp = diminuto_list_next(lp); pp != lp; pp = diminuto_list_next(pp)) {
        nn += 1;
    }

    return nn;    
}

/******************************************************************************/

typedef node_t segment_t;

static inline segment_t * segment_remove(segment_t * sp)
{
    return (segment_t *)diminuto_list_remove(&(sp->list));
}

/******************************************************************************/

typedef node_t record_t;

static inline segment_t * record_segment_prepend(record_t * rp, segment_t * sp)
{
    return (segment_t *)diminuto_list_push(&(rp->list), &(sp->list));
}

static inline segment_t * record_segment_append(record_t * rp, segment_t * sp)
{
    return (segment_t *)diminuto_list_enqueue(&(rp->list), &(sp->list));
}

static inline segment_t * record_segment_insert(segment_t * ep, segment_t * np)
{
    return (segment_t *)diminuto_list_splice(&(ep->list), &(np->list));
}

static inline segment_t * record_segment_replace(segment_t * op, segment_t * np)
{
    return (segment_t *)diminuto_list_replace(&(op->list), &(np->list));
}

/******************************************************************************/

typedef node_t pool_t;

static inline node_t * pool_node_get(pool_t * pp)
{
    return (node_t *)diminuto_list_dequeue(&(pp->list));
}

static inline node_t * pool_node_put(pool_t * pp, node_t * np)
{
    return (node_t *)diminuto_list_enqueue(&(pp->list), &(np->list));
}

/******************************************************************************/

static node_t nodes[NODES];

static pool_t pool = { DIMINUTO_LIST_NULLINIT(&pool.list), 0 };

static void pool_init(void)
{
    int ii = 0;

    for (ii = 0; ii < countof(nodes); ++ii) {
        (void)pool_node_put(&pool, node_init(&nodes[ii]));
    }
}

static segment_t * pool_allocate(size_t size)
{
    void * bp = (void *)0;
    node_t * np = (segment_t *)0;

    if ((bp = diminuto_buffer_malloc(size)) == (void *)0) {
        errno = ENOMEM;
        diminuto_perror("pool_allocate: diminuto_buffer_malloc");
    } else if ((np = pool_node_get(&pool)) == (node_t *)0) {
        diminuto_buffer_free(bp);
        bp = (void *)0;
    } else {
        np->list.data = bp;
        np->size = 0;
    }

    return np;
}

static void pool_free(segment_t * np)
{
    diminuto_buffer_free(np->list.data);
    np->list.data = (void *)0;
    np->size = 0;
    pool_node_put(&pool, np);
}

/******************************************************************************/

static struct iovec * record_gather(record_t * rp, struct iovec va[], size_t nn)
{
    diminuto_list_t * lp = (diminuto_list_t *)0;
    diminuto_list_t * pp = (diminuto_list_t *)0;
    size_t ii = 0;
    segment_t * sp = (segment_t *)0;

    if (va != (struct iovec *)0) {
        lp = &(rp->list);
        for (pp = diminuto_list_next(lp); pp != lp; pp = diminuto_list_next(pp)) {
            if (ii < nn) {
                va[ii].iov_base = (void *)pp->data;
                sp = (segment_t *)pp;
                va[ii].iov_len = sp->size;
                ii += 1;
            }
        }
    }

    return va;    
}

static ssize_t record_write(int fd, record_t * rp)
{
    ssize_t total = -1;
    size_t count = 0;
    struct iovec * vector = (struct iovec *)0;

    count = node_enumerate(rp);

    if (!((0 < count) && (count <= UIO_MAXIOV))) {
        errno = EINVAL;
        diminuto_perror("record_write: UIO_MAXIOV");
    } else if ((vector = record_gather(rp, (struct iovec *)alloca(count * sizeof(*vector)), count)) == (struct iovec *)0) {
        diminuto_perror("record_write: alloca");
    } else if ((total = writev(fd, record_gather(rp, vector, count), count)) < 0) {
        diminuto_perror("record_write: writev");
    } else {
        /* Do nothing. */
    }

    return total;
}

static int record_send(int fd, record_t * rp, diminuto_ipv4_t address, diminuto_port_t port)
{
    ssize_t total = 0;
    size_t count = 0;
    struct iovec * vector = (struct iovec *)0;
    struct msghdr message = { 0, };
    struct sockaddr_in sa = { 0, };
    struct sockaddr * sap = (struct sockaddr *)0;
    socklen_t length = 0;

    if (port > 0) {
        length = sizeof(sa);
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(address);
        sa.sin_port = htons(port);
        sap = (struct sockaddr *)&sa;
    }

    message.msg_name = sap;
    message.msg_namelen = length;
    message.msg_iovlen = node_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= UIO_MAXIOV))) {
        errno = EINVAL;
        diminuto_perror("record_write: UIO_MAXIOV");
    } else if ((message.msg_iov = record_gather(rp, (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov)), message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("record_write: alloca");
    } else if ((total = sendmsg(fd, &message, 0)) < 0) {
        diminuto_perror("record_send: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main(void)
{

    {
        int ii;

        TEST();

        pool_init();
        ASSERT(node_enumerate(&pool) == NODES);

        STATUS();
    }

    {
        node_t * np0;
        node_t * np1;
        node_t * np2;

        TEST();

        ASSERT(node_enumerate(&pool) == (NODES - 0));
        ASSERT((np0 = pool_node_get(&pool)) != (node_t *)0);
        ASSERT(node_enumerate(&pool) == (NODES - 1));
        ASSERT((np1 = pool_node_get(&pool)) != (node_t *)0);
        ASSERT(node_enumerate(&pool) == (NODES - 2));
        ASSERT((np2 = pool_node_get(&pool)) != (node_t *)0);
        ASSERT(node_enumerate(&pool) == (NODES - 3));
        ASSERT(pool_node_put(&pool, np0) == np0);
        ASSERT(node_enumerate(&pool) == (NODES - 2));
        ASSERT(pool_node_put(&pool, np1) == np1);
        ASSERT(node_enumerate(&pool) == (NODES - 1));
        ASSERT(pool_node_put(&pool, np2) == np2);
        ASSERT(node_enumerate(&pool) == (NODES - 0));

        STATUS();
    }

    {
        record_t * rp;

        TEST();

        STATUS();
    }

    EXIT();
}
