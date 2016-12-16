/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_countof.h"
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/limits.h>

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

const diminuto_ipv6_t DIMINUTO_IPC6_UNSPECIFIED = { 0, 0, 0, 0, 0, 0, 0, 0 };

const diminuto_ipv6_t DIMINUTO_IPC6_LOOPBACK = { 0, 0, 0, 0, 0, 0, 0, 1 };

const diminuto_ipv6_t DIMINUTO_IPC6_LOOPBACK4 = { 0, 0, 0, 0, 0, 0xffff, ((127 << 8) + 0), ((0 << 8) + 1) };

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

void diminuto_ipc6_ntoh6_generic(diminuto_ipv6_t * addressp)
{
    size_t ii;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        addressp->u16[ii] = ntohs(addressp->u16[ii]);
    }
}

void diminuto_ipc6_hton6_generic(diminuto_ipv6_t * addressp)
{
    size_t ii;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        addressp->u16[ii] = htons(addressp->u16[ii]);
    }
}

/*
 * It is fortuitous (and probably by design) that the values for which we have
 * to check are the same whether in host byte order or network byte order.
 */
static inline int isipany(const diminuto_ipv6_t * addressp) {
    size_t ii;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        if (addressp->u16[ii] != 0x0000) {
            return 0;
        }
    }

    return !0;
}

/*
 * It is fortuitous (and probably by design) that the values for which we have
 * to check are the same whether in host byte order or network byte order.
 */
static inline int isipv4(const diminuto_ipv6_t * addressp)
{
    if (addressp->u16[0] != 0x0000) {
        /* Do nothing. */
    } else if (addressp->u16[1] != 0x0000) {
        /* Do nothing. */
    } else if (addressp->u16[2] != 0x0000) {
        /* Do nothing. */
    } else if (addressp->u16[3] != 0x0000) {
        /* Do nothing. */
    } else if (addressp->u16[4] != 0x0000) {
        /* Do nothing. */
    } else if (addressp->u16[5] != 0xffff) {
        /* Do nothing. */
    } else {
        return !0;
    }

    return 0;
}

static inline void ipv42ipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp)
{
    addressp->u16[0] = 0x0000;
    addressp->u16[1] = 0x0000;
    addressp->u16[2] = 0x0000;
    addressp->u16[3] = 0x0000;
    addressp->u16[4] = 0x0000;
    addressp->u16[5] = 0xffff;
    memcpy(&(addressp->u16[6]), &address, sizeof(address));
}

/*
 * If the socket is in the IPv6 family, we returned the IPv6 address as is. If
 * it is in the IPv4 family, we return the IPv4 address encapsulated in an
 * IPv6 address. If it is unspecified, which can legitimately occur when a
 * datagram send or receive is performed against a stream socket (done typically
 * to send an urgent data byte out of band), we return UNSPECIFIED. Anything
 * else is an error.
 */
int diminuto_ipc6_identify(struct sockaddr * sap, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int result = -1;

    if (sap->sa_family == AF_INET6) {
        result = 0;
         if (addressp != (diminuto_ipv6_t *)0) {
            memcpy(addressp->u16, ((struct sockaddr_in6 *)sap)->sin6_addr.s6_addr, sizeof(addressp->u16));
            diminuto_ipc6_ntoh6(addressp);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in6 *)sap)->sin6_port);
        }
    } else if (sap->sa_family == AF_INET) {
        result = 0;
        if (addressp != (diminuto_ipv6_t *)0) {
            ipv42ipv6(((struct sockaddr_in *)sap)->sin_addr.s_addr, addressp);
            diminuto_ipc6_ntoh6(addressp);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in *)sap)->sin_port);
        }
    } else if (sap->sa_family == AF_UNSPEC) {
        result = 0;
        if (addressp != (diminuto_ipv6_t *)0) {
            *addressp = DIMINUTO_IPC6_UNSPECIFIED;
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = 0;
        }
    } else {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc6_identify: sa_family");
    }

    return result;
}

/*******************************************************************************
 * ADDRESS FORMAT CONVERTORS
 ******************************************************************************/

int diminuto_ipc6_ipv62ipv4(diminuto_ipv6_t address, diminuto_ipv4_t * addressp)
{
    int result;

    if (!(result = isipv4(&address))) {
        /* Do nothing. */
    } else if (addressp == (diminuto_ipv4_t *)0) {
        /* Do nothing. */
    } else {
        diminuto_ipc6_hton6(&address);
        memcpy(addressp, &(address.u16[6]), sizeof(*addressp));
        *addressp = ntohl(*addressp);
    }

    return result;
}

