/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the IPC feature for Local sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the IPC feature for Local (UNIX Domain)
 * sockets.
 */

#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/un.h>
#include "../src/diminuto_ipcl.h"

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

const char DIMINUTO_IPCL_UNNAMED[1] = { '\0' };

/*******************************************************************************
 * EXTRACTORS
 ******************************************************************************/

int diminuto_ipcl_identify(struct sockaddr * sap, char * pathp, size_t size)
{
    int rc = 0;

    if (sap->sa_family != AF_UNIX) {
        rc = -1;
    } else if (pathp == (char *)0) {
        /* Do nothing. */
    } else {
        strncpy(pathp, ((struct sockaddr_un *)sap)->sun_path, size);
        pathp[size - 1] = '\0';
    }

    return rc;
}

/*******************************************************************************
 * RESOLVERS
 ******************************************************************************/

char * diminuto_ipcl_path(const char * path, char * buffer, size_t size)
{
    char * result = (char *)0;
    diminuto_local_t canonical = { '\0', };
    size_t length = 0;

    if (size <= 0) {
        /* Too long. */
        errno = ENAMETOOLONG;
        diminuto_perror(path);
    } else if (path[0] == '\0') {
        /* Unnamed Local socket. */
        buffer[0] = '\0';
        result = buffer;
    } else if (diminuto_fs_canonicalize(path, canonical, sizeof(canonical)) < 0) {
        /* Canonicalization failed. */
    } else if ((length = strlen(canonical)) < 2) {
        /* Must be at least "/x". */
        errno = EINVAL;
        diminuto_perror(path);
    } else if (canonical[length - 1] == '/') {
        /* Canonicalization gave us "/path/" but no file name. */
        errno = EINVAL;
        diminuto_perror(path);
    } else if (length >= size) {
        /* Too long. */
        errno = ENAMETOOLONG;
        diminuto_perror(path);
    } else {
        strncpy(buffer, canonical, size);
        buffer[size - 1] = '\0';
        result = buffer;
    }

    return result;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipcl_nearend(int fd, char * pathp, size_t psize)
{
    int rc = -1;
    struct sockaddr_un sa = { 0,  };
    socklen_t length = sizeof(sa);

    if ((rc = getsockname(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipcl_nearend: getsockname");
    } else {
        diminuto_ipcl_identify((struct sockaddr *)&sa, pathp, psize);
    }

    return rc;
}

int diminuto_ipcl_farend(int fd, char * pathp, size_t psize)
{
    int rc = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = getpeername(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipcl_farend: getpeername");
    } else {
        diminuto_ipcl_identify((struct sockaddr *)&sa, pathp, psize);
    }

    return rc;
}

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

int diminuto_ipcl_source(int fd, const char * path)
{
    int rc = fd;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    /*
     * We handle a NULL pointer as a special case
     * so that the canonicalize function can be used
     * directly as an argument. Similarly, a path
     * that is zero length represents an unnamed
     * socket, which is unnamed and unbound; this
     * is not an error.
     */

    if (path == (const char *)0) {

        errno = EINVAL;
        diminuto_perror("diminuto_ipcl_source: NULL");
        rc = -1;

    } else if (path[0] == '\0') {

        /* Do nothing. */

    } else {

        sa.sun_family = AF_UNIX;
        strncpy(sa.sun_path, path, sizeof(sa.sun_path));

        if ((rc = bind(fd, (struct sockaddr *)&sa, length)) < 0) {
            diminuto_perror("diminuto_ipcl_source: bind");
        }

    }

    return rc;
}

int diminuto_ipcl_remove(const char * path)
{
    int rc = 0;

    /*
     * We handle a NULL pointer as a special case
     * so that the canonicalize function can be used
     * directly as an argument.
     */

    if (path == (const char *)0) {

        errno = EINVAL;
        diminuto_perror("diminuto_ipcl_remove: NULL");
        rc = -1;

    } else if (unlink(path) < 0) {

        diminuto_perror("diminuto_ipcl_remove: unlink");
        rc = -1;

    } else {

        /* Do nothing. */

    }

    return rc;
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

int diminuto_ipcl_stream_provider_base(const char * path, int backlog, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if (backlog > SOMAXCONN) {
        backlog = SOMAXCONN;
    } else if (backlog < 0) {
        backlog = SOMAXCONN;
    } else {
        /* Do nothing. */
    }

    if ((rc = fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_stream_provider_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipcl_stream_provider_base: injector");
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path)) < 0) {
        diminuto_ipcl_close(fd);
    } else if ((rc = listen(fd, backlog)) < 0) {
        diminuto_perror("diminuto_ipcl_stream_provider_base: listen");
        diminuto_ipcl_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

int diminuto_ipcl_stream_accept_generic(int fd, char * pathp, size_t psize)
{
    int rc = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = accept(fd, (struct sockaddr *)&sa, &length)) >= 0) {
        diminuto_ipcl_identify((struct sockaddr *)&sa, pathp, psize);
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipcl_accept_generic: accept");
    }

    return rc;
}

int diminuto_ipcl_stream_consumer_base(const char * path, const char * path0, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, path, sizeof(sa.sun_path));

    if ((rc = fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_stream_consumer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipcl_stream_consumer_base: injector");
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path0)) < 0) {
        diminuto_ipcl_close(fd);
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) >= 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        diminuto_ipcl_close(fd);
    } else {
        diminuto_perror("diminuto_ipcl_stream_consumer_base: connect");
        diminuto_ipcl_close(fd);
    }

    return (rc < 0) ? rc : fd;
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

int diminuto_ipcl_datagram_peer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if ((rc = fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_datagram_peer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipcl_datagram_peer_base: injector");
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path)) < 0) {
        diminuto_ipcl_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

ssize_t diminuto_ipcl_datagram_receive_generic(int fd, void * buffer, size_t size, char * pathp, size_t psize, int flags)
{
    ssize_t total = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) > 0) {
        diminuto_ipcl_identify((struct sockaddr *)&sa, pathp, psize);
    } else if (total == 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipcl_datagram_receive_generic: recvfrom");
    }

    return total;
}

