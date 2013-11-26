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
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

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

	nfds = getdtablesize();
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

	ss = getdtablesize();
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
			break;
		}
		ASSERT(rc > 0);
		while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {
			ASSERT(fd < ss);
			ii = map[fd];
			ASSERT(ii >= 0);
			ASSERT(ii < countof(pipefd));
			rc = read(fd, &buffer, sizeof(buffer));
			if (rc == 0) {
				fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d read(%d)=%d close\n", cc, pp, fd, ii, fd, rc);
				ASSERT(diminuto_mux_close(&mux, fd) == 0);
				--cc;
				continue;
			}
			fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d read(%d)=%d %u\n", cc, pp, fd, ii, fd, rc, buffer);
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
			fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d write(%d)=%d %u\n", cc, pp, fd, ii, fd, rc, buffer);
			ASSERT(rc == sizeof(buffer));
			++writes[buffer];
			if (buffer == (((ii + 1) * countof(output)) - 1)) {
				fprintf(stderr, "cc=%d pp=%d fd=%d ii=%d write(%d)=%d close\n", cc, pp, fd, ii, fd, rc);
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
		ASSERT(diminuto_mux_indefinite() == 0x7fffffffffffffffLL);
	}

	{
		diminuto_mux_t mux;

		diminuto_mux_init(&mux);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
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
		diminuto_mux_dump(&mux, stderr);

		ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
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
		diminuto_mux_dump(&mux, stderr);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
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
		diminuto_mux_dump(&mux, stderr);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
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
		diminuto_mux_dump(&mux, stderr);
	}

	{
		diminuto_mux_t mux;

		diminuto_mux_init(&mux);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
		ASSERT(diminuto_mux_register_write(&mux, STDERR_FILENO) == 0);

		ASSERT(diminuto_mux_wait(&mux, diminuto_mux_indefinite()) == 2);
		ASSERT(mux.nfds == STDERR_FILENO);
		ASSERT(mux.read.next == 0);
		ASSERT(mux.write.next == 0);
		diminuto_mux_dump(&mux, stderr);

		ASSERT(diminuto_mux_ready_read(&mux) < 0);

		ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
		ASSERT(diminuto_mux_ready_write(&mux) == STDERR_FILENO);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);

		ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
		ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) == 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
	}

	{
		diminuto_mux_t mux;

		diminuto_mux_init(&mux);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);

		ASSERT(diminuto_mux_wait(&mux, diminuto_mux_indefinite()) == 1);
		ASSERT(mux.nfds == STDOUT_FILENO);
		ASSERT(mux.read.next == 0);
		ASSERT(mux.write.next == 0);
		diminuto_mux_dump(&mux, stderr);

		ASSERT(diminuto_mux_ready_read(&mux) < 0);

		ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
		ASSERT(diminuto_mux_ready_write(&mux) < 0);

		ASSERT(diminuto_mux_close(&mux, STDIN_FILENO) == 0);
		ASSERT(diminuto_mux_close(&mux, STDOUT_FILENO) == 0);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);
	}

	{
		diminuto_mux_test(diminuto_mux_indefinite());
		diminuto_mux_test(diminuto_frequency());
		diminuto_mux_test(0);
	}

    EXIT();
}