void diminuto_ipc6_ipv42ipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp)
{
    ipv42ipv6(htonl(address), addressp);
    diminuto_ipc6_ntoh6(addressp);
}

/*******************************************************************************
 * STRING TO ADDRESS AND VICE VERSA
 ******************************************************************************/

/*
 * This ipc6 feature prefers IPv6 addresses if they are available. So we
 * try for those first, then we settle for IPv4, then as a last ditch
 * we try unspecified, which aren't likely to be useful, but at least
 * identify() will emit an error messages in that case.
 */
static struct addrinfo * resolve(const char * hostname)
{
    struct addrinfo hints = { 0 };
    struct addrinfo * infop;

    hints.ai_family = AF_INET6;
    infop = (struct addrinfo *)0;
    if ((getaddrinfo(hostname, (const char *)0, &hints, &infop)) != 0) {
        if (infop != (struct addrinfo *)0) {
            freeaddrinfo(infop);
            infop = (struct addrinfo *)0;
        }
        hints.ai_family = AF_INET;
        if ((getaddrinfo(hostname, (const char *)0, &hints, &infop)) != 0) {
            if (infop != (struct addrinfo *)0) {
                freeaddrinfo(infop);
                infop = (struct addrinfo *)0;
            }
            hints.ai_family = AF_UNSPEC;
            if ((getaddrinfo(hostname, (const char *)0, &hints, &infop)) != 0) {
                if (infop != (struct addrinfo *)0) {
                    freeaddrinfo(infop);
                    infop = (struct addrinfo *)0;
                }
            }
        }
    }

    return infop;
}

/*
 * getaddrinfo(3) will return multiple entries with the same address but
 * which differ in other respects that we don't care about in this
 * feature. So we try to filter those out under the assumption that such
 * entries with the same address are grouped consecutively. That's been
 * my experience so far. The function still works if they aren't so grouped,
 * but redundant addresses just won't be filtered out. The alternative I've
 * considered would have O(n^2) performance, so for now I'm sticking with
 * this.
 */
static int redundant(const struct addrinfo * prevp, const struct addrinfo * nextp)
{
    int result = 0;

    if (nextp->ai_addr == (struct sockaddr *)0) {
        result = !0;
    } else if ((nextp->ai_addr->sa_family != AF_INET6) && (nextp->ai_addr->sa_family != AF_INET)) {
        result = !0;
    } else if (prevp == (struct addrinfo *)0) {
        /* Do nothing. */
    } else if (prevp->ai_addr == (struct sockaddr *)0) {
        /* Do nothing. */
    } else if (prevp->ai_addr->sa_family != nextp->ai_addr->sa_family) {
        /* Do nothing. */
    } else if ((nextp->ai_addr->sa_family == AF_INET6) && (memcmp(&(((struct sockaddr_in6 *)(nextp->ai_addr))->sin6_addr), &(((struct sockaddr_in6 *)(prevp->ai_addr))->sin6_addr), sizeof(struct in6_addr)) == 0)) {
        result = !0;
    } else if ((nextp->ai_addr->sa_family == AF_INET) && (memcmp(&(((struct sockaddr_in *)(nextp->ai_addr))->sin_addr), &(((struct sockaddr_in *)(prevp->ai_addr))->sin_addr), sizeof(struct in_addr)) == 0)) {
        result = !0;
    } else {
        /* Do nothing. */
    }

    return result;
}

diminuto_ipv6_t * diminuto_ipc6_addresses(const char * hostname)
{
    diminuto_ipv6_t * addresses = (diminuto_ipv6_t *)0;
    struct addrinfo * infop;
    struct addrinfo * nextp;
    struct addrinfo * prevp;
    size_t index;

    if ((infop = resolve(hostname)) != (struct addrinfo *)0) {
        index = 0;
        for (nextp = infop, prevp = (struct addrinfo *)0; nextp != (struct addrinfo *)0; prevp = nextp, nextp = prevp->ai_next) {
            if (!redundant(prevp, nextp)) {
                ++index;
            }
        }
        addresses = (diminuto_ipv6_t *)malloc(sizeof(diminuto_ipv6_t) * (index + 1));
        index = 0;
        for (nextp = infop, prevp = (struct addrinfo *)0; nextp != (struct addrinfo *)0; prevp = nextp, nextp = prevp->ai_next) {
            if (!redundant(prevp, nextp)) {
                diminuto_ipc6_identify(nextp->ai_addr, &(addresses[index]), (diminuto_port_t *)0);
                ++index;
            }
        }
        memcpy(&(addresses[index]), &DIMINUTO_IPC6_UNSPECIFIED, sizeof(addresses[index]));
        freeaddrinfo(infop);
    }

    return addresses;
}

