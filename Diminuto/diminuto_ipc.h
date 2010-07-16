/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_
#define _H_COM_DIAG_DIMINUTO_IPC_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>
#include <sys/types.h>

extern uint32_t diminuto_ipc_address_index(const char * hostname, size_t index);

extern uint32_t diminuto_ipc_address(const char * hostname);

extern uint16_t diminuto_ipc_port(const char * service, const char * protocol);

extern const char * diminuto_ipc_dotnotation(uint32_t address, char * buffer, size_t length);

extern int diminuto_ipc_stream_provider_backlog(uint16_t port, int backlog);

extern int diminuto_ipc_stream_provider(uint16_t port);

extern int diminuto_ipc_stream_accept_address(int fd, uint32_t * addressp);

extern int diminuto_ipc_stream_accept(int fd);

extern int diminuto_ipc_stream_consumer(uint32_t address, uint16_t port);

extern int diminuto_ipc_datagram_peer(uint16_t port);

extern int diminuto_ipc_shutdown(int fd);

extern int diminuto_ipc_close(int fd);

extern int diminuto_ipc_set_status(int fd, int enable, long mask);

extern int diminuto_ipc_set_option(int fd, int enable, int option);

extern int diminuto_ipc_set_nonblocking(int fd, int enable);

extern int diminuto_ipc_set_reuseaddress(int fd, int enable);

extern int diminuto_ipc_set_keepalive(int fd, int enable);

extern int diminuto_ipc_set_debug(int fd, int enable);

extern int diminuto_ipc_set_linger(int fd, int enable);

extern ssize_t diminuto_ipc_stream_read(int fd, void * buffer, size_t min, size_t max);

extern ssize_t diminuto_ipc_stream_write(int fd, const void * buffer, size_t min, size_t max);

extern ssize_t diminuto_ipc_datagram_receive_generic(int fd, void * buffer, size_t size, uint32_t * addressp, uint16_t * portp, int flags);

extern ssize_t diminuto_ipc_datagram_receive(int fd, void * buffer, size_t size, uint32_t * addressp, uint16_t * portp);

extern ssize_t diminuto_ipc_datagram_send_generic(int fd, const void * buffer, size_t size, uint32_t address, uint16_t port, int flags);

extern ssize_t diminuto_ipc_datagram_send(int fd, const void * buffer, size_t size, uint32_t address, uint16_t port);

#endif
