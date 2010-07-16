/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * Ported from the Desperado::Service class.
 */

#include "diminuto_ipc.h"
#include "diminuto_fd.h"
#include "diminuto_number.h"
#include "diminuto_log.h"
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int diminuto_ipc_close(int fd)
{
    if (close(fd) < 0) {
        diminuto_perror("diminuto_ipc_close: close");
        fd = -1;
    }

    return fd;
}

int diminuto_ipc_shutdown(int fd)
{
    if (shutdown(fd, SHUT_RDWR) < 0) {
        diminuto_perror("diminuto_ipc_shutdown: shutdown");
        fd = -1;
    }

    return fd;
}

int diminuto_ipc_set_status(int fd, int enable, long mask)
{
    long flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        diminuto_perror("diminuto_ipc_set_status: fcntl(F_GETFL)");
        fd = -1;
    } else if (fcntl(fd, F_SETFL, enable ? (flags|mask) : (flags&(~mask))) <0) {
        diminuto_perror("diminuto_ipc_set_status: fcntl(F_SETFL)");
        fd = -2;
    } else {
        /* Do nothing. */
    }

    return fd;
}

int diminuto_ipc_set_option(int fd, int enable, int option)
{
    int onoff;

    onoff = enable ? 1 : 0;
    if (setsockopt(fd, SOL_SOCKET, option, &onoff, sizeof(onoff)) < 0) {
        diminuto_perror("diminuto_ipc_set_option: setsockopt");
        fd = -1;
    }

    return fd;
}

int diminuto_ipc_set_nonblocking(int fd, int enable)
{
    return diminuto_ipc_set_status(fd, enable, O_NONBLOCK);
}

int diminuto_ipc_set_reuseaddress(int fd, int enable)
{
    return diminuto_ipc_set_option(fd, enable, SO_REUSEADDR);
}

int diminuto_ipc_set_keepalive(int fd, int enable)
{
    return diminuto_ipc_set_option(fd, enable, SO_KEEPALIVE);
}

int diminuto_ipc_set_debug(int fd, int enable)
{
    return diminuto_ipc_set_option(fd, enable, SO_DEBUG);
}

int diminuto_ipc_set_linger(int fd, int enable)
{
    return diminuto_ipc_set_option(fd, enable, SO_LINGER);
}

uint32_t diminuto_ipc_address_index(const char * hostname, size_t index)
{
    uint32_t ipaddress = 0;
    struct  hostent * hostp;
    struct in_addr inaddr;
    size_t limit;
    size_t size;

    if (inet_aton(hostname, &inaddr)) {
        ipaddress = ntohl(inaddr.s_addr);
    } else if ((hostp = gethostbyname(hostname)) == (struct hostent *)0) {
        /* Do nothing: no host entry. */
    } else if (hostp->h_addrtype != AF_INET) {
        /* Do nothing: not in the IP address family. */
    } else {
        for (limit = 0; hostp->h_addr_list[limit] != 0; ++limit) {
            continue;
        }
        if (index < limit) {
            memset(&inaddr, 0, sizeof(inaddr));
            if (hostp->h_length < (int)sizeof(inaddr)) {
                size = hostp->h_length;
            } else {
                size = sizeof(inaddr);
            }
            memcpy(&inaddr, hostp->h_addr_list[index], size);
            ipaddress = ntohl(inaddr.s_addr);
        } else {
            /* Do nothing: no address at provided index. */
        }
    }

    return ipaddress;
}

uint32_t diminuto_ipc_address(const char * hostname)
{
    return diminuto_ipc_address_index(hostname, 0);
}

const char * diminuto_ipc_dotnotation(uint32_t address, char * buffer, size_t length)
{
    struct in_addr inaddr;
    char * dot;

    inaddr.s_addr = htonl(address);
    dot = inet_ntoa(inaddr);
    strncpy(buffer, dot, length);

    return buffer;
}

uint16_t diminuto_ipc_port(const char * service, const char * protocol)
{
    uint16_t port = 0;
    struct servent * portp;
    size_t length;
    uint64_t temp;
    const char * end;

    if ((portp = getservbyname(service, protocol)) != (struct servent *)0) {
        port = ntohs(portp->s_port);
    } else if (*(end = diminuto_number_unsigned(service, &temp)) != '\0') {
        diminuto_perror("diminuto_ipc_port: diminuto_number_unsigned");
    } else if (temp > (uint16_t)~0) {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc_port: magnitude");
        port = 0;
    } else {
        port = temp;
    }

    return port;
}

