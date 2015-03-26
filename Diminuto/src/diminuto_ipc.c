/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * Ported from the Desperado::Service class.
 */

#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
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

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

static int identify(struct sockaddr * sap, diminuto_ipv4_t * addressp, diminuto_port_t * portp) {
    int result = 0;

    if (sap->sa_family == AF_INET) {
        if (addressp != (diminuto_ipv4_t *)0) {
            *addressp = ntohl(((struct sockaddr_in *)sap)->sin_addr.s_addr);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in *)sap)->sin_port);
        }
    } else {
        result = -1;
        errno = EINVAL;
        diminuto_perror("diminuto_ipc: sa_family");
    }

    return result;
}

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

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
        /* Do nothing: success. */
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

int diminuto_ipc_set_linger(int fd, diminuto_ticks_t ticks)
{
    struct linger opt;

    if (ticks > 0) {
        opt.l_onoff = !0;
        opt.l_linger = (ticks + diminuto_frequency() - 1) / diminuto_frequency();
    } else {
        opt.l_onoff = 0;
        opt.l_linger = 0;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt)) < 0) {
        diminuto_perror("diminuto_ipc_set_linger: setsockopt");
        fd = -1;
    }

    return fd;
}

/*******************************************************************************
 * STRING TO ADDRESS AND VICE VERSA
 ******************************************************************************/

diminuto_ipv4_t * diminuto_ipc_addresses(const char * hostname)
{
	diminuto_ipv4_t * addresses = (diminuto_ipv4_t *)0;
    struct  hostent * hostp;
    struct in_addr inaddr;
    size_t index;
    size_t limit;

    if (inet_aton(hostname, &inaddr)) {
        addresses = (diminuto_ipv4_t *)malloc(sizeof(diminuto_ipv4_t) * 2);
        addresses[0] = ntohl(inaddr.s_addr);
        addresses[1] = 0;
    } else if ((hostp = gethostbyname(hostname)) == (struct hostent *)0) {
        /* Do nothing: no host entry. */
    } else if (hostp->h_addrtype != AF_INET) {
        /* Do nothing: not in the IP address family. */
    } else {
        for (limit = 0; hostp->h_addr_list[limit] != 0; ++limit) {
            continue;
        }
        if (limit > 0) {
        	addresses = (diminuto_ipv4_t *)malloc(sizeof(diminuto_ipv4_t) * (limit + 1));
        	for (index = 0; index < limit; ++index) {
        		memset(&inaddr, 0, sizeof(inaddr));
         		memcpy(&inaddr, hostp->h_addr_list[index], (hostp->h_length < (int)sizeof(inaddr)) ? hostp->h_length : sizeof(inaddr));
        		addresses[index] = ntohl(inaddr.s_addr);
        	}
        	addresses[index] = 0;
        }
    }

    return addresses;
}

diminuto_ipv4_t diminuto_ipc_address(const char * hostname)
{
	diminuto_ipv4_t address = (diminuto_ipv4_t)0;
    struct  hostent * hostp;
    struct in_addr inaddr;

    if (inet_aton(hostname, &inaddr)) {
        address = ntohl(inaddr.s_addr);
    } else if ((hostp = gethostbyname(hostname)) == (struct hostent *)0) {
        /* Do nothing: no host entry. */
    } else if (hostp->h_addrtype != AF_INET) {
        /* Do nothing: not in the IP address family. */
    } else {
        memset(&inaddr, 0, sizeof(inaddr));
        memcpy(&inaddr, hostp->h_addr_list[0], (hostp->h_length < (int)sizeof(inaddr)) ? hostp->h_length : sizeof(inaddr));
        address = ntohl(inaddr.s_addr);
    }

    return address;
}

const char * diminuto_ipc_dotnotation(diminuto_ipv4_t address, char * buffer, size_t length)
{
    struct in_addr inaddr;
    char * dot;

    if (length > 0) {
        inaddr.s_addr = htonl(address);
        dot = inet_ntoa(inaddr);
        strncpy(buffer, dot, length);
        buffer[length - 1] = '\0';
    }

    return buffer;
}

diminuto_port_t diminuto_ipc_port(const char * service, const char * protocol)
{
    diminuto_port_t port = 0;
    struct servent * portp;
    size_t length;
    diminuto_unsigned_t temp;
    const char * end;

    if ((portp = getservbyname(service, protocol)) != (struct servent *)0) {
        port = ntohs(portp->s_port);
    } else if (*(end = diminuto_number_unsigned(service, &temp)) != '\0') {
        /* Do nothing: might be an unknown service. */
    } else if (temp > (diminuto_port_t)~0) {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc_port: magnitude");
        port = 0;
    } else {
        port = temp;
    }

    return port;
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

int diminuto_ipc_stream_provider_backlog(diminuto_port_t port, int backlog)
{
    struct sockaddr_in sa;
    socklen_t length;
    int fd;

    if (backlog > SOMAXCONN) { backlog = SOMAXCONN; }

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
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

int diminuto_ipc_stream_provider(diminuto_port_t port)
{
    return diminuto_ipc_stream_provider_backlog(port, SOMAXCONN);
}

int diminuto_ipc_stream_accept(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    struct sockaddr sa;
    socklen_t length;
    int newfd;

    length = sizeof(sa);
    if ((newfd = accept(fd, &sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc_accept: accept");
        newfd = -1;
    } else {
        identify(&sa, addressp, portp);
    }
   
    return newfd;
}

int diminuto_ipc_stream_consumer(diminuto_ipv4_t address, diminuto_port_t port)
{
    struct sockaddr_in sa;
    socklen_t length;
    int fd;
    int rc;

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);
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

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

int diminuto_ipc_datagram_peer(diminuto_port_t port)
{
    struct sockaddr_in sa;
    socklen_t length;
    int fd;

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
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

ssize_t diminuto_ipc_datagram_receive_flags(int fd, void * buffer, size_t size, diminuto_ipv4_t * addressp, diminuto_port_t * portp, int flags)
{
    ssize_t total;
    struct sockaddr sa;
    socklen_t length;

    length = sizeof(sa);
    memset(&sa, 0, length);

    if ((total = recvfrom(fd, buffer, size, flags, &sa, &length)) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        identify(&sa, addressp, portp);
    } else if ((errno != EINTR) && (errno != EAGAIN)) { 
        diminuto_perror("diminuto_ipc_datagram_receive_flags: recvfrom");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

ssize_t diminuto_ipc_datagram_send_flags(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port, int flags)
{
    ssize_t total;
    struct sockaddr_in sa;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_port = htons(port);

    if ((total = sendto(fd, buffer, size, flags, (struct sockaddr *)&sa, sizeof(sa))) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipc_datagram_send_flags: sendto");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipc_nearend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr sa;
    socklen_t length;

    length = sizeof(sa);
    if ((rc = getsockname(fd, &sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc_nearend: getsockname");
    } else {
        identify(&sa, addressp, portp);
    }

    return rc;
}

int diminuto_ipc_farend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr sa;
    socklen_t length;

    length = sizeof(sa);
    if ((rc = getpeername(fd, &sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc_farend: getpeername");
    } else {
        identify(&sa, addressp, portp);
    }

    return rc;
}
