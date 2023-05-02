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
 *
 * The GLibC standard I/O buffer capacity is 4KB or 1 virtual
 * memory page of 4KB. This is probably documented somewhere, but I
 * haven't found it.
 *
 * The Linux pipe capacity is an astonishing 64KB or 16 virtual
 * memory pages of 4KB each. This is documented in pipe(7).
 */

#include "com/diag/diminuto/diminuto_file.h"
#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    size_t buffersize;

    SETLOGMASK();

    {
        diminuto_memory_pagesize_method_t method = DIMINUTO_MEMORY_PAGESIZE_METHOD_UNKNOWN;
        size_t pagesize;

        TEST();

        pagesize = diminuto_memory_pagesize(&method);
        CHECKPOINT("VMPAGE size=%zu\n", pagesize);
        ASSERT(pagesize > 0);
        ADVISE(pagesize == 4096);
        CHECKPOINT("VMPAGE method=%c\n", method);
        ASSERT(method != DIMINUTO_MEMORY_PAGESIZE_METHOD_UNKNOWN);

        /*
         * BUFSIZ is the default standard I/O buffer size.
         * The virtual page size is used instead.
         */

        CHECKPOINT("STDIO size=%d\n", BUFSIZ);
        ADVISE(BUFSIZ == pagesize);

        buffersize = pagesize;
        CHECKPOINT("BUFFER size=%zu\n", buffersize);

        STATUS();
    }

    /*
     * Test small amounts of data through a FIFO.
     */

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
        CHECKPOINT("INITIAL source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("INITIAL sink empty=%zd\n", empty);
        ADVISE(empty == 0); /* Apparently allocated on first use. */

        rc = fputc('\x12', sink);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("PUT1 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT1 sink empty=%zd\n", empty);
        EXPECT(empty > 0);

        size = empty + 1;
        CHECKPOINT("BUFFER size=%zd\n", size);
        ADVISE(size == buffersize);

        rc = fputc('\x34', sink);
        ASSERT(rc == '\x34');

        ready = diminuto_file_ready(source);
        CHECKPOINT("PUT2 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT2 sink empty=%zd\n", empty);
        EXPECT(empty == (size - 2));

        rc = fflush(sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("FLUSH source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("FLUSH sink empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fgetc(source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET1 source ready=%zd\n", ready);
        EXPECT(ready == 1);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET1 sink empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = ungetc(rc, source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("UNGET1 source ready=%zd\n", ready);
        EXPECT(ready == 2);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("UNGET1 sink empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fgetc(source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET2 source ready=%zd\n", ready);
        EXPECT(ready == 1);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET2 sink empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fgetc(source);
        ASSERT(rc == '\x34');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET3 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET3 sink empty=%zd\n", empty);
        EXPECT(empty == size);

        rc = fclose(sink);
        ASSERT(rc == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("CLOSE source ready=%zd\n", ready);
        ADVISE(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("CLOSE source empty=%zd\n", empty);
        ADVISE(empty == 0); /* Apparently typical. */

        rc = fgetc(source);
        ASSERT(rc == EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("EOF source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("EOF source empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fclose(source);
        ASSERT(rc == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("CLOSE source ready=%zd\n", ready);
        ADVISE(ready < 0); /* Apparently unpredictable and non-deterministic. */

        empty = diminuto_file_empty(sink);
        CHECKPOINT("CLOSE sink empty=%zd\n", empty);
        ADVISE(empty == 0);

        STATUS();
    }

    /*
     * Same test as above but with buffering disabled.
     */

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

        size = diminuto_file_readsize(source);
        CHECKPOINT("BEFORE source readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(source);
        CHECKPOINT("BEFORE source writesize=%zd\n", size);
        EXPECT(size == 0);

        rc = setvbuf(source, (char *)0, _IONBF, 0);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(source);
        CHECKPOINT("AFTER source readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(source);
        CHECKPOINT("AFTER source writesize=%zd\n", size);
        EXPECT(size == 0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        size = diminuto_file_readsize(sink);
        CHECKPOINT("BEFORE sink readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(sink);
        CHECKPOINT("BEFORE sink writesize=%zd\n", size);
        EXPECT(size == 0);

        rc = setvbuf(sink, (char *)0, _IONBF, 0);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(sink);
        CHECKPOINT("AFTER sink readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(sink);
        CHECKPOINT("AFTER sink writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("INITIAL source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("INITIAL sink empty=%zd\n", empty);
        ADVISE(empty == 0); /* Apparently allocated on first use. */

        rc = fputc('\x12', sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("PUT1 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT1 sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fputc('\x34', sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("PUT2 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("PUT2 sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fflush(sink);
        ASSERT(rc != EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("FLUSH source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("FLUSH sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fgetc(source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET1 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET1 sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        /*
         * The push back buffer used by the unget is seperate.
         */

        rc = ungetc(rc, source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("UNGET1 source ready=%zd\n", ready);
        EXPECT(ready == 1);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("UNGET1 sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fgetc(source);
        ASSERT(rc == '\x12');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET2 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET2 sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fgetc(source);
        ASSERT(rc == '\x34');

        ready = diminuto_file_ready(source);
        CHECKPOINT("GET3 source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("GET3 sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fclose(sink);
        ASSERT(rc == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("CLOSE source ready=%zd\n", ready);
        ADVISE(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("CLOSE sink empty=%zd\n", empty);
        ADVISE(empty == 0); /* Apparently typical. */

        rc = fgetc(source);
        ASSERT(rc == EOF);

        ready = diminuto_file_ready(source);
        CHECKPOINT("EOF source ready=%zd\n", ready);
        EXPECT(ready == 0);

        empty = diminuto_file_empty(sink);
        CHECKPOINT("EOF sink empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fclose(source);
        ASSERT(rc == 0);

        ready = diminuto_file_ready(source);
        CHECKPOINT("CLOSE source ready=%zd\n", ready);
        ADVISE(ready < 0); /* Apparently unpredictable and non-deterministic. */

        empty = diminuto_file_empty(sink);
        CHECKPOINT("CLOSE sink empty=%zd\n", empty);
        ADVISE(empty == 0);

        STATUS();
    }

    /*
     * Test filling up the read buffer.
     */

    {
        int rc;
        int fd;
        FILE * stream;
        ssize_t ready;
        ssize_t size;
        int ii;

        TEST();

        fd = open("/dev/zero", O_RDONLY);
        ASSERT(fd >= 0);

        stream = fdopen(fd, "r");
        ASSERT(stream != (FILE *)0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("INITIAL readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("INITIAL writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(stream);
        CHECKPOINT("INITIAL ready=%zd\n", ready);
        EXPECT(ready == 0);

        rc = fgetc(stream);
        ASSERT(rc == 0);

        /*
         * First get allocates read buffer *and* does a read to fill it up.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("GET readsize=%zd\n", size);
        EXPECT(size == buffersize);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("GET writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(stream);
        CHECKPOINT("GET ready=%zd\n", ready);
        EXPECT(ready == (buffersize - 1));

        size = ready + 1;
        CHECKPOINT("BUFFER size=%zd\n", size);
        EXPECT(size == buffersize);

        rc = ungetc(rc, stream);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("UNGET readsize=%zd\n", size);
        EXPECT(size == buffersize);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("UNGET writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(stream);
        CHECKPOINT("UNGET ready=%zd\n", ready);
        EXPECT(ready == buffersize);

        for (ii = 0; ii < ready; ++ii) {
            rc = fgetc(stream);
            ASSERT(rc == 0);
        }

        /*
         * Get of all ready octets does not automatically cause another read.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("GETS readsize=%zd\n", size);
        EXPECT(size == buffersize);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("GETS writesize=%zd\n", size);
        EXPECT(size == 0);

        ready = diminuto_file_ready(stream);
        CHECKPOINT("GETS ready=%zd\n", ready);
        EXPECT(ready == 0);

        rc = fclose(stream);
        ASSERT(rc == 0);

        STATUS();
    }

    /*
     * Test filling up the write buffer.
     */

    {
        int rc;
        int fd;
        FILE * stream;
        ssize_t empty;
        ssize_t size;
        ssize_t ii;
        ssize_t limit;

        TEST();

        fd = open("/dev/null", O_WRONLY);
        ASSERT(fd >= 0);
    
        stream = fdopen(fd, "w");
        ASSERT(stream != (FILE *)0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("INITIAL readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("INITIAL writesize=%zd\n", size);
        EXPECT(size == 0);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("INITIAL empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fputc('\0', stream);
        ASSERT(rc == 0);

        /*
         * First put allocates write buffer.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUT writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("PUT empty=%zd\n", empty);
        EXPECT(empty == (buffersize - 1));

        size = empty + 1;
        CHECKPOINT("BUFFER size=%zd\n", size);
        EXPECT(size == buffersize);

        limit = empty;
        for (ii = 0; ii < limit; ++ii) {
            rc = fputc('\0', stream);
            ASSERT(rc == 0);
            empty = diminuto_file_empty(stream);
            EXPECT(empty == (buffersize - ii - 2));
        }

        /*
         * Write buffer is full but has not yet automatically flushed.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUTS readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUTS writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("PUTS empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fflush(stream);
        ASSERT(rc == 0);

        /*
         * Write buffer is empty after manual flush.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("FLUSH readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("FLUSH writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("FLUSH empty=%zd\n", empty);
        EXPECT(empty == buffersize);

        rc = fclose(stream);
        ASSERT(rc == 0);

        STATUS();
    }

    /*
     * Same test as above except uses autoamtic flush.
     */

    {
        int rc;
        int fd;
        FILE * stream;
        ssize_t empty;
        ssize_t size;
        ssize_t ii;
        ssize_t limit;

        TEST();

        fd = open("/dev/null", O_WRONLY);
        ASSERT(fd >= 0);
    
        stream = fdopen(fd, "w");
        ASSERT(stream != (FILE *)0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("INITIAL readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("INITIAL writesize=%zd\n", size);
        EXPECT(size == 0);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("INITIAL empty=%zd\n", empty);
        EXPECT(empty == 0);

        rc = fputc('\0', stream);
        ASSERT(rc == 0);

        /*
         * First put allocates write buffer.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUT writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("PUT empty=%zd\n", empty);
        EXPECT(empty == (buffersize - 1));

        size = empty + 1;
        CHECKPOINT("BUFFER size=%zd\n", size);
        EXPECT(size == buffersize);

        limit = empty;
        for (ii = 0; ii < limit; ++ii) {
            rc = fputc('\0', stream);
            ASSERT(rc == 0);
            empty = diminuto_file_empty(stream);
            EXPECT(empty == (buffersize - ii - 2));
        }

        /*
         * Write buffer is full but has not yet automatically flushed.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUTS readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUTS writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("PUTS empty=%zd\n", empty);
        EXPECT(empty == 0);

        /*
         * One more put forces automatic flush and leaves one octet.
         */

        rc = fputc('\0', stream);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("AUTO readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("AUTO writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("AUTO empty=%zd\n", empty);
        EXPECT(empty == (buffersize - 1));

        rc = fflush(stream);
        ASSERT(rc == 0);

        /*
         * Write buffer is empty after manual flush.
         */

        size = diminuto_file_readsize(stream);
        CHECKPOINT("FLUSH readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("FLUSH writesize=%zd\n", size);
        EXPECT(size == buffersize);

        empty = diminuto_file_empty(stream);
        CHECKPOINT("FLUSH empty=%zd\n", empty);
        EXPECT(empty == buffersize);

        rc = fclose(stream);
        ASSERT(rc == 0);

        STATUS();
    }

    /*
     * Set buffer explicitly for an input stream.
     */

    {
        FILE * stream;
        ssize_t size;
        char * buffer;
        size_t total;
        int rc;

        TEST();

        total = buffersize + BUFSIZ;
        ASSERT(total != buffersize);
        ASSERT(total != BUFSIZ);
        CHECKPOINT("TOTAL size=%zu\n", total);

        buffer = malloc(total);
        ASSERT(buffer != (char *)0);

        stream = fopen("/dev/zero", "r");
        ASSERT(stream != (FILE *)0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("DEFAULT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("DEFAULT writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("DEFAULT ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("DEFAULT empty=%zd\n", size);
        EXPECT(size == 0);

        rc = setvbuf(stream, buffer, _IOFBF, total);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("SETVBUF readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("SETVBUF writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("SETVBUF ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("SETVBUF empty=%zd\n", size);
        EXPECT(size == 0);

        rc = fgetc(stream);
        ASSERT(rc == '\0');

        size = diminuto_file_readsize(stream);
        CHECKPOINT("GET readsize=%zd\n", size);
        EXPECT(size == total);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("GET writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("GET ready=%zd\n", size);
        EXPECT(size == (total - 1));

        size = diminuto_file_empty(stream);
        CHECKPOINT("GET empty=%zd\n", size);
        EXPECT(size == 0);

        rc = fclose(stream);
        ASSERT(rc == 0);

        free(buffer);

        STATUS();
    }

    /*
     * Set buffer explicitly for an output stream.
     */

    {
        FILE * stream;
        ssize_t size;
        char * buffer;
        size_t total;
        int rc;

        TEST();

        total = buffersize + BUFSIZ;
        ASSERT(total != buffersize);
        ASSERT(total != BUFSIZ);
        CHECKPOINT("TOTAL size=%zu\n", total);

        buffer = malloc(total);
        ASSERT(buffer != (char *)0);

        stream = fopen("/dev/null", "a");
        ASSERT(stream != (FILE *)0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("DEFAULT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("DEFAULT writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("DEFAULT ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("DEFAULT empty=%zd\n", size);
        EXPECT(size == 0);

        rc = setvbuf(stream, buffer, _IOFBF, total);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("SETVBUF readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("SETVBUF writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("SETVBUF ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("SETVBUF empty=%zd\n", size);
        EXPECT(size == 0);

        rc = fputc('\0', stream);
        ASSERT(rc == '\0');

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUT writesize=%zd\n", size);
        EXPECT(size == total);

        size = diminuto_file_ready(stream);
        CHECKPOINT("PUT ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("PUT empty=%zd\n", size);
        EXPECT(size == (total - 1));

        rc = fclose(stream);
        ASSERT(rc == 0);

        free(buffer);

        STATUS();
    }


    /*
     * Set buffer explicitly for a read/write stream. Some of the behavior
     * here was unexpected, and (as far as I can tell so far) undocumented.
     * But perhaps I should have expected it. We are only providing a single
     * buffer, and the buffer is shared for both the input and the output
     * side of the same stream. If you look at the glibc source code, you can
     * oretty quickly find the code where the buffer management switches
     * between output mode to input mode, or vice versa.
     */

    {
        FILE * stream;
        ssize_t size;
        char * buffer;
        size_t total;
        int rc;

        TEST();

        total = buffersize + BUFSIZ;
        ASSERT(total != buffersize);
        ASSERT(total != BUFSIZ);
        CHECKPOINT("TOTAL size=%zu\n", total);

        buffer = malloc(total);
        ASSERT(buffer != (char *)0);

        stream = fopen("/dev/zero", "a+");
        ASSERT(stream != (FILE *)0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("DEFAULT readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("DEFAULT writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("DEFAULT ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("DEFAULT empty=%zd\n", size);
        EXPECT(size == 0);

        rc = setvbuf(stream, buffer, _IOFBF, total);
        ASSERT(rc == 0);

        size = diminuto_file_readsize(stream);
        CHECKPOINT("SETVBUF readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("SETVBUF writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("SETVBUF ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("SETVBUF empty=%zd\n", size);
        EXPECT(size == 0);

        rc = fputc('\0', stream);
        ASSERT(rc == '\0');

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUT1 readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUT1 writesize=%zd\n", size);
        EXPECT(size == total);

        size = diminuto_file_ready(stream);
        CHECKPOINT("PUT1 ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("PUT1 empty=%zd\n", size);
        EXPECT(size == (total - 1));

        /*
         * The fgetc() presumably forces a flush on the output side.
         */

        rc = fgetc(stream);
        ASSERT(rc == '\0');

        size = diminuto_file_readsize(stream);
        CHECKPOINT("GET1 readsize=%zd\n", size);
        EXPECT(size == total);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("GET1 writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("GET1 ready=%zd\n", size);
        EXPECT(size == (total - 1));

        size = diminuto_file_empty(stream);
        CHECKPOINT("GET1 empty=%zd\n", size);
        EXPECT(size == 0);

        /*
         * What happens to the buffered input when we do the fputc()?
         * It's apparently discarded, perhaps under the assumption
         * that since we're modifying the file, it may no longer
         * be correct.
         */

        rc = fputc('\0', stream);
        ASSERT(rc == '\0');

        size = diminuto_file_readsize(stream);
        CHECKPOINT("PUT2 readsize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("PUT2 writesize=%zd\n", size);
        EXPECT(size == (total - 1));

        size = diminuto_file_ready(stream);
        CHECKPOINT("PUT2 ready=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_empty(stream);
        CHECKPOINT("PUT2 empty=%zd\n", size);
        EXPECT(size == (total - 2));

        rc = fgetc(stream);
        ASSERT(rc == '\0');

        size = diminuto_file_readsize(stream);
        CHECKPOINT("GET2 readsize=%zd\n", size);
        EXPECT(size == total);

        size = diminuto_file_writesize(stream);
        CHECKPOINT("GET2 writesize=%zd\n", size);
        EXPECT(size == 0);

        size = diminuto_file_ready(stream);
        CHECKPOINT("GET2 ready=%zd\n", size);
        EXPECT(size == (total - 1));

        size = diminuto_file_empty(stream);
        CHECKPOINT("GET2 empty=%zd\n", size);
        EXPECT(size == 0);

        rc = fclose(stream);
        ASSERT(rc == 0);

        free(buffer);

        STATUS();
    }

    /*
     * Predefined FILEs: stdin, stdout, and stderr.
     * stdin and stdout have yet to be used, so will not have allocated
     * their buffers. stderr is not buffered.
     */

    {
        TEST();

        CHECKPOINT("STDIN readsize=%zd\n",  diminuto_file_readsize(stdin));
        CHECKPOINT("STDIN writesize=%zd\n", diminuto_file_writesize(stdin));
        CHECKPOINT("STDIN ready=%zd\n",     diminuto_file_ready(stdin));
        CHECKPOINT("STDIN empty=%zd\n",     diminuto_file_empty(stdin));

        CHECKPOINT("STDOUT readsize=%zd\n",  diminuto_file_readsize(stdout));
        CHECKPOINT("STDOUT writesize=%zd\n", diminuto_file_writesize(stdout));
        CHECKPOINT("STDOUT ready=%zd\n",     diminuto_file_ready(stdout));
        CHECKPOINT("STDOUT empty=%zd\n",     diminuto_file_empty(stdout));

        CHECKPOINT("STDERR readsize=%zd\n",  diminuto_file_readsize(stderr));
        CHECKPOINT("STDERR writesize=%zd\n", diminuto_file_writesize(stderr));
        CHECKPOINT("STDERR ready=%zd\n",     diminuto_file_ready(stderr));
        CHECKPOINT("STDERR empty=%zd\n",     diminuto_file_empty(stderr));

        STATUS();
    }

    EXIT();
}
