/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the IPC feature for IPv6.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the IPC feature for IPv6.
 */

#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_environment.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "../src/diminuto_ipc6.h"

/*******************************************************************************
 * ORDERERS
 ******************************************************************************/

void diminuto_ipc6_ntoh6_generic(diminuto_ipv6_t * addressp)
{
    size_t ii = 0;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        addressp->u16[ii] = ntohs(addressp->u16[ii]);
    }
}

void diminuto_ipc6_hton6_generic(diminuto_ipv6_t * addressp)
{
    size_t ii = 0;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        addressp->u16[ii] = htons(addressp->u16[ii]);
    }
}

/*******************************************************************************
 * CONVERTORS
 ******************************************************************************/

int diminuto_ipc6_ipv6toipv4(diminuto_ipv6_t address, diminuto_ipv4_t * addressp)
{
    int result = -1;

    if ((result = diminuto_ipc6_is_v4mapped(&address))) {
        diminuto_ipc6_hton6(&address);
        memcpy(addressp, &(address.u16[6]), sizeof(*addressp));
        *addressp = ntohl(*addressp);
    }

    return result;
}

void diminuto_ipc6_ipv4toipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp)
{
    memcpy(addressp, &DIMINUTO_IPC6_LOOPBACK4, sizeof(diminuto_ipv6_t) - sizeof(diminuto_ipv4_t));
    address = htonl(address);
    memcpy(&(addressp->u16[6]), &address, sizeof(diminuto_ipv4_t));
    addressp->u16[6] = ntohs(addressp->u16[6]);
    addressp->u16[7] = ntohs(addressp->u16[7]);
}

/*******************************************************************************
 * EXTRACTORS
 ******************************************************************************/

/*
 * If the socket is in the IPv6 family, we returned the IPv6 address as is. If
 * it is in the IPv4 family, we return the IPv4 address encapsulated in an
 * IPv6 address. If it is anything else, we return zeros.
 */
int diminuto_ipc6_identify(struct sockaddr * sap, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc = 0;

    if (sap->sa_family == AF_INET6) {
         if (addressp != (diminuto_ipv6_t *)0) {
            memcpy(addressp->u16, ((struct sockaddr_in6 *)sap)->sin6_addr.s6_addr, sizeof(addressp->u16));
            diminuto_ipc6_ntoh6(addressp);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in6 *)sap)->sin6_port);
        }
    } else if (sap->sa_family == AF_INET) {
        if (addressp != (diminuto_ipv6_t *)0) {
            diminuto_ipc6_ipv4toipv6(ntohl(((struct sockaddr_in *)sap)->sin_addr.s_addr), addressp);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in *)sap)->sin_port);
        }
    } else {
        if (addressp != (diminuto_ipv6_t *)0) {
            *addressp = DIMINUTO_IPC6_UNSPECIFIED;
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = 0;
        }
        rc = -1;
    }

    return rc;
}

/*******************************************************************************
 * RESOLVERS
 ******************************************************************************/

/**
 * Wrapper around getaddrinfo(3) that calls the funciton inside the environment
 * critical section.
 * @param node points to the nod name.
 * @param service points to the service name.
 * @param hints points to the addrinfo structure.
 * @param res points to the arrayinfo value-result pointer.
 ( @return the getaddrinfo(3) result.
 */
static int diminuto_ipc6_getaddrinfo(const char * node, const char * service, const struct addrinfo * hints, struct addrinfo ** res)
{
    int rc = -1;

    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        rc = getaddrinfo(node, service, hints, res);
    DIMINUTO_ENVIRONMENT_READER_END;

    return rc;
}

/*
 * This ipc6 feature prefers IPv6 addresses if they are available. So we
 * try for those first, then we settle for IPv4, then as a last ditch
 * we try unspecified, which aren't likely to be useful, but at least
 * identify() will emit an error messages in that case.
 */
static struct addrinfo * resolve(const char * hostname)
{
    struct addrinfo hints = { 0, };
    struct addrinfo * infop = (struct addrinfo *)0;

