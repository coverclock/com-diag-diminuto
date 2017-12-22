/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * Ported from the Desperado::Service class.
 */

#include "diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <asm/ioctls.h>
#include <linux/limits.h>
#include <linux/sockios.h>

/*******************************************************************************
 * STRING TO PORT AND VICE VERSA
 ******************************************************************************/

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
 * SOCKETS
 ******************************************************************************/

int diminuto_ipc_close(int fd)
{
    if (close(fd) < 0) {
        diminuto_perror("diminuto_ipc_close: close");
        fd = -2;
    }

    return fd;
}

int diminuto_ipc_shutdown(int fd)
{
    if (shutdown(fd, SHUT_RDWR) < 0) {
        diminuto_perror("diminuto_ipc_shutdown: shutdown");
        fd = -3;
    }

    return fd;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

int diminuto_ipc_set_status(int fd, int enable, long mask)
{
    long flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        diminuto_perror("diminuto_ipc_set_status: fcntl(F_GETFL)");
        fd = -6;
    } else if (fcntl(fd, F_SETFL, enable ? (flags|mask) : (flags&(~mask))) <0) {
        diminuto_perror("diminuto_ipc_set_status: fcntl(F_SETFL)");
        fd = -7;
    } else {
        /* Do nothing: success. */
    }

    return fd;
}

int diminuto_ipc_set_socket(int fd, int level, int option, int value)
{
    if (setsockopt(fd, level, option, &value, sizeof(value)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ipc_set_socket: setsockopt: must be root");
        fd = -8;
    } else {
        diminuto_perror("diminuto_ipc_set_socket: setsockopt");
        fd = -9;
    }

    return fd;
}

int diminuto_ipc_get_control(int fd, int option)
{
    int value = -2;

    if (ioctl(fd, option, &value) < 0) {
        diminuto_perror("diminuto_ipc_get_control: ioctl");
        value = -1;
    }

    return value;
}

/*******************************************************************************
 * OPTIONS
 ******************************************************************************/

int diminuto_ipc_set_interface(int fd, const char * interface)
{
    struct ifreq intf = { 0 };
    socklen_t length = 0;

    if (interface != (const char *)0) {
        if (interface[0] != '\0') {
            strncpy(intf.ifr_name, interface, sizeof(intf.ifr_name));
            intf.ifr_name[sizeof(intf.ifr_name) - 1] = '\0';
            length = sizeof(intf);
        }
        if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&intf, length) >= 0) {
            /* Do nothing. */
        } else if (errno == EPERM) {
            diminuto_perror("diminuto_ipc_set_interface: setsockopt: must be root");
            fd = -4;
        } else {
            diminuto_perror("diminuto_ipc_set_interface: setsockopt");
            fd = -5;
        }
    }

    return fd;
}

int diminuto_ipc_set_nonblocking(int fd, int enable)
{
    return diminuto_ipc_set_status(fd, enable, O_NONBLOCK);
}

int diminuto_ipc_set_reuseaddress(int fd, int enable)
{
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_REUSEADDR, !!enable);
}

int diminuto_ipc_set_keepalive(int fd, int enable)
{
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_KEEPALIVE, !!enable);
}

int diminuto_ipc_set_debug(int fd, int enable)
{
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_DEBUG, !!enable);
}

/*
 * https://github.com/coverclock/com-diag-desperadito/blob/master/Desperadito/inc/com/diag/desperado/generics.h
 */
static const int MAXIMUM_SIGNED_INT = (~((int)1 << ((sizeof(int) * 8) - 1)));

int diminuto_ipc_set_send(int fd, ssize_t size)
{
    int value;

    if (size < 0) {
        /* Do nothing. */
    } else if (size > MAXIMUM_SIGNED_INT) {
        fd = -13;
    } else {
        fd = diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_SNDBUF, value = size);
    }

    return fd;
}

int diminuto_ipc_set_receive(int fd, ssize_t size)
{
    int value;

    if (size < 0) {
        /* Do nothing. */
    } else if (size > MAXIMUM_SIGNED_INT) {
        fd = -14;
    } else {
        fd = diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_RCVBUF, value = size);
    }

    return fd;
}