diminuto_ipv6_t diminuto_ipc6_address(const char * hostname)
{
    diminuto_ipv6_t address = { 0 };
    struct addrinfo * infop;

    if ((infop = resolve(hostname)) != (struct addrinfo *)0) {
        diminuto_ipc6_identify(infop->ai_addr, &address, (diminuto_port_t *)0);
        freeaddrinfo(infop);
    }

    return address;
}

const char * diminuto_ipc6_colonnotation(diminuto_ipv6_t address, char * buffer, size_t length)
{
    if (length > 0) {
        struct in6_addr in6;
        char temporary[INET6_ADDRSTRLEN];

        diminuto_ipc6_hton6(&address);
        memcpy(in6.s6_addr, address.u16, sizeof(in6.s6_addr));
        inet_ntop(AF_INET6, &in6, temporary, sizeof(temporary));

        strncpy(buffer, temporary, length);
        buffer[length - 1] = '\0';
    }

    return buffer;
}

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

int diminuto_ipc6_source(int fd, diminuto_ipv6_t address, diminuto_port_t port)
{
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);

    sa.sin6_family = AF_INET6;
    /* in6addr_any is all zeros so this is overly paranoid. */
    if (isipany(&address)) {
        memcpy(sa.sin6_addr.s6_addr, &in6addr_any, sizeof(sa.sin6_addr.s6_addr));
    } else {
        diminuto_ipc6_hton6(&address);
        memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    }
    sa.sin6_port = htons(port);

    if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc6_source: bind");
        fd = -1;
    }

    return fd;
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

int diminuto_ipc6_stream_provider_specific(diminuto_ipv6_t address, diminuto_port_t port, int backlog)
{
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);
    int fd;

    if (backlog > SOMAXCONN) { backlog = SOMAXCONN; }

    sa.sin6_family = AF_INET6;
    /* in6addr_any is all zeros so this is overly paranoid. */
    if (isipany(&address)) {
        memcpy(sa.sin6_addr.s6_addr, &in6addr_any, sizeof(sa.sin6_addr.s6_addr));
    } else {
        diminuto_ipc6_hton6(&address);
        memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    }
    sa.sin6_port = htons(port);

    if ((fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_provider_specific: socket");
    } else if (diminuto_ipc6_set_reuseaddress(fd, !0) != fd) {
        diminuto_perror("diminuto_ipc6_provider_specific: diminuto_ipc6_set_reuseadddress");
        diminuto_ipc6_close(fd);
        fd = -2;
    } else if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc6_provider_specific: bind");
        diminuto_ipc6_close(fd);
        fd = -3;
    } else if (listen(fd, backlog) < 0) {
        diminuto_perror("diminuto_ipc6_provider_specific: listen");
        diminuto_ipc6_close(fd);
        fd = -4;
    } else {
        /* Do nothing. */
    }

    return fd;
}

int diminuto_ipc6_stream_provider(diminuto_port_t port)
{
    diminuto_ipv6_t any = { 0 };
    return diminuto_ipc6_stream_provider_specific(any, port, SOMAXCONN);
}

int diminuto_ipc6_stream_accept(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);
    int newfd;

    if ((newfd = accept(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc6_accept: accept");
        newfd = -1;
    } else {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);
    }
   
    return newfd;
}

int diminuto_ipc6_stream_consumer(diminuto_ipv6_t address, diminuto_port_t port)
{
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);
    int fd;
    int rc;

    sa.sin6_family = AF_INET6;
    diminuto_ipc6_hton6(&address);
    memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    sa.sin6_port = htons(port);

    if ((fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_consumer: socket");
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) < 0) {
        diminuto_perror("diminuto_ipc6_consumer: connect");
        diminuto_ipc6_close(fd);
        fd = -2;
    } else {
        /* Do nothing. */
    }

    return fd;
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

