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
#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    size_t pagesize;

    SETLOGMASK();

    {
        diminuto_memory_pagesize_method_t method = DIMINUTO_MEMORY_PAGESIZE_METHOD_UNKNOWN;

        TEST();

        pagesize = diminuto_memory_pagesize(&method);
        ASSERT(pagesize > 0);
        ASSERT(method != DIMINUTO_MEMORY_PAGESIZE_METHOD_UNKNOWN);
        CHECKPOINT("VMPAGE size=%zu\n", pagesize);
        CHECKPOINT("VMPAGE method=%c\n", method);

        STATUS();
    }

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
        ADVISE(size == pagesize);

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

    {
        int rc;
        int input;
        int output;
        FILE * source;
        FILE * sink;
        ssize_t ready;
        ssize_t empty;
        ssize_t size;

        TEST();

        input = open("/dev/zero", O_RDONLY);
        ASSERT(input >= 0);

        output = open("/dev/null", O_WRONLY);
        ASSERT(output >= 0);

        source = fdopen(input, "r");
        ASSERT(source != (FILE *)0);

        size = diminuto_file_readsize(source);
        CHECKPOINT("SOURCE readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(source);
        CHECKPOINT("SOURCE writesize=%zd\n", size);
        EXPECT(size == 0);
    
        sink = fdopen(output, "w");
        ASSERT(sink != (FILE *)0);

        size = diminuto_file_readsize(source);
        CHECKPOINT("SINK readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(source);
        CHECKPOINT("SINK writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("INITIAL ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("INITIAL empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fgetc(source);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(source);
        CHECKPOINT("GET readsize=%zd\n", size);
        EXPECT(size == pagesize);

        size = diminuto_file_writesize(source);
        CHECKPOINT("GET writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET ready=%zd\n", ready);
        EXPECT(ready == (pagesize - 1));

        rc = ungetc(rc, source);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(source);
        CHECKPOINT("UNGET readsize=%zd\n", size);
        EXPECT(size == pagesize);

        size = diminuto_file_writesize(source);
        CHECKPOINT("UNGET writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("UNGET ready=%zd\n", ready);
        EXPECT(ready == pagesize);

        rc = fputc(rc, sink);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(sink);
        CHECKPOINT("PUT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(sink);
        CHECKPOINT("PUT writesize=%zd\n", size);
        EXPECT(size == pagesize);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT empty=%zd\n", empty);
        EXPECT(empty == (pagesize - 1));

        rc = fflush(sink);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(sink);
        CHECKPOINT("FLUSH readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(sink);
        CHECKPOINT("FLUSH writesize=%zd\n", size);
        EXPECT(size == pagesize);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("FLUSH empty=%zd\n", empty);
        EXPECT(empty == pagesize);

        rc = fclose(sink);
        ASSERT(rc == 0);

        rc = fclose(source);
        ASSERT(rc == 0);

        STATUS();
    }

    EXIT();
}
