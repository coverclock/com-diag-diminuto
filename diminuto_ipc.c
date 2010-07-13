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
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int diminuto_ipc_provider_backlog(uint16_t port, int backlog)
{
    struct sockaddr_in sa;
    int rc;

    if (backlog > SOMAXCONN) { backlog = SOMAXCONN; }

    memset(&sa, 0, sizeof(sa));
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd >= 0) {
        rc = diminuto_ipc_set_reuseaddress(fd, !0);
        if (rc != fd) {
            fd = -2;
        } else {
            socklen_t length = sizeof(sa);
            rc = bind(fd, (struct sockaddr *)&sa, length);
            if (rc < 0) {
                diminuto_ipc_close(fd);
                fd = -3;
            } else {
                rc = listen(fd, backlog);
                if (rc < 0) {
                    diminuto_ipc_close(fd);
                    fd = -4;
                }
            }
        }
    }

    return fd;
}

int diminuto_ipc_provider(uint16_t port)
{
    return diminuto_ipc_provider_backlog(port, SOMAXCONN);
}

int diminuto_ipc_accept_address(int fd, uint32_t * addressp)
{
    struct sockaddr_in sa;
    int newfd;

    socklen_t length = sizeof(sa);
    newfd = accept(fd, (struct sockaddr *)&sa, &length);
    if (newfd < 0) {
        /* Do nothing: failed */
    } else if (length != sizeof(sa)) {
        /* Do nothing: size botched up somehow. */
    } else if (addressp == (uint32_t *)0) {
        /* Do nothing: no pointer. */
    } else {
        *addressp = ntohl(sa.sin_addr.s_addr);
    }
   
    return newfd;
}

int diminuto_ipc_accept(int fd)
{
    uint32_t address;

    return diminuto_ipc_accept_address(fd, address);
}

int diminuto_ipc_consumer(uint32_t address, uint16_t port)
{
    struct sockaddr_in sa;
    int fd;
    int rc;

    memset(&sa, 0, sizeof(sa));
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd >= 0) {
        socklen_t length = sizeof(sa);
        rc = connect(fd, (struct sockaddr *)&sa, length);
        if (rc < 0) {
            diminuto_ipc_close(fd);
            fd = -2;
        }
    }

    return fd;
}

int diminuto_ipc_peer(uint16_t port)
{
    struct sockaddr_in sa;
    int fd;
    int rc;

    memset(&sa, 0, sizeof(sa));
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd >= 0) {
        rc = diminuto_ipc_set_reuseaddress(fd, !0);
        if (rc != fd) {
            fd = -2;
        } else {
            socklen_t length = sizeof(sa);
            rc = bind(fd, (struct sockaddr *)&sa, length);
            if (rc < 0) {
                diminuto_ipc_close(fd);
                fd = -3;
            }
        }
    }

    return fd;
}

int diminuto_ipc_shutdown(int fd)
{
    return shutdown(fd, SHUT_RDWR);
}

int diminuto_ipc_close(int fd)
{
    int rc;

    rc = close(fd);
    if (rc >= 0) {
        rc = fd;
    }

    return rc;
}

int diminuto_ipc_set_status(int fd, int enable, long mask)
{
    long flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1) {
        flags = enable ? (flags | mask) : (flags & (~mask));
        flags = fcntl(fd, F_SETFL, flags);
        if (flags != -1) {
            flags = fd;
        }
    }

    return (int)flags;
}

int diminuto_ipc_set_option(int fd, int enable, int option)
{
    int rc;
    int on;

    rc = fd;
    if (rc >= 0) {
        on = enable ? 1 : 0;
        rc = setsockopt(fd, SOL_SOCKET, option, &on, sizeof(on));
        if (rc >= 0) {
            rc = fd;
        }
    }

    return rc;
}

int diminuto_ipc_set_nonblocking(int fd, int enable)
{
    return diminuto_ipc_set_status(fd, enable, FNDELAY);
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

uint32_t diminuto_ipc_address(const char * hostname, size_t index)
{
    uint32_t ipaddress = 0;
    struct  hostent * hostp;
    struct in_addr inaddr;
    size_t limit;
    size_t size;
    int rc;

    // Note that this is not reentrant. The reentrant version of
    // this call is not POSIX compliant. POSIX deprecates this
    // call but my current Linux build server (2.4) does not
    // implement the new POSIX calls.
 
    hostp = gethostbyname(hostname);
    if (hostp != 0) {
        if (hostp->h_addrtype == AF_INET) {
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
            }
        }
    } else {
        // gethostbyname should have done this for us, but we make
        // a list ditch effort anyway.
        rc = inet_aton(hostname, &inaddr);
        if (rc != 0) {
            ipaddress = ntohl(inaddr.s_addr);
        }
    }

    return ipaddress;
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
    unsigned long temp;
    const char * end;

    portp = getservbyname(service, protocol);
    if (portp != 0) {
        port = ntohs(portp->s_port);
    } else {
        temp = strtoul(service, &end, 0);
        if (*end != '\0') {
            /* Do nothing: not a valid numeric character. */
        } else if (temp > ~(uint16_t)0) {
            /* Do nothing: too large */
        } else {
            port = temp;
        }
    }

    return port;
}