int diminuto_ipc_stream_provider_backlog(uint16_t port, int backlog)
{
    struct sockaddr_in sa;
    socklen_t length;
    int fd;

    if (backlog > SOMAXCONN) { backlog = SOMAXCONN; }

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc_provider_backlog: socket");
    } else if (diminuto_ipc_set_reuseaddress(fd, !0) != fd) {
        diminuto_perror("diminuto_ipc_provider_backlog: diminuto_ipc_set_reuseadddress");
        diminuto_ipc_close(fd);
        fd = -2;
    } else if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc_provider_backlog: bind");
        diminuto_ipc_close(fd);
        fd = -3;
    } else if (listen(fd, backlog) < 0) {
        diminuto_perror("diminuto_ipc_provider_backlog: listen");
        diminuto_ipc_close(fd);
        fd = -4;
    }

    return fd;
}

int diminuto_ipc_stream_provider(uint16_t port)
{
    return diminuto_ipc_stream_provider_backlog(port, SOMAXCONN);
}

int diminuto_ipc_stream_accept_address(int fd, uint32_t * addressp)
{
    struct sockaddr_in sa;
    socklen_t length;
    int newfd;

    length = sizeof(sa);
    if ((newfd = accept(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc_accept_address: accept");
        newfd = -1;
    } else if (length != sizeof(sa)) {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc_accept_address: length");
        newfd = -2;
    } else {
        if (addressp != (uint32_t *)0) {
            *addressp = ntohl(sa.sin_addr.s_addr);
        }
    }
   
    return newfd;
}

int diminuto_ipc_stream_accept(int fd)
{
    uint32_t address;
    return diminuto_ipc_stream_accept_address(fd, &address);
}

int diminuto_ipc_stream_consumer(uint32_t address, uint16_t port)
{
    struct sockaddr_in sa;
    socklen_t length;
    int fd;
    int rc;

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc_consumer: socket");
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) < 0) {
        diminuto_perror("diminuto_ipc_consumer: connect");
        diminuto_ipc_close(fd);
        fd = -2;
    }

    return fd;
}

int diminuto_ipc_datagram_peer(uint16_t port)
{
    struct sockaddr_in sa;
    socklen_t length;
    int fd;

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc_peer: socket");
    } else if (diminuto_ipc_set_reuseaddress(fd, !0) != fd) {
        diminuto_perror("diminuto_ipc_peer: diminuto_ipc_set_reuseaddress");
        diminuto_ipc_close(fd);
        fd = -2;
    } else if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc_peer: bind");
        diminuto_ipc_close(fd);
        fd = -3;
    }

    return fd;
}

ssize_t diminuto_ipc_stream_read(int fd, void * buffer, size_t min, size_t max)
{
    return diminuto_fd_read(fd, buffer, min, max);
}

ssize_t diminuto_ipc_stream_write(int fd, const void * buffer, size_t min, size_t max)
{
    return diminuto_fd_write(fd, buffer, min, max);
}

ssize_t diminuto_ipc_datagram_receive_generic(int fd, void * buffer, size_t size, uint32_t * addressp, uint16_t * portp, int flags)
{
    ssize_t total;
    const char * bp;
    struct sockaddr_in sa;
    socklen_t length;

    bp = (char *) buffer;
    length = sizeof(sa);
    memset(&sa, 0, length);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) < 0) {
        if (errno == EINTR) {
            total = 0;
        } else {
            diminuto_perror("diminuto_ipc_datagram_receive_generic: recvfrom");
        }
    } else if (total != size) {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc_datagram_receive_generic: short");
    } else {
        if (addressp != (uint32_t *)0) {
            *addressp = ntohl(sa.sin_addr.s_addr);
        }
        if (portp != (uint16_t *)0) {
            *portp = ntohs(sa.sin_port);
        }
    }

    return total;
}

ssize_t diminuto_ipc_datagram_send_generic(int fd, const void * buffer, size_t size, uint32_t address, uint16_t port, int flags)
{
    ssize_t total;
    const char * bp;
    struct sockaddr_in sa;

    bp = (char *) buffer;

    memset(&sa, 0, sizeof(sa));
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if ((total = sendto(fd, buffer, size, flags, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
        diminuto_perror("diminuto_ipc_datagram_send_generic: sendto");
    } else if (total != size) {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc_datagram_send_generic: size");
    } else {
        /* Do nothing. */
    }

    return total;
}

ssize_t diminuto_ipc_datagram_receive(int fd, void * buffer, size_t size, uint32_t * addressp, uint16_t * portp)
{
    return diminuto_ipc_datagram_receive_generic(fd, buffer, size, addressp, portp, MSG_WAITALL);
}

ssize_t diminuto_ipc_datagram_send(int fd, const void * buffer, size_t size, uint32_t address, uint16_t port)
{
    return diminuto_ipc_datagram_send_generic(fd, buffer, size, address, port, 0);
}
