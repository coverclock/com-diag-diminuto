/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static const int DEBUG = 0;
static const int VERBOSE = 0;

static const diminuto_port_t PORT = 0xfff0;
static const size_t TOTAL = 1024 * 1024 * 1024;

static const char * diminuto_mux_set_name(diminuto_mux_t * that, diminuto_mux_set_t * set)
{
	return (set == &that->read) ? "read" : (set == &that->write) ? "write" : "other";
}

static void diminuto_mux_sigs_dump(sigset_t * sigs, FILE * fp)
{
	int signum;

	for (signum = 1; signum < NSIG; ++signum) {
		if (sigismember(sigs, signum)) {
			fprintf(fp, " %d", signum);
		}
	}
}

static void diminuto_mux_fds_dump(fd_set * fds, FILE *fp)
{
	int fd;
	int nfds;

	nfds = sysconf(_SC_OPEN_MAX);
	for (fd = 0; fd < nfds; ++fd) {
		if (FD_ISSET(fd, fds)) {
			fprintf(fp, " %d", fd);
		}
	}

}

static void diminuto_mux_set_dump(diminuto_mux_t * that, diminuto_mux_set_t * set, FILE * fp)
{
	const char * name;

	name = diminuto_mux_set_name(that, set);
	fprintf(fp, "mux@%p: %s.next=%d\n", that, name, set->next);
	fprintf(fp, "mux@%p: %s.active=<", that, name); diminuto_mux_fds_dump(&set->active, fp); fputs(" >\n", fp);
	fprintf(fp, "mux@%p: %s.ready=<", that, name); diminuto_mux_fds_dump(&set->ready, fp); fputs(" >\n", fp);
}

static void diminuto_mux_dump(diminuto_mux_t * that, FILE * fp)
{
	int signum;

	fprintf(fp, "mux@%p: nfds=%d\n", that, that->nfds);
	diminuto_mux_set_dump(that, &that->read, fp);
	diminuto_mux_set_dump(that, &that->write, fp);
	fprintf(fp, "mux@%p: mask=<", that); diminuto_mux_sigs_dump(&that->mask, fp); fputs(" >\n", fp);
}