    hints.ai_family = AF_INET6;
    infop = (struct addrinfo *)0;
    if ((diminuto_ipc6_getaddrinfo(hostname, (const char *)0, &hints, &infop)) != 0) {
        if (infop != (struct addrinfo *)0) {
            freeaddrinfo(infop);
            infop = (struct addrinfo *)0;
        }
        hints.ai_family = AF_INET;
        if ((diminuto_ipc6_getaddrinfo(hostname, (const char *)0, &hints, &infop)) != 0) {
            if (infop != (struct addrinfo *)0) {
                freeaddrinfo(infop);
                infop = (struct addrinfo *)0;
            }
            hints.ai_family = AF_UNSPEC;
            if ((diminuto_ipc6_getaddrinfo(hostname, (const char *)0, &hints, &infop)) != 0) {
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
    struct addrinfo * infop = (struct addrinfo *)0;
    struct addrinfo * nextp = (struct addrinfo *)0;
    struct addrinfo * prevp = (struct addrinfo *)0;
    size_t index = 0;

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
    diminuto_ipv6_t address = { 0, };
    struct addrinfo * infop = (struct addrinfo *)0;

    if ((infop = resolve(hostname)) != (struct addrinfo *)0) {
        diminuto_ipc6_identify(infop->ai_addr, &address, (diminuto_port_t *)0);
        freeaddrinfo(infop);
    }

    return address;
}

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

char * diminuto_ipc6_colonnotation(diminuto_ipv6_t address, void * buffer, size_t length)
{
    char * string = (char *)0;

    string = (char *)buffer;
    if (length > 0) {
        struct in6_addr in6 = { 0, };
        char temporary[INET6_ADDRSTRLEN] = { '\0', };

        string[0] = '\0';
        diminuto_ipc6_hton6(&address);
        memcpy(in6.s6_addr, address.u16, sizeof(in6.s6_addr));
        inet_ntop(AF_INET6, &in6, temporary, sizeof(temporary));
        strncpy(string, temporary, length);
        string[length - 1] = '\0';
    }

    return string;
}

const char * diminuto_ipc6_address2type(diminuto_ipv6_t address)
{
    const char * result = (const char *)0;

    if (diminuto_ipc6_is_loopback(&address)) {
        result = "loopback";
    } else if (diminuto_ipc6_is_unicastglobal(&address)) {
        result = "global-unicast";
    } else if (diminuto_ipc6_is_uniquelocal(&address)) {
        result = "unique-local";
    } else if (diminuto_ipc6_is_linklocal(&address)) {
        result = "link-local";
    } else if (diminuto_ipc6_is_multicast(&address)) {
        result = "multicast";
    } else if (diminuto_ipc6_is_nat64wkp(&address)) {
        result = "nat64-wkp";
    } else if (diminuto_ipc6_is_isatap(&address)) {
        result = "atap";
    } else if (diminuto_ipc6_is_6to4(&address)) {
        result = "6to4";
    } else if (diminuto_ipc6_is_loopback4(&address)) {
        result = "v4-loopback";
    } else if (diminuto_ipc6_is_v4mapped(&address)) {
        result = "v4-mapped";
    } else if (diminuto_ipc6_is_v4compatible(&address)) {
        result = "v4-compatible";
    } else {
        result = "public";
    }

    return result;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipc6_nearend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc = -1;
    struct sockaddr_in6 sa = { 0, };
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
    int rc = -1;
    struct sockaddr_in6 sa = { 0, };
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
    struct ifaddrs * ip = (struct ifaddrs *)0;
    diminuto_ipv6_t * vp = (diminuto_ipv6_t *)0;
    size_t vs = sizeof(diminuto_ipv6_t);
    int rc = -1;

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

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

int diminuto_ipc6_source(int fd, diminuto_ipv6_t address, diminuto_port_t port)
{
    int rc = fd;
    struct sockaddr_in6 sa = { 0, };
    socklen_t length = sizeof(sa);

    sa.sin6_family = AF_INET6;
    /* in6addr_any is all zeros so this is overly paranoid. */
    if (diminuto_ipc6_is_unspecified(&address)) {
        memcpy(sa.sin6_addr.s6_addr, &in6addr_any, sizeof(sa.sin6_addr.s6_addr));
    } else {
        diminuto_ipc6_hton6(&address);
        memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    }
    sa.sin6_port = htons(port);

    if ((rc = bind(fd, (struct sockaddr *)&sa, length)) < 0) {
        diminuto_perror("diminuto_ipc6_source: bind");
    }

    return rc;
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

int diminuto_ipc6_stream_provider_base(diminuto_ipv6_t address, diminuto_port_t port, const char * interface, int backlog, diminuto_ipc_injector_t * functionp, void * datap)
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

    if ((rc = fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_stream_provider_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipc6_stream_provider_base: injector");
        diminuto_ipc6_close(fd);
    } else if ((rc = diminuto_ipc6_source(fd, address, port)) < 0) {
        diminuto_ipc6_close(fd);
    } else if ((rc = diminuto_ipc_set_interface(fd, interface)) < 0) {
        diminuto_ipc6_close(fd);
    } else if ((rc = listen(fd, backlog)) < 0) {
        diminuto_perror("diminuto_ipc6_stream_provider_base: listen");
        diminuto_ipc6_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

int diminuto_ipc6_stream_accept_generic(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc = -1;
    struct sockaddr_in6 sa = { 0, };
    socklen_t length = sizeof(sa);

    if ((rc = accept(fd, (struct sockaddr *)&sa, &length)) >= 0) {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc6_accept_generic: accept");
    }
   
    return rc;
}

int diminuto_ipc6_stream_consumer_base(diminuto_ipv6_t address, diminuto_port_t port, diminuto_ipv6_t address0, diminuto_port_t port0, const char * interface,  diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;
    struct sockaddr_in6 sa = { 0, };
    socklen_t length = sizeof(sa);

     sa.sin6_family = AF_INET6;
     diminuto_ipc6_hton6(&address);
     memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
     sa.sin6_port = htons(port);

    if ((rc = fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_stream_consumer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipc6_stream_consumer_base: injector");
        diminuto_ipc6_close(fd);
    } else if ((rc = diminuto_ipc6_source(fd, address0, port0)) < 0) {
        diminuto_ipc6_close(fd);
    } else if ((rc = diminuto_ipc_set_interface(fd, interface)) < 0) {
        diminuto_ipc6_close(fd);
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) >= 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
         diminuto_ipc6_close(fd);
    } else {
         diminuto_perror("diminuto_ipc6_stream_consumer_base: connect");
         diminuto_ipc6_close(fd);
    }

    return (rc < 0) ? rc : fd;
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

int diminuto_ipc6_datagram_peer_base(diminuto_ipv6_t address, diminuto_port_t port, const char * interface, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if ((rc = fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_peer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipc6_peer_base: injector");
        diminuto_ipc6_close(fd);
    } else if ((rc = diminuto_ipc6_source(fd, address, port)) < 0) {
        diminuto_ipc6_close(fd);
    } else if ((rc = diminuto_ipc_set_interface(fd, interface)) < 0) {
        diminuto_ipc6_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

ssize_t diminuto_ipc6_datagram_receive_generic(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp, int flags)
{
    ssize_t total = -1;
    struct sockaddr_in6 sa = { 0, };
    socklen_t length = sizeof(sa);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) > 0) {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);
    } else if (total == 0) {
        /* Do nothing ("orderly shutdown"). */
    } else if (errno == EINTR) { 
        /* Do nothing. */
    } else if (errno == EAGAIN) { 
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc6_datagram_receive_generic: recvfrom");
    }

    return total;
}

ssize_t diminuto_ipc6_datagram_send_generic(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port, int flags)
{
    ssize_t total = -1;
    struct sockaddr_in6 sa = { 0, };
    socklen_t length = 0;
    struct sockaddr * sap = (struct sockaddr *)0;

    if (port > 0) {
        length = sizeof(sa);
        sa.sin6_family = AF_INET6;
        diminuto_ipc6_hton6(&address);
        memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
        sa.sin6_port = htons(port);
        sap = (struct sockaddr *)&sa;
    }

    if ((total = sendto(fd, buffer, size, flags, sap, length)) > 0) {
        /* Do nothing. */
    } else if (total == 0) {
        /* Do nothing ("orderly shutdown"). */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc6_datagram_send_generic: sendto");
    }

    return total;
}
