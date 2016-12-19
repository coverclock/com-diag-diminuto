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

int diminuto_ipc_set_interface(int fd, const char * ifname)
{
    struct ifreq intf = { 0 };
    socklen_t length = 0;

    if (ifname[0] != '\0') {
        strncpy(intf.ifr_name, ifname, sizeof(intf.ifr_name));
        intf.ifr_name[sizeof(intf.ifr_name) - 1] = '\0';
        length = sizeof(intf);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&intf, length) < 0) {
        diminuto_perror("diminuto_ipc_set_interface: setsockopt(SO_BINDTODEVICE)");
        fd = -1;
    } else {
        /* Do nothing: success. */
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
    if (setsockopt(fd, SOL_SOCKET, option, &onoff, sizeof(onoff)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ipc_set_option: must be root");
        fd = -1;
    } else {
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