static void diminuto_mux_test(diminuto_ticks_t timeout)
{
	diminuto_mux_t mux;
	int pipefd[16][2]; /* 16^2==256 */
	int ss;
	int ii;
	int rc;
	int fd;
	int cc; /* consumers */
	int pp; /* producers */
	uint8_t input[countof(pipefd)];
	uint8_t output[countof(pipefd)];
	uint8_t buffer;
	int reads[1 << (sizeof(uint8_t) * 8)]; /* 1<<(1*8)==256 */
	int writes[1 << (sizeof(uint8_t) * 8)]; /* 1<<(1*8)==256 */
	int * map;

	/*
	 * This is a useful trade off of memory for processing time: allocating
	 * an array big enough to map any file descriptor to something else. You
	 * might reduce the footprint by playing double indirection games, making
	 * each entry one byte, then mapping into a second array that is say 256
	 * bytes or smaller. Note that calloc() automatically zeros the memory that
	 * it allocates. That would be more useful if we were storing pointers, but
	 * alas zero is a valid file descriptor value.
	 */
	ss = sysconf(_SC_OPEN_MAX);
	map = (int *)calloc(ss, sizeof(int));
	ASSERT(map != (int *)0);
	for (ii = 0; ii < ss; ++ii) {
		map[ii] = -1;
	}

	diminuto_mux_init(&mux);
	ASSERT(mux.nfds < 0);
	ASSERT(mux.read.next < 0);
	ASSERT(mux.write.next < 0);

	for (ii = 0; ii < countof(reads); ++ii) {
		reads[ii] = 0;
	}

	for (ii = 0; ii < countof(writes); ++ii) {
		writes[ii] = 0;
	}

	for (ii = 0; ii < countof(pipefd); ++ii) {
		input[ii] = ii * countof(input);
		output[ii] = ii * countof(output);
		ASSERT(pipe(pipefd[ii]) == 0);
		ASSERT(pipefd[ii][0] < ss);
		ASSERT(pipefd[ii][0] >= 0);
		ASSERT(pipefd[ii][1] < ss);
		ASSERT(pipefd[ii][0] != pipefd[ii][1]);
		ASSERT(diminuto_mux_register_read(&mux, pipefd[ii][0]) == 0);
		ASSERT(diminuto_mux_register_write(&mux, pipefd[ii][1]) == 0);
		map[pipefd[ii][0]] = ii;
		map[pipefd[ii][1]] = ii;
	}

	cc = countof(pipefd);
	pp = countof(pipefd);

	while ((cc > 0) || (pp > 0)) {
		rc = diminuto_mux_wait(&mux, timeout);
		if (rc == 0) {
			diminuto_yield();
			continue;
		}
		ASSERT(rc > 0);
		while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {
			ASSERT(fd < ss);
			ii = map[fd];
			ASSERT(ii >= 0);
			ASSERT(ii < countof(pipefd));
			rc = read(fd, &buffer, sizeof(buffer));
			if (rc == 0) {
				if (DEBUG) { fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d read(%d)=%d close\n", cc, pp, fd, ii, fd, rc); }
				ASSERT(diminuto_mux_close(&mux, fd) == 0);
				--cc;
				continue;
			}
			if (DEBUG) { fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d read(%d)=%d %u\n", cc, pp, fd, ii, fd, rc, buffer); }
			ASSERT(rc == sizeof(buffer));
			ASSERT(buffer == input[ii]);
			++reads[buffer];
			++input[ii];
		}
		while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {
			ASSERT(fd < ss);
			ii = map[fd];
			ASSERT(ii >= 0);
			ASSERT(ii < countof(pipefd));
			buffer = output[ii]++;
			rc = write(fd, &buffer, sizeof(buffer));
			if (DEBUG) { fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d write(%d)=%d %u\n", cc, pp, fd, ii, fd, rc, buffer); }
			ASSERT(rc == sizeof(buffer));
			++writes[buffer];
			if (buffer == (((ii + 1) * countof(output)) - 1)) {
				if (DEBUG) { fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d write(%d)=%d close\n", cc, pp, fd, ii, fd, rc); }
				ASSERT(diminuto_mux_close(&mux, fd) == 0);
				--pp;
				continue;
			}
		}
	}

	ASSERT(cc == 0);
	ASSERT(pp == 0);

	for (ii = 0; ii < countof(reads); ++ii) {
		ASSERT(reads[ii] == 1);
	}

	for (ii = 0; ii < countof(writes); ++ii) {
		ASSERT(writes[ii] == 1);
	}

	ASSERT(mux.nfds < 0);
	ASSERT(mux.read.next < 0);
	ASSERT(mux.write.next < 0);

	free(map);
}

int main(int argc, char ** argv)
{
	{
		diminuto_mux_t mux;

		diminuto_mux_init(&mux);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_signal(&mux, SIGHUP) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_signal(&mux, SIGINT) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_read(&mux) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds == STDIN_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
		ASSERT(mux.nfds == STDOUT_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_register_write(&mux, STDERR_FILENO) == 0);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_register_signal(&mux, SIGHUP) == 0);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_register_signal(&mux, SIGINT) == 0);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_read(&mux) < 0);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) == 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_signal(&mux, SIGHUP) == 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_signal(&mux, SIGINT) == 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_read(&mux) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_signal(&mux, SIGHUP) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_unregister_signal(&mux, SIGINT) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_read(&mux) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
	}

	{
		diminuto_mux_t mux;

		diminuto_mux_init(&mux);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
		ASSERT(diminuto_mux_register_write(&mux, STDERR_FILENO) == 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }

		ASSERT(diminuto_mux_wait(&mux, -1) == 2);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next == 0);
		ASSERT(mux.write.next == 0);

		ASSERT(diminuto_mux_ready_read(&mux) < 0);

		ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
		ASSERT(diminuto_mux_ready_write(&mux) == STDERR_FILENO);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) == 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
	}

	{
		diminuto_mux_t mux;

		diminuto_mux_init(&mux);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }

		ASSERT(diminuto_mux_wait(&mux, -1) == 1);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds == STDOUT_FILENO);
		ASSERT(mux.read.next == 0);
		ASSERT(mux.write.next == 0);

		ASSERT(diminuto_mux_ready_read(&mux) < 0);

		ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);

		ASSERT(diminuto_mux_close(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_close(&mux, STDOUT_FILENO) == 0);
		if (DEBUG) { diminuto_mux_dump(&mux, stderr); }
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
	}

	{
		diminuto_mux_test(-1);
		diminuto_mux_test(diminuto_frequency());
		diminuto_mux_test(0);
	}

	{
        pid_t pid;

		ASSERT((pid = fork()) >= 0);

		if (pid) {

			int rendezvous;
	        diminuto_ipv4_t address;
	        diminuto_port_t port;
			diminuto_mux_t mux;
			int producer;
			uint8_t output[1 << (sizeof(uint8_t) * 8)];
			uint8_t input[1 << (sizeof(uint8_t) * 8)];
			uint8_t datum;
			ssize_t sent;
			ssize_t received;
			ssize_t used;
			ssize_t available;
			uint8_t * here;
			uint8_t * there;
			uint8_t * current;
			size_t totalsent;
			size_t totalreceived;
			int status;
			int ready;
			int fd;

			ASSERT((rendezvous = diminuto_ipc_stream_provider(PORT)) >= 0);

			ASSERT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);

			ASSERT(diminuto_alarm_install(0) == 0);
			ASSERT(diminuto_timer_periodic(diminuto_frequency() / 10) == 0);

			diminuto_mux_init(&mux);
			ASSERT(diminuto_mux_register_read(&mux, producer) == 0);
			ASSERT(diminuto_mux_register_write(&mux, producer) == 0);
			ASSERT(diminuto_mux_register_signal(&mux, SIGALRM) == 0);

			here = output;
			used = sizeof(output);
			sent = 0;

			there = input;
			available = sizeof(input);
			received = 0;

			totalsent = 0;
			totalreceived = 0;

			datum = 0;
			do {
				output[datum] = datum;
			} while ((++datum) > 0);

			memset(input, 0, sizeof(input));

			do {

				while (!0) {
					if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
						break;
					} else if (ready == 0) {
						diminuto_yield();
					} else if (errno == EINTR) {
						if (DEBUG) { fprintf(stderr, "producer interrupted\n"); }
					} else {
						FATAL("diminuto_mux_wait");
					}
				}

				while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {

					if (totalsent < TOTAL) {

						if (used > (TOTAL - totalsent)) {
							used = TOTAL - totalsent;
						}

						ASSERT((sent = diminuto_ipc_stream_write(fd, here, 1, used)) > 0);
						ASSERT(sent <= used);

						totalsent += sent;
						if (VERBOSE) { fprintf(stderr, "producer sent %d %u\n", sent, totalsent); }

						here += sent;
						used -= sent;

						if (used == 0) {
							here = output;
							used = sizeof(output);
						}

					}

				}

				current = there;

				if (available > (TOTAL - totalreceived)) {
					available = TOTAL - totalreceived;
				}

				while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

					ASSERT((received = diminuto_ipc_stream_read(fd, there, 1, available)) > 0);
					ASSERT(received <= available);

					totalreceived += received;
					if (VERBOSE) { fprintf(stderr, "producer received %d %u\n", received, totalreceived); }

					there += received;
					available -= received;

					while (current < there) {
						ASSERT(*(current++) == (datum++));
					}

					if (available == 0) {
						there = input;
						available = sizeof(input);
						memset(input, 0, sizeof(input));
					}

				}

			} while (totalreceived < TOTAL);

			ASSERT(diminuto_mux_close(&mux, producer) == 0);
			ASSERT(diminuto_ipc_close(rendezvous) >= 0);

			EXPECT(waitpid(pid, &status, 0) == pid);
			EXPECT(WIFEXITED(status));
			EXPECT(WEXITSTATUS(status) == 0);

		} else {

			diminuto_mux_t mux;
			int consumer;
			uint8_t buffer[64];
			ssize_t sent;
			ssize_t received;
			size_t totalsent;
			size_t totalreceived;
			int ready;
			int fd;
			int done;

			diminuto_delay(diminuto_time_frequency() / 1000, !0);

			ASSERT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

			diminuto_mux_init(&mux);
			ASSERT(diminuto_mux_register_read(&mux, consumer) == 0);

			totalreceived = 0;
			totalsent = 0;
			done = 0;

			do {

				while (!0) {
					if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
						break;
					} else if (ready == 0) {
						diminuto_yield();
					} else if (errno == EINTR) {
						if (DEBUG) { fprintf(stderr, "consumer interrupted\n"); }
					} else {
						FATAL("diminuto_mux_wait");
					}
				}

				while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

					ASSERT((received = diminuto_ipc_stream_read(fd, buffer, 1, sizeof(buffer))) >= 0);
					ASSERT(received <= sizeof(buffer));

					totalreceived += received;
					if (VERBOSE) { fprintf(stderr, "consumer received %d %u\n", received, totalreceived); }

					if (received == 0) {
						done = !0;
						break;
					}

					sent = 0;
					while (sent < received) {
						ASSERT((sent = diminuto_ipc_stream_write(fd,  buffer + sent, 1, received - sent)) > 0);
						ASSERT(sent <= received);

						totalsent += sent;
						if (VERBOSE) { fprintf(stderr, "consumer sent %d %u\n", sent, totalsent); }

						received -= sent;
					}

				}

			} while (!done);

			ASSERT(diminuto_mux_close(&mux, consumer) == 0);

			exit(0);
		}


	}

    EXIT();
}
