/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the IPC feature.
 */

#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <linux/limits.h>
#include "../src/diminuto_ipc.h"

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

int diminuto_ipc_debug = 0;

/*******************************************************************************
 * STRING TO PORT AND VICE VERSA
 ******************************************************************************/

diminuto_port_t diminuto_ipc_port(const char * service, const char * protocol)
{
    diminuto_port_t port = 0;
    const char * sp = (const char *)0;
    struct servent * portp = (struct servent *)0;
    diminuto_unsigned_t temp = 0;
    const char * end = (const char *)0;

    /* Maintain compatibility with endpoint syntax. */
    if (*(sp = service) == ':') { sp += 1; }

    if ((portp = getservbyname(sp, protocol)) != (struct servent *)0) {
        port = ntohs(portp->s_port);
    } else if (*(end = diminuto_number_unsigned(sp, &temp)) != '\0') {
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

const char * diminuto_ipc_port2string(diminuto_port_t port, void * buffer, size_t length) {
    char * string = (char *)buffer;

    if (length > 0) {
        string[0] = '\0';
        (void)snprintf(string, length, "%u", port);
        string[length - 1] = '\0';
    }

    return string;
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
 * INTERROGATORS
 ******************************************************************************/

int diminuto_ipc_type(int fd)
{
    int rc = AF_UNSPEC;
    struct sockaddr sa = { 0,  };
    socklen_t length = sizeof(sa);

    if (getsockname(fd, (struct sockaddr *)&sa, &length) < 0) {
        diminuto_perror("diminuto_ipc_type: getsockname");
    } else {
        rc = sa.sa_family;
    }

    return rc;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

int diminuto_ipc_set_status(int fd, int enable, long mask)
{
    long flags = 0;

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
    int value = 0;

    if (ioctl(fd, option, &value) < 0) {
        diminuto_perror("diminuto_ipc_get_control: ioctl");
        value = -1;
    }

    return value;
}

diminuto_sticks_t diminuto_ipc_get_timestamp(int fd)
{
    struct timeval value = { 0, };
    diminuto_sticks_t ticks = -1;

    if (ioctl(fd, SIOCGSTAMP, &value) < 0) {
        diminuto_perror("diminuto_ipc_get_timestamp: ioctl");
    } else {
        ticks = diminuto_frequency_seconds2ticks(value.tv_sec, value.tv_usec, 1000000LL);
    }

    return ticks;
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

int diminuto_ipc_set_send(int fd, ssize_t size)
{
    static const int MAXIMUM = diminuto_maximumof(int);

    if (size < 0) {
        /* Do nothing. */
    } else if (size > MAXIMUM) {
        fd = diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_SNDBUF, MAXIMUM);
    } else {
        int value;
        fd = diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_SNDBUF, value = size);
    }

    return fd;
}

int diminuto_ipc_set_receive(int fd, ssize_t size)
{
    static const int MAXIMUM = diminuto_maximumof(int);

    if (size < 0) {
        /* Do nothing. */
    } else if (size > MAXIMUM) {
        fd = diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_RCVBUF, MAXIMUM);
    } else {
        int value;
        fd = diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_RCVBUF, value = size);
    }

    return fd;
}

int diminuto_ipc_set_linger(int fd, diminuto_ticks_t ticks)
{
    struct linger opt = { 0, };
    static const int MAXIMUM = diminuto_maximumof(int);

    if (diminuto_ipc_debug) {
        diminuto_log_emit("diminuto_ipc_set_linger: fd=%d ticks=0x%llx\n", fd, (unsigned long long)ticks);
    }

    if (ticks == 0) {
        opt.l_onoff = 0;
        opt.l_linger = 0;
    } else if (ticks <= (diminuto_maximumof(diminuto_ticks_t) - diminuto_frequency())) {
        opt.l_onoff = !0;
        ticks += diminuto_frequency();
        ticks -= 1;
        ticks /= diminuto_frequency();
        opt.l_linger = (ticks <= MAXIMUM) ? ticks : MAXIMUM;
    } else {
        opt.l_onoff = !0;
        opt.l_linger = MAXIMUM;
    }

    if (diminuto_ipc_debug) {
        diminuto_log_emit("diminuto_ipc_set_linger: l_onoff=%d l_linger=%d\n", opt.l_onoff, opt.l_linger);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt)) < 0) {
        diminuto_perror("diminuto_ipc_set_linger: setsockopt");
        fd = -10;
    }

    return fd;
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
    struct ifaddrs * ip = (struct ifaddrs *)0;
    char ** vp = (char **)0;
    char * np = (char *)0;
    size_t vs = sizeof(char *);
    size_t ns = 0;
    size_t rs = 0;
    char ** cp = (char **)0;

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

/*******************************************************************************
 * MESSAGING
 ******************************************************************************/

ssize_t diminuto_ipc_message_receive_generic(int fd, struct msghdr * message, int flags)
{
    ssize_t total = -1;

    if ((total = recvmsg(fd, message, flags)) > 0) {
        /* Do nothing. */
    } else if (total == 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc_receive: recvmsg");
    }

    return total;
}

ssize_t diminuto_ipc_message_send_generic(int fd, const struct msghdr * message, int flags)
{
    ssize_t total = -1;

    if ((total = sendmsg(fd, message, flags)) > 0) {
        /* Do nothing. */
    } else if (total == 0) {
        /* Do nothing. */
    } else if (errno == EINTR) {
        /* Do nothing. */
    } else if (errno == EAGAIN) {
        /* Do nothing. */
    } else if (errno == EWOULDBLOCK) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_ipc_send: sendmsg");
    }

    return total;
}
