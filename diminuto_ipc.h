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

uint32_t diminuto_ipc_address_index(const char * hostname, size_t index);

uint32_t diminuto_ipc_address(const char * hostname);

uint16_t diminuto_ipc_port(const char * service, const char * protocol);

const char * diminuto_ipc_dotnotation(uint32_t address, char * buffer, size_t length);

int diminuto_ipc_provider_backlog(uint16_t port, int backlog);

int diminuto_ipc_provider(uint16_t port);

int diminuto_ipc_accept_address(int fd, uint32_t * addressp);

int diminuto_ipc_accept(int fd);

int diminuto_ipc_consumer(uint32_t address, uint16_t port);

int diminuto_ipc_peer(uint16_t port);

int diminuto_ipc_shutdown(int fd);

int diminuto_ipc_close(int fd);

int diminuto_ipc_set_status(int fd, int enable, long mask);

int diminuto_ipc_set_option(int fd, int enable, int option);

int diminuto_ipc_set_nonblocking(int fd, int enable);

int diminuto_ipc_set_reuseaddress(int fd, int enable);

int diminuto_ipc_set_keepalive(int fd, int enable);

int diminuto_ipc_set_debug(int fd, int enable);

int diminuto_ipc_set_linger(int fd, int enable);

#endif
