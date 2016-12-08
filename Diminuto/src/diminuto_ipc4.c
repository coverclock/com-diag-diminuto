/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * Ported from the Desperado::Service class.
 */

#include "diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
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
 * GLOBALS
 ******************************************************************************/

const diminuto_ipv4_t DIMINUTO_IPC4_UNSPECIFIED = 0x00000000UL;

const diminuto_ipv4_t DIMINUTO_IPC4_LOOPBACK = 0x7f000001UL;

const diminuto_ipv4_t DIMINUTO_IPC4_LOOPBACK2 = 0x7f000002UL;

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/*
 * If the socket is in the IPv4 family, we return the IPv4 address as is. If it
 * is unspecified, which can legitimately occur when a datagram send or receive
 * is performed against a stream socket (done typically to send an urgent data
 * byte out of band), we return zeros. Anything  else is an error.
 */
int diminuto_ipc4_identify(struct sockaddr * sap, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int result = 0;

    if (sap->sa_family == AF_INET) {
        if (addressp != (diminuto_ipv4_t *)0) {
            *addressp = ntohl(((struct sockaddr_in *)sap)->sin_addr.s_addr);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in *)sap)->sin_port);
        }
    } else if (sap->sa_family == AF_UNSPEC) {
        result = 0;
        if (addressp != (diminuto_ipv4_t *)0) {
            *addressp = 0;
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = 0;
        }
    } else {
        result = -1;
        errno = EINVAL;
        diminuto_perror("diminuto_ipc4_identify: sa_family");
    }

    return result;
}

/*******************************************************************************
 * STRING TO ADDRESS AND VICE VERSA
 ******************************************************************************/

diminuto_ipv4_t * diminuto_ipc4_addresses(const char * hostname)
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

diminuto_ipv4_t diminuto_ipc4_address(const char * hostname)
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

const char * diminuto_ipc4_dotnotation(diminuto_ipv4_t address, char * buffer, size_t length)
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

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

int diminuto_ipc4_stream_provider_generic(diminuto_ipv4_t address, diminuto_port_t port, int backlog)
{
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    int fd;

    if (backlog > SOMAXCONN) { backlog = SOMAXCONN; }

    sa.sin_family = AF_INET;
    /* INADDR_ANY is all zeros so this is overly paranoid. */
    sa.sin_addr.s_addr = (address == 0) ? INADDR_ANY : htonl(address);
    sa.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc4_provider_backlog: socket");
    } else if (diminuto_ipc4_set_reuseaddress(fd, !0) != fd) {
        diminuto_perror("diminuto_ipc4_provider_backlog: diminuto_ipc4_set_reuseadddress");
        diminuto_ipc4_close(fd);
        fd = -2;
    } else if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc4_provider_backlog: bind");
        diminuto_ipc4_close(fd);
        fd = -3;
    } else if (listen(fd, backlog) < 0) {
        diminuto_perror("diminuto_ipc4_provider_backlog: listen");
        diminuto_ipc4_close(fd);
        fd = -4;
    }

    return fd;
}

int diminuto_ipc4_stream_provider(diminuto_port_t port)
{
    return diminuto_ipc4_stream_provider_generic(0, port, SOMAXCONN);
}

int diminuto_ipc4_stream_accept(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    struct sockaddr sa = { 0 };
    socklen_t length = sizeof(sa);
    int newfd;

    if ((newfd = accept(fd, &sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc4_accept: accept");
        newfd = -1;
    } else {
        diminuto_ipc4_identify(&sa, addressp, portp);
    }
   
    return newfd;
}

int diminuto_ipc4_stream_consumer(diminuto_ipv4_t address, diminuto_port_t port)
{
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    int fd;
    int rc;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc4_consumer: socket");
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) < 0) {
        diminuto_perror("diminuto_ipc4_consumer: connect");
        diminuto_ipc4_close(fd);
        fd = -2;
    }

    return fd;
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

int diminuto_ipc4_datagram_peer(diminuto_port_t port)
{
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    int fd;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc4_datagram_peer: socket");
    } else if (diminuto_ipc4_set_reuseaddress(fd, !0) != fd) {
        diminuto_perror("diminuto_ipc4_datagram_peer: diminuto_ipc4_set_reuseaddress");
        diminuto_ipc4_close(fd);
        fd = -2;
    } else if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc4_datagram_peer: bind");
        diminuto_ipc4_close(fd);
        fd = -3;
    }

    return fd;
}

ssize_t diminuto_ipc4_datagram_receive_flags(int fd, void * buffer, size_t size, diminuto_ipv4_t * addressp, diminuto_port_t * portp, int flags)
{
    ssize_t total;
    struct sockaddr sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((total = recvfrom(fd, buffer, size, flags, &sa, &length)) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        diminuto_ipc4_identify(&sa, addressp, portp);
    } else if ((errno != EINTR) && (errno != EAGAIN)) { 
        diminuto_perror("diminuto_ipc4_datagram_receive_flags: recvfrom");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

ssize_t diminuto_ipc4_datagram_send_flags(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port, int flags)
{
    ssize_t total;
    struct sockaddr_in sa;
    struct sockaddr * sap;
    socklen_t length;

    if (port > 0) {
        length = sizeof(sa);
        memset(&sa, 0, length);
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(address);
        sa.sin_port = htons(port);
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
        diminuto_perror("diminuto_ipc4_datagram_send_flags: sendto");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipc4_nearend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = getsockname(fd, &sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc4_nearend: getsockname");
    } else {
        diminuto_ipc4_identify(&sa, addressp, portp);
    }

    return rc;
}

int diminuto_ipc4_farend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = getpeername(fd, &sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc4_farend: getpeername");
    } else {
        diminuto_ipc4_identify(&sa, addressp, portp);
    }

    return rc;
}