ssize_t diminuto_ipcl_datagram_send_generic(int fd, const void * buffer, size_t size, const char * path, int flags)
{
    ssize_t total = -1;
    struct sockaddr_un sa = { 0, };
    struct sockaddr * sap = (struct sockaddr *)0;
    socklen_t length = 0;

    if (path != (const char *)0) {
        length = sizeof(sa);
        sa.sun_family = AF_UNIX;
        strncpy(sa.sun_path, path, sizeof(sa.sun_path));
        sap = (struct sockaddr *)&sa;
    }

    if ((total = sendto(fd, buffer, size, flags, sap, length)) > 0) {
        /* Do nothing. */
    } else if (total ==  0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipcl_datagram_send_generic: sendto");
    }

    return total;
}

/*******************************************************************************
 * SEQUENTIAL PACKET SOCKETS
 ******************************************************************************/

int diminuto_ipcl_packet_provider_base(const char * path, int backlog, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if (backlog > SOMAXCONN) {
        backlog = SOMAXCONN;
    } else if (backlog < 0) {
        backlog = SOMAXCONN;
    } else {
        /* Do nothing. */
    }

    if ((rc = fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_packet_provider_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipcl_packet_provider_base: injector");
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path)) < 0) {
        diminuto_ipcl_close(fd);
    } else if ((rc = listen(fd, backlog)) < 0) {
        diminuto_perror("diminuto_ipcl_packet_provider_base: listen");
        diminuto_ipcl_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

int diminuto_ipcl_packet_consumer_base(const char * path, const char * path0, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, path, sizeof(sa.sun_path));

    if ((rc = fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_packet_consumer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipcl_packet_consumer_base: injector");
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path0)) < 0) {
        diminuto_ipcl_close(fd);
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) >= 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        diminuto_ipcl_close(fd);
    } else {
        diminuto_perror("diminuto_ipcl_stream_consumer_base: connect");
        diminuto_ipcl_close(fd);
    }

    return (rc < 0) ? rc : fd;
}

ssize_t diminuto_ipcl_packet_receive_generic(int fd, struct msghdr * message, int flags)
{
    ssize_t total = -1;

    if ((total = recvmsg(fd, message, flags)) > 0) {
        /* Do nothing. */
    } else if (total == 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipcl_packet_receive_generic: recvmsg");
    }

    return total;
}

ssize_t diminuto_ipcl_packet_send_generic(int fd, const struct msghdr * message, int flags)
{
    ssize_t total = -1;

    if ((total = sendmsg(fd, message, flags)) > 0) {
        /* Do nothing. */
    } else if (total == 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipcl_packet_receive_generic: sendmsg");
    }

    return total;
}
