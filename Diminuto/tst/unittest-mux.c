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

int main(int argc, char ** argv)
{
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

		ASSERT(diminuto_mux_wait(&mux, 0) == 2);
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

		ASSERT(diminuto_mux_wait(&mux, 0) == 1);
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
		diminuto_mux_t mux;
		int pipefd[8][2];
		int ii;
		int rc;
		int fd;
		int nn;
		uint8_t input[countof(pipefd)];
		uint8_t output[countof(pipefd)];
		uint8_t buffer;

		diminuto_mux_init(&mux);
		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

		for (ii = 0; ii < countof(pipefd); ++ii) {
			input[ii] = output[ii] = ii * countof(output);
			ASSERT(pipe(pipefd[ii]) == 0);
			ASSERT(pipefd[ii][0] >= 0);
			ASSERT(pipefd[ii][1] >= 0);
			ASSERT(pipefd[ii][0] != pipefd[ii][1]);
			ASSERT(diminuto_mux_register_read(&mux, pipefd[ii][0]) == 0);
			ASSERT(diminuto_mux_register_write(&mux, pipefd[ii][1]) == 0);
		}

		nn = countof(pipefd);

		while (nn > 0) {
			rc = diminuto_mux_wait(&mux, diminuto_frequency());
			if (rc == 0) {
				continue;
			}
			ASSERT(rc > 0);
			while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {
				for (ii = 0; ii < countof(pipefd); ++ii) {
					if (fd == pipefd[ii][0]) {
						break;
					}
				}
				ASSERT(ii < countof(pipefd));
				rc = read(pipefd[ii][0], &buffer, sizeof(buffer));
				if (rc == 0) {
					fprintf(stderr, "nn=%d fd=%d ii=%d read(%d)=%d close\n", nn, fd, ii, pipefd[ii][0], rc);
					ASSERT(diminuto_mux_close(&mux, pipefd[ii][0]) == 0);
					--nn;
					continue;
				}
				fprintf(stderr, "nn=%d fd=%d ii=%d read(%d)=%d 0x%2.2x\n", nn, fd, ii, pipefd[ii][0], rc, buffer);
				ASSERT(rc == sizeof(buffer));
				ASSERT(buffer == (input[ii]++));
			}
			while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {
				for (ii = 0; ii < countof(pipefd); ++ii) {
					if (fd == pipefd[ii][1]) {
						break;
					}
				}
				ASSERT(ii < countof(pipefd));
				rc = write(pipefd[ii][1], &output[ii], sizeof(output[ii]));
				fprintf(stderr, "nn=%d fd=%d ii=%d write(%d)=%d 0x%2.2x\n", nn, fd, ii, pipefd[ii][1], rc, output[ii]);
				ASSERT(rc == sizeof(output[ii]));
				++output[ii];
				if (output[ii] == 0) {
					fprintf(stderr, "nn=%d fd=%d ii=%d write(%d)=%d close\n", nn, fd, ii, pipefd[ii][1], rc);
					ASSERT(diminuto_mux_close(&mux, pipefd[ii][1]) == 0);
				}
			}
		}

		ASSERT(mux.nfds < 0);
		ASSERT(mux.read.next < 0);
		ASSERT(mux.write.next < 0);

	}

    EXIT();
}
