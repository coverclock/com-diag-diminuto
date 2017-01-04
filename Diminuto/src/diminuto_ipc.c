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
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <linux/limits.h>

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

int diminuto_ipc_set_value(int fd, int value, int option)
{
    if (setsockopt(fd, SOL_SOCKET, option, &value, sizeof(value)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ipc_set_value: setsockopt: must be root");
        fd = -8;
    } else {
        diminuto_perror("diminuto_ipc_set_value: setsockopt");
        fd = -9;
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

int diminuto_ipc_set_tcpoption(int fd, int value, int option)
{
    struct protoent *pp;

    if ((pp = getprotobyname("tcp")) == (struct protoent *)0) {
        diminuto_perror("diminuto_ipc_set_tcpoption: getprotobyname: tcp");
        fd = -11;
    } else if (setsockopt(fd, pp->p_proto, option, &value, sizeof(value)) < 0) {
        diminuto_perror("diminuto_ipc_set_tcpoption: setsockopt");
        fd = -12;
    } else {
        /* Do nothing. */
    }

    return fd;
}

int diminuto_ipc_set_ipv6option(int fd, int value, int option)
{
    if (setsockopt(fd, IPPROTO_IPV6, option, &value, sizeof(value)) < 0) {
        diminuto_perror("diminuto_ipc_set_ipv6option: setsockopt");
        fd = -15;
    }

    return fd;
}

int diminuto_ipc_set_nonblocking(int fd, int enable)
{
    return diminuto_ipc_set_status(fd, enable, O_NONBLOCK);
}

int diminuto_ipc_set_reuseaddress(int fd, int enable)
{
    return diminuto_ipc_set_boolean(fd, enable, SO_REUSEADDR);
}

int diminuto_ipc_set_keepalive(int fd, int enable)
{
    return diminuto_ipc_set_boolean(fd, enable, SO_KEEPALIVE);
}

int diminuto_ipc_set_debug(int fd, int enable)
{
    return diminuto_ipc_set_boolean(fd, enable, SO_DEBUG);
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
        fd = diminuto_ipc_set_value(fd, value = size, SO_SNDBUF);
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
        fd = diminuto_ipc_set_value(fd, value = size, SO_RCVBUF);
    }

    return fd;
}

int diminuto_ipc_set_nodelay(int fd, int enable)
{
    return diminuto_ipc_set_tcpoption(fd, !!enable, TCP_NODELAY);
}

int diminuto_ipc_set_quickack(int fd, int enable)
{
    return diminuto_ipc_set_tcpoption(fd, !!enable, TCP_QUICKACK);
}

int diminuto_ipc_set_ipv6only(int fd, int enable)
{
    return diminuto_ipc_set_ipv6option(fd, !!enable, IPV6_V6ONLY);
}

int diminuto_ipc_set_ipv6toipv4(int fd)
{
    return diminuto_ipc_set_ipv6option(fd, AF_INET, IPV6_ADDRFORM);
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