int diminuto_ipc6_datagram_peer_specific(diminuto_ipv6_t address, diminuto_port_t port)
{
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);
    int fd;

    sa.sin6_family = AF_INET6;
    /* in6addr_any is all zeros so this is overly paranoid. */
    if (isipany(&address)) {
        memcpy(sa.sin6_addr.s6_addr, &in6addr_any, sizeof(sa.sin6_addr.s6_addr));
    } else {
        diminuto_ipc6_hton6(&address);
        memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    }
    sa.sin6_port = htons(port);

    if ((fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_peer_specific: socket");
    } else if (diminuto_ipc6_set_reuseaddress(fd, !0) != fd) {
        diminuto_perror("diminuto_ipc6_peer_specific: diminuto_ipc6_set_reuseaddress");
        diminuto_ipc6_close(fd);
        fd = -2;
    } else if (bind(fd, (struct sockaddr *)&sa, length) < 0) {
        diminuto_perror("diminuto_ipc6_peer_specific: bind");
        diminuto_ipc6_close(fd);
        fd = -3;
    } else {
        /* Do nothing. */
    }

    return fd;
}

int diminuto_ipc6_datagram_peer(diminuto_port_t port)
{
    diminuto_ipv6_t any = { 0 };
	return diminuto_ipc6_datagram_peer_specific(any, port);
}

ssize_t diminuto_ipc6_datagram_receive_flags(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp, int flags)
{
    ssize_t total;
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) == 0) {
        /* Do nothing: "orderly shutdown" (not sure what that means in this context). */
    } else if (total > 0) {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);
    } else if ((errno != EINTR) && (errno != EAGAIN)) { 
        diminuto_perror("diminuto_ipc6_datagram_receive_flags: recvfrom");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

ssize_t diminuto_ipc6_datagram_send_flags(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port, int flags)
{
    ssize_t total;
    struct sockaddr_in6 sa = { 0 };
    socklen_t length;
    struct sockaddr * sap;

    if (port > 0) {
        length = sizeof(sa);
        sa.sin6_family = AF_INET6;
        diminuto_ipc6_hton6(&address);
        memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
        sa.sin6_port = htons(port);
        sap = (struct sockaddr *)&sa;
    } else {
        length = 0;
        sap = (struct sockaddr *)0;
    }

    if ((total = sendto(fd, buffer, size, flags, sap, length)) == 0) {
        /* Do nothing: "orderly shutdown" (not sure what that means in this context). */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipc6_datagram_send_flags: sendto");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipc6_nearend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = getsockname(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc6_nearend: getsockname");
    } else {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);
    }

    return rc;
}

int diminuto_ipc6_farend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr_in6 sa = { 0 };
    socklen_t length = sizeof(sa);

    if ((rc = getpeername(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc6_farend: getpeername");
    } else {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);
    }

    return rc;
}

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

diminuto_ipv6_t * diminuto_ipc6_interface(const char * interface)
{
    diminuto_ipv6_t * rp = (diminuto_ipv6_t *)0;
    struct ifaddrs * ifa = (struct ifaddrs *)0;
    struct ifaddrs * ip;
    diminuto_ipv6_t * vp;
    size_t vs = sizeof(diminuto_ipv6_t);
    int rc;

    do {

        if ((rc = getifaddrs(&ifa)) < 0) {
            diminuto_perror("diminuto_ipc6_interface: getifaddrs");
            break;
        }

        for (ip = ifa; ip != (struct ifaddrs *)0; ip = ip->ifa_next) {
            if (ip->ifa_name == (char *)0) {
                continue;
            } else if (ip->ifa_addr == (struct sockaddr *)0) {
                continue;
            } else if (ip->ifa_addr->sa_family != AF_INET6) {
                continue;
            } else if (strncmp(ip->ifa_name, interface, NAME_MAX) != 0) {
                continue;
            } else {
                vs += sizeof(diminuto_ipv6_t);
            }
        }

        rp = (diminuto_ipv6_t *)malloc(vs);
        vp = rp;

        for (ip = ifa; ip != (struct ifaddrs *)0; ip = ip->ifa_next) {
            if (ip->ifa_name == (char *)0) {
                continue;
            } else if (ip->ifa_addr == (struct sockaddr *)0) {
                continue;
            } else if (ip->ifa_addr->sa_family != AF_INET6) {
                continue;
            } else if (strncmp(ip->ifa_name, interface, NAME_MAX) != 0) {
                continue;
            } else {
                memcpy(vp, ((struct sockaddr_in6 *)(ip->ifa_addr))->sin6_addr.s6_addr, sizeof(diminuto_ipv6_t));
                diminuto_ipc6_ntoh6(vp);
                ++vp;
            }
        }

        memset(vp, 0, sizeof(diminuto_ipv6_t));

#if 0
        diminuto_dump(stderr, rp, vs);
#endif

    } while (0);

    if (ifa != (struct ifaddrs *)0) {
        freeifaddrs(ifa);
    }

    return rp;
}
