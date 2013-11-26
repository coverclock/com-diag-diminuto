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
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <values.h>

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
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);

		EXPECT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_signal(&mux, SIGHUP) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_signal(&mux, SIGINT) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_read(&mux) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_write(&mux) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		diminuto_mux_dump(&mux, stderr);

		EXPECT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
		EXPECT(mux.nfds == STDIN_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
		EXPECT(mux.nfds == STDOUT_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_register_write(&mux, STDERR_FILENO) == 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_register_signal(&mux, SIGHUP) == 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_register_signal(&mux, SIGINT) == 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_read(&mux) < 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_write(&mux) < 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		diminuto_mux_dump(&mux, stderr);

		EXPECT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
		EXPECT(mux.nfds == STDERR_FILENO);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) == 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_signal(&mux, SIGHUP) == 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_signal(&mux, SIGINT) == 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_read(&mux) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_write(&mux) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		diminuto_mux_dump(&mux, stderr);

		EXPECT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_signal(&mux, SIGHUP) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_unregister_signal(&mux, SIGINT) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_read(&mux) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		EXPECT(diminuto_mux_ready_write(&mux) < 0);
		EXPECT(mux.nfds < 0);
		EXPECT(mux.read.next < 0);
		EXPECT(mux.write.next < 0);
		diminuto_mux_dump(&mux, stderr);
	}

    EXIT();
}
