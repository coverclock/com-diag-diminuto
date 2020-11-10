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
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include "../src/diminuto_ipcl.h"

char * diminuto_ipcl_canonicalize(const char * path, char * buffer, size_t size)
{
    char * result = (char *)0;
    diminuto_local_buffer_t local = { '\0', };
    size_t length = 0;

    if (diminuto_fs_canonicalize(path, local, sizeof(local)) < 0) {
        /* Canonicalization failed. */
    } else if ((length = strlen(local)) < 2) {
        /* Must be at least "/x". */
        errno = EINVAL;
        diminuto_perror(path);
    } else if (local[length - 1] == '/') {
        /* Canonicalization gave us "/path/" but no file name. */
        errno = EINVAL;
        diminuto_perror(path);
    } else if (length >= size) {
        /* Too long. */
        errno = ENAMETOOLONG;
        diminuto_perror(path);
    } else {
        strncpy(buffer, local, size);
        buffer[size - 1] = '\0';
        result = buffer;
    }

    return result;
}

int diminuto_ipcl_identify(struct sockaddr * sap, char * pathp, size_t size)
{
    int rc = 0;

    if (sap->sa_family != AF_UNIX) {
        rc = -60;
    } else if (pathp == (char *)0) {
        /* Do nothing. */
    } else {
        strncpy(pathp, ((struct sockaddr_un *)sap)->sun_path, size);
        pathp[size - 1] = '\0';
    }

    return rc;
}

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

int diminuto_ipcl_source(int fd, const char * path)
{
    int rc = fd;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    sa.sun_family = AF_UNIX;
    strncpy(sa.sun_path, path, sizeof(sa.sun_path));

    if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipcl_source: bind");
        rc = -61;
    }

    return rc;
}

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
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path)) < 0) {
        diminuto_ipcl_close(fd);
    } else if (listen(fd, backlog) < 0) {
        diminuto_perror("diminuto_ipcl_stream_provider_base: listen");
        diminuto_ipcl_close(fd);
        rc = -62;
    } else {
        /* Do nothing. */
    }

    return rc;
}

int diminuto_ipcl_stream_accept_generic(int fd, char * pathp, size_t psize)
{
    int rc = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = accept(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipcl_accept_generic: accept");
        rc = -63;
    } else {
        diminuto_ipcl_identify((struct sockaddr *)&sa, pathp, psize);
    }

    return rc;
}

int diminuto_ipcl_stream_consumer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    sa.sun_family = AF_UNIX;

    if ((rc = fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_stream_consumer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path)) < 0) {
        diminuto_ipcl_close(fd);
    } else if (connect(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipcl_stream_consumer_base: connect");
        diminuto_ipcl_close(fd);
        rc = -64;
    } else {
        /* Do nothing. */
    }

    return rc;
}

int diminuto_ipcl_datagram_peer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if ((rc = fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipcl_datagram_peer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_ipcl_close(fd);
    } else if ((rc = diminuto_ipcl_source(fd, path)) < 0) {
        diminuto_ipcl_close(fd);
    } else {
        /* Do nothing. */
    }

    return rc;
}

ssize_t diminuto_ipcl_datagram_receive_generic(int fd, void * buffer, size_t size, char * pathp, size_t psize, int flags)
{
    ssize_t total = -1;
    struct sockaddr_un sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        diminuto_ipcl_identify((struct sockaddr *)&sa, pathp, psize);
    } else if ((errno != EINTR) && (errno != EAGAIN)) { 
        diminuto_perror("diminuto_ipcl_datagram_receive_generic: recvfrom");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

ssize_t diminuto_ipcl_datagram_send_generic(int fd, const void * buffer, size_t size, const char * path, int flags)
{
    ssize_t total = -1;
    struct sockaddr_un sa = { 0, };;
    struct sockaddr * sap = (struct sockaddr *)0;
    socklen_t length = 0;

    if (path != (const char *)0) {
        length = sizeof(sa);
        sa.sun_family = AF_INET;
        strncpy(sa.sun_path, path, sizeof(sa.sun_path));
        sap = (struct sockaddr *)&sa;
    } else {
        length = 0;
        sap = (struct sockaddr *)0;
    }

    if ((total = sendto(fd, buffer, size, flags, sap, length)) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipcl_datagram_send_generic: sendto");
    } else {
        /* Do nothing: interrupt, timeout, or poll. */
    }

    return total;
}
