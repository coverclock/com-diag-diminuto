/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the IPC feature for IPv4.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the IPC feature for IPv4.
 */

#include "com/diag/diminuto/diminuto_ipcl.h"
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
#include <ifaddrs.h>
#include <linux/limits.h>
#include <net/if.h>

int diminuto_ipcl_compare(const char * path1p, const char * path2p)
{
}

const char * diminuto_ipcl_path2string(const char * path, char * buffer, size_t length)
{
}

int diminuto_ipcl_source(int fd, const char * path)
{
}

int diminuto_ipcl_stream_provider_base(const char * path, int backlog, diminuto_ipc_injector_t * functionp, void * datap)
{
}

int diminuto_ipcl_stream_accept_generic(int fd)
{
}

int diminuto_ipcl_stream_consumer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap)
{
}

int diminuto_ipcl_datagram_peer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap)
{
}

ssize_t diminuto_ipcl_datagram_receive_generic(int fd, void * buffer, size_t size, int flags)
{
}

ssize_t diminuto_ipcl_datagram_send_generic(int fd, const void * buffer, size_t size, const char * path, int flags)
{
}
