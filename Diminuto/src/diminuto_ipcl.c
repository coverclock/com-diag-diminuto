/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the IPC feature for Local sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the IPC feature for Local (UNIX Domain)
 * sockets.
 */

#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_fs.h"
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
#include <net/if.h>

char * diminuto_ipcl_path2string(const char * path, char * buffer, size_t size)
{
    char * result = (char *)0;
    diminuto_local_buffer_t local = { '\0', };
    size_t length = 0;

    if (diminuto_fs_canonicalize(path, local, sizeof(local)) < 0) {
        /* Canonicalization failed. */
    } else if ((length = strlen(local)) < 2) {
        /* Must be at least "/x". */
        errno = EINVAL;
        diminuto_perror(path);
    } else if (local[length - 1] == '/') {
        /* Canonicalization gave us "/path/" but no file name. */
        errno = EINVAL;
        diminuto_perror(path);
    } else if (length >= size) {
        /* Too long. */
        errno = ENAMETOOLONG;
        diminuto_perror(path);
    } else {
        strncpy(buffer, local, size);
        buffer[size - 1] = '\0';
        result = buffer;
    }

    return result;
}

int diminuto_ipcl_compare(const char * path1p, const char * path2p)
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