int diminuto_ipc_set_linger(int fd, diminuto_ticks_t ticks)
{
    struct linger opt = { 0 };

    if (ticks > 0) {
        opt.l_onoff = !0;
        opt.l_linger = (ticks + diminuto_frequency() - 1) / diminuto_frequency();
    }
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt)) < 0) {
        diminuto_perror("diminuto_ipc_set_linger: setsockopt");
        fd = -10;
    }

    return fd;
}

int diminuto_ipc_set_nodelay(int fd, int enable)
{
    return diminuto_ipc_set_socket(fd, IPPROTO_TCP, TCP_NODELAY, !!enable);
}

int diminuto_ipc_set_quickack(int fd, int enable)
{
    return diminuto_ipc_set_socket(fd, IPPROTO_TCP, TCP_QUICKACK, !!enable);
}

int diminuto_ipc6_set_ipv6only(int fd, int enable)
{
    return diminuto_ipc_set_socket(fd, IPPROTO_IPV6, IPV6_V6ONLY, !!enable);
}

ssize_t diminuto_ipc_stream_get_available(int fd)
{
    return diminuto_ipc_get_control(fd, SIOCINQ);
}

ssize_t diminuto_ipc_stream_get_pending(int fd)
{
    return diminuto_ipc_get_control(fd, SIOCOUTQ);
}

/*
 * IPV6_ADDRFORM appears to have been deprecated in the latest pertinent
 * internet standard, RFC 3943. And perusing the implementation of it in
 * the 4.2 kernel code I had some WTF moments (e.g. UDP is supported but
 * the socket has to have TCP Established set). Seems pretty sketchy to me.
 */

int diminuto_ipc6_stream_set_ipv6toipv4(int fd)
{
    return diminuto_ipc_set_socket(fd, IPPROTO_TCP, IPV6_ADDRFORM, AF_INET);
}

int diminuto_ipc6_datagram_set_ipv6toipv4(int fd)
{
    return diminuto_ipc_set_socket(fd, IPPROTO_UDP, IPV6_ADDRFORM, AF_INET);
}

/*******************************************************************************
 * INJECTORS
 ******************************************************************************/

int diminuto_ipc_inject_defaults(int fd, void * datap)
{
    return diminuto_ipc_set_reuseaddress(fd, !0);
}

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

char ** diminuto_ipc_interfaces(void)
{
    char ** rp = (char **)0;
    struct ifaddrs * ifa = (struct ifaddrs *)0;
    struct ifaddrs * ip;
    char ** vp;
    char * np;
    size_t vs = sizeof(char *);
    size_t ns = 0;
    size_t rs;
    char ** cp;

    do {

        if (getifaddrs(&ifa) < 0) {
            diminuto_perror("diminuto_ipc_interfaces: getifaddrs");
            break;
        }

        for (ip = ifa; ip != (struct ifaddrs *)0; ip = ip->ifa_next) {
            if (ip->ifa_name != (char *)0) {
                vs += sizeof(char *);
                ns += strnlen(ip->ifa_name, NAME_MAX) + 1;
            }
        }

        rs = vs + ns;
        rp = (char **)malloc(rs);
        vp = rp;
        np = (char *)vp + vs;

        for (ip = ifa; ip != (struct ifaddrs *)0; ip = ip->ifa_next) {
            if (ip->ifa_name != (char *)0) {
                for (cp = rp; cp < vp; ++cp) {
                    if (strncmp(ip->ifa_name, *cp, NAME_MAX) == 0) {
                        break;
                    }
                }
                if (cp >= vp) {
                    *(vp++) = np;
                    ns = strnlen(ip->ifa_name, NAME_MAX);
                    strncpy(np, ip->ifa_name, ns);
                    np += ns;
                    *(np++) = '\0';
                }
            }
        }

        *vp = (char *)0;

#if 0
        diminuto_dump(stderr, rp, rs);
#endif

    } while (0);

    if (ifa != (struct ifaddrs *)0) {
        freeifaddrs(ifa);
    }

    return rp;
}
