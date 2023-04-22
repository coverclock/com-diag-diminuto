/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the File feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the File feature.
 */

#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t ready;
        ssize_t empty;
        ssize_t size;

        TEST();

        rc = pipe(fd);
        ASSERT(rc == 0);
        CHECKPOINT("PIPE fd[0]=%d\n", fd[0]);
        CHECKPOINT("PIPE fd[1]=%d\n", fd[1]);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("INITIAL ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("INITIAL empty=%zd\n", empty);
        ADVISE(empty == 0); /* Apparently allocated on first use. */

        rc = fputc('\x12', sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("PUT1 ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT1 empty=%zd\n", empty);
        EXPECT(empty > 0);

        size = empty + 1;
        CHECKPOINT("BUFFER size=%zd\n", size);

        rc = fputc('\x34', sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("PUT2 ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT2 empty=%zd\n", empty);
        EXPECT(empty == (size - 2));

        rc = fflush(sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("FLUSH ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("FLUSH empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fgetc(source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET1 ready=%zd\n", ready);
        EXPECT(ready == 1);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET1 empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = ungetc(rc, source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("UNGET1 ready=%zd\n", ready);
        EXPECT(ready == 2);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("UNGET1 empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fgetc(source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET2 ready=%zd\n", ready);
        EXPECT(ready == 1);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET2 empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fgetc(source);
        ASSERT(rc == '\x34');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET3 ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET3 empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fclose(sink);
        ASSERT(rc == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("CLOSESINK ready=%zd\n", ready);
        ADVISE(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("CLOSESINK empty=%zd\n", empty);
        ADVISE(empty == 0); /* Apparently typical. */

        rc = fgetc(source);
        ASSERT(rc == EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("EOF ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("EOF empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fclose(source);
        ASSERT(rc == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("CLOSESOURCE ready=%zd\n", ready);
        ADVISE(ready < 0); /* Apparently unpredictable and non-deterministic. */

        empty = diminuto_file_empty(sink);
        CHECKPOINT("CLOSESOURCE empty=%zd\n", empty);
        ADVISE(empty == 0);

        STATUS();
    }

    EXIT();
}

