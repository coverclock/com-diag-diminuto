/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the IPC feature for IPv4.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the IPC feature for IPv4.
 */

#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
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
#include <net/if.h>
#include <pthread.h>
#include "../src/diminuto_ipc4.h"

/*******************************************************************************
 * EXTRACTORS
 ******************************************************************************/

/*
 * If the socket is in the IPv4 family, we return the IPv4 address as is. If it
 * is anything else, we return zeros.
 */
int diminuto_ipc4_identify(struct sockaddr * sap, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc = 0;

    if (sap->sa_family == AF_INET) {
        if (addressp != (diminuto_ipv4_t *)0) {
            *addressp = ntohl(((struct sockaddr_in *)sap)->sin_addr.s_addr);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(((struct sockaddr_in *)sap)->sin_port);
        }
    } else {
        if (addressp != (diminuto_ipv4_t *)0) {
            *addressp = 0;
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

diminuto_ipv4_t * diminuto_ipc4_addresses(const char * hostname)
{
    diminuto_ipv4_t * addresses = (diminuto_ipv4_t *)0;
    struct  hostent * hostp = (struct hostent *)0;
    struct in_addr inaddr = { 0, };
    size_t index = 0;
    size_t limit = 0;

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
    struct  hostent * hostp = (struct hostent *)0;
    struct in_addr inaddr = { 0, };

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

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

const char * diminuto_ipc4_dotnotation(diminuto_ipv4_t address, void * buffer, size_t length)
{
    char * string = (char *)0;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    string = (char *)buffer;
    if (length > 0) {
        struct in_addr inaddr = { 0, };
        char * dot = (char *)0;

        string[0] = '\0';
        inaddr.s_addr = htonl(address);
        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            dot = inet_ntoa(inaddr); /* NOT THREAD SAFE! */
            strncpy(string, dot, length);
        DIMINUTO_CRITICAL_SECTION_END;
        string[length - 1] = '\0';
    }

    return string;
}

const char * diminuto_ipc4_address2type(diminuto_ipv4_t address)
{
    const char * result = (const char *)0;

    if (diminuto_ipc4_is_limitedbroadcast(&address)) {
        result = "limited-broadcast";
    } else if (diminuto_ipc4_is_loopback(&address)) {
        result = "loopback";
    } else if (diminuto_ipc4_is_private(&address)) {
        result = "private";
    } else if (diminuto_ipc4_is_multicast(&address)) {
        result = "multicast";
    } else {
        result = "public";
    }

    return result;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipc4_nearend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc = -1;
    struct sockaddr_in sa = { 0,  };
    socklen_t length = sizeof(sa);

    if ((rc = getsockname(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc4_nearend: getsockname");
    } else {
        diminuto_ipc4_identify((struct sockaddr *)&sa, addressp, portp);
    }

    return rc;
}

int diminuto_ipc4_farend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc = -1;
    struct sockaddr_in sa = { 0, };
    socklen_t length = sizeof(sa);

    if ((rc = getpeername(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc4_farend: getpeername");
    } else {
        diminuto_ipc4_identify((struct sockaddr *)&sa, addressp, portp);
    }

    return rc;
}

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

diminuto_ipv4_t * diminuto_ipc4_interface(const char * interface)
{
    diminuto_ipv4_t * rp = (diminuto_ipv4_t *)0;
    struct ifaddrs * ifa = (struct ifaddrs *)0;
    struct ifaddrs * ip = (struct ifaddrs *)0;
    diminuto_ipv4_t * vp = (diminuto_ipv4_t *)0;
    size_t vs = sizeof(diminuto_ipv4_t);
    int rc = -1;

    do {

        if ((rc = getifaddrs(&ifa)) < 0) {
            diminuto_perror("diminuto_ipc4_interface: getifaddrs");
            break;
        }

        for (ip = ifa; ip != (struct ifaddrs *)0; ip = ip->ifa_next) {
            if (ip->ifa_name == (char *)0) {
                continue;
            } else if (ip->ifa_addr == (struct sockaddr *)0) {
                continue;
            } else if (ip->ifa_addr->sa_family != AF_INET) {
                continue;
            } else if (strncmp(ip->ifa_name, interface, NAME_MAX) != 0) {
                continue;
            } else {
                vs += sizeof(diminuto_ipv4_t);
            }
        }

        rp = (diminuto_ipv4_t *)malloc(vs);
        vp = rp;

        for (ip = ifa; ip != (struct ifaddrs *)0; ip = ip->ifa_next) {
            if (ip->ifa_name == (char *)0) {
                continue;
            } else if (ip->ifa_addr == (struct sockaddr *)0) {
                continue;
            } else if (ip->ifa_addr->sa_family != AF_INET) {
                continue;
            } else if (strncmp(ip->ifa_name, interface, NAME_MAX) != 0) {
                continue;
            } else {
                *(vp++) = ntohl(((struct sockaddr_in *)(ip->ifa_addr))->sin_addr.s_addr);
            }
        }

        *vp = 0;

    } while (0);

    if (ifa != (struct ifaddrs *)0) {
        freeifaddrs(ifa);
    }

    return rp;
}

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

int diminuto_ipc4_source(int fd, diminuto_ipv4_t address, diminuto_port_t port)
{
    int rc = fd;
    struct sockaddr_in sa = { 0, };
    socklen_t length = sizeof(sa);

    sa.sin_family = AF_INET;
    /* INADDR_ANY is all zeros so this is overly paranoid. */
    sa.sin_addr.s_addr = (address == 0) ? INADDR_ANY : htonl(address);
    sa.sin_port = htons(port);

    if ((rc = bind(fd, (struct sockaddr *)&sa, length)) < 0) {
        diminuto_perror("diminuto_ipc4_source: bind");
    }

    return rc;
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

int diminuto_ipc4_stream_provider_base(diminuto_ipv4_t address, diminuto_port_t port, const char * interface, int backlog, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if ((rc = fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc4_stream_provider_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipc4_stream_provider_base: injector");
        diminuto_ipc4_close(fd);
    } else if ((rc = diminuto_ipc4_source(fd, address, port)) < 0) {
        diminuto_ipc4_close(fd);
    } else if ((rc = diminuto_ipc_set_interface(fd, interface)) < 0) {
        diminuto_ipc4_close(fd);
    } else if ((rc = listen(fd, (backlog < 0) ? diminuto_maximumof(int) : backlog)) < 0) { 
        diminuto_perror("diminuto_ipc4_stream_provider_base: listen");
        diminuto_ipc4_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

int diminuto_ipc4_stream_accept_generic(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    int rc = -1;
    struct sockaddr_in sa = { 0, };
    socklen_t length = sizeof(sa);

    if ((rc = accept(fd, (struct sockaddr *)&sa, &length)) >= 0) {
        diminuto_ipc4_identify((struct sockaddr *)&sa, addressp, portp);
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc4_accept_generic: accept");
    }

    return rc;
}

int diminuto_ipc4_stream_consumer_base(diminuto_ipv4_t address, diminuto_port_t port, diminuto_ipv4_t address0, diminuto_port_t port0, const char * interface, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;
    struct sockaddr_in sa = { 0, };
    socklen_t length = sizeof(sa);

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_port = htons(port);

    if ((rc = fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc4_stream_consumer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipc4_stream_consumer_base: injector");
        diminuto_ipc4_close(fd);
    } else if ((rc = diminuto_ipc4_source(fd, address0, port0)) < 0) {
        diminuto_ipc4_close(fd);
    } else if ((rc = diminuto_ipc_set_interface(fd, interface)) < 0) {
        diminuto_ipc4_close(fd);
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) >= 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        diminuto_ipc4_close(fd);
    } else {
        diminuto_perror("diminuto_ipc4_stream_consumer_base: connect");
        diminuto_ipc4_close(fd);
    }

    return (rc < 0) ? rc : fd;
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

int diminuto_ipc4_datagram_peer_base(diminuto_ipv4_t address, diminuto_port_t port, const char * interface, diminuto_ipc_injector_t * functionp, void * datap)
{
    int rc = -1;
    int fd = -1;

    if ((rc = fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc4_datagram_peer_base: socket");
    } else if ((functionp != (diminuto_ipc_injector_t *)0) && ((rc = (*functionp)(fd, datap)) < 0)) {
        diminuto_perror("diminuto_ipc4_datagram_peer_base: injector");
        diminuto_ipc4_close(fd);
    } else if ((rc = diminuto_ipc4_source(fd, address, port)) < 0) {
        diminuto_ipc4_close(fd);
    } else if ((rc = diminuto_ipc_set_interface(fd, interface)) < 0) {
        diminuto_ipc4_close(fd);
    } else {
        /* Do nothing. */
    }

    return (rc < 0) ? rc : fd;
}

ssize_t diminuto_ipc4_datagram_receive_generic(int fd, void * buffer, size_t size, diminuto_ipv4_t * addressp, diminuto_port_t * portp, int flags)
{
    ssize_t total = -1;
    struct sockaddr_in sa = { 0, };
    socklen_t length = sizeof(sa);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) > 0) {
        diminuto_ipc4_identify((struct sockaddr *)&sa, addressp, portp);
    } else if (total == 0) {
        /* Do nothing ("orderly shutdown"). */
    } else if (errno == EINTR) { 
        /* Do nothing. */
    } else if (errno == EAGAIN) { 
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc4_datagram_receive_generic: recvfrom");
    }

    return total;
}

ssize_t diminuto_ipc4_datagram_send_generic(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port, int flags)
{
    ssize_t total = -1;
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
        diminuto_perror("diminuto_ipc4_datagram_send_generic: sendto");
    }

    return total;
}
