/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Framer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Framer feature.
 */

#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "../src/diminuto_framer.h"
#include <ctype.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(FLAG == '~');
        ASSERT(FLAG == '\x7e');
        ASSERT(ESCAPE == '}');
        ASSERT(ESCAPE == '\x7d');
        ASSERT(XON == '\x11');
        ASSERT(XOFF == '\x13');
        ASSERT(RESERVED == (uint8_t)'\xf8');
        ASSERT(MASK == ' ');

        STATUS();
    }

    {
        diminuto_framer_t framer;

        TEST();

        ASSERT(sizeof(diminuto_framer_length_t) == sizeof(uint32_t));
        ASSERT(sizeof(framer.length) == sizeof(uint32_t));
        ASSERT(sizeof(framer.sum) == 2);
        ASSERT(sizeof(framer.check) == 3);

        STATUS();
    }

    {
        diminuto_framer_state_t state = DIMINUTO_FRAMER_STATE_INITIALIZE;

        TEST();

        /*
         * If I botch the definition of the state enumeration, this
         * should get an error or warning at compile time.
         */

        switch (state) {
        case DIMINUTO_FRAMER_STATE_INITIALIZE:          break;
        case DIMINUTO_FRAMER_STATE_FLAG:                break;
        case DIMINUTO_FRAMER_STATE_FLAGS:               break;
        case DIMINUTO_FRAMER_STATE_LENGTH:              break;
        case DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED:      break;
        case DIMINUTO_FRAMER_STATE_FLETCHER:            break;
        case DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED:    break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD:             break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_KERMIT:              break;
        case DIMINUTO_FRAMER_STATE_COMPLETE:            break;
        case DIMINUTO_FRAMER_STATE_FINAL:               break;
        case DIMINUTO_FRAMER_STATE_ABORT:               break;
        case DIMINUTO_FRAMER_STATE_FAILED:              break;
        case DIMINUTO_FRAMER_STATE_OVERFLOW:            break;
        case DIMINUTO_FRAMER_STATE_IDLE:                break;
        }

        STATUS();
    }

    {
        TEST();

        ASSERT(EOF == -1);

        STATUS();
    }

    {
        diminuto_framer_t framer = { (void*)0, };
        diminuto_framer_t * ff = (diminuto_framer_t *)0;
        char buffer[64];

        TEST();

        ff = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        ASSERT(ff == &framer);
        diminuto_framer_dump(ff);
        ASSERT(framer.buffer == buffer);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_INITIALIZE);

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ff = diminuto_framer_reset(&framer);
        ASSERT(ff == &framer);
        diminuto_framer_dump(ff);
        ASSERT(framer.buffer == buffer);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_INITIALIZE);

        ASSERT(framer.length == 0);
        framer.length = sizeof(buffer) / 2;
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);
        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        ASSERT(diminuto_framer_buffer(&framer) == buffer);
        ASSERT(diminuto_framer_length(&framer) == (sizeof(buffer) / 2));

        ff = diminuto_framer_fini(&framer);
        ASSERT(ff == (diminuto_framer_t *)0);
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_IDLE);

        STATUS();
    }

    {
        diminuto_framer_t framer;

        TEST();

        framer.state = DIMINUTO_FRAMER_STATE_INITIALIZE;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLAG;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLAGS;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_LENGTH;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLETCHER;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_PAYLOAD;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_KERMIT;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        ASSERT(diminuto_framer_complete(&framer));
        ASSERT(diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FINAL;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(diminuto_framer_terminal(&framer));
        ASSERT(diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_ABORT;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(diminuto_framer_terminal(&framer));
        ASSERT(diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FAILED;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(diminuto_framer_terminal(&framer));
        ASSERT(diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_OVERFLOW;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(diminuto_framer_terminal(&framer));
        ASSERT(diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));


        STATUS();
    }

    {
        diminuto_framer_t framer;
        diminuto_framer_t * ff;
        diminuto_framer_state_t state;

        ff = diminuto_framer_init(&framer, (void *)0, 0);
        diminuto_framer_dump(ff);

        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, '~');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAGS);
        state = diminuto_framer_machine(ff, '~');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAGS);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);

        ASSERT(ff->length == 0);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);

        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);

        diminuto_framer_dump(ff);
        ff = diminuto_framer_fini(&framer);
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country\xf8.";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t count;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];

        TEST();

        CHECKPOINT("sizeof(DATA)=%zu\n", sizeof(DATA));
        diminuto_dump(stderr, DATA, sizeof(DATA));

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);
        diminuto_framer_debug(that, true);

        do {
            count = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", count);
        } while (count == 0);
        ASSERT(count > 0);

        ASSERT(diminuto_framer_buffer(that) == &buffer);
        ASSERT((count = diminuto_framer_length(that)) > 0);
        CHECKPOINT("diminuto_framer_length=%zd\n", count);
        diminuto_dump(stderr, diminuto_framer_buffer(that), count);
        ASSERT(strcmp(DATA, buffer) == 0);
        diminuto_framer_dump(that);

        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        uint8_t DATA[256];
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t count;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];
        int ii;

        TEST();

        for (ii = 0; ii <= sizeof(DATA); ++ii) {
            DATA[ii] = ii;
        }
        CHECKPOINT("sizeof(DATA)=%zu\n", sizeof(DATA));
        diminuto_dump(stderr, DATA, sizeof(DATA));

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);
        diminuto_framer_debug(that, true);

        do {
            count = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", count);
        } while (count == 0);
        ASSERT(count > 0);

        ASSERT(diminuto_framer_buffer(that) == &buffer);
        ASSERT((count = diminuto_framer_length(that)) > 0);
        CHECKPOINT("diminuto_framer_length=%zd\n", count);
        diminuto_dump(stderr, diminuto_framer_buffer(that), count);
        ASSERT(memcmp(DATA, buffer, sizeof(DATA)) == 0);
        diminuto_framer_dump(that);

        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country\xf8.";
        static const char TOOBIG[sizeof(DATA) + 1] = { '\0', };
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t count;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA)];
        int frames;

        TEST();

        CHECKPOINT("sizeof(DATA)=%zu\n", sizeof(DATA));
        diminuto_dump(stderr, DATA, sizeof(DATA));

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        count = diminuto_framer_abort(sink);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_abort=%zd\n", count);

        /*
         * Multiple FLAG frame starts are filtered out.
         */

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        /*
         * ABORT abandons the current frame.
         */

        count = diminuto_framer_abort(sink);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_abort=%zd\n", count);

        /*
         * Empty frames are discarded.
         */

        count = diminuto_framer_write(sink, (void *)0, 0);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write(empty)=%zd\n", count);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        /*
         * Oversized frames are discarded albeit with log message.
         */

        count = diminuto_framer_write(sink, TOOBIG, sizeof(TOOBIG));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write(toobig)=%zd\n", count);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);

        frames = 0;
        do {
            count = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", count);
            if (count > 0) {
                ASSERT(diminuto_framer_buffer(that) == &buffer);
                ASSERT((count = diminuto_framer_length(that)) > 0);
                CHECKPOINT("diminuto_framer_length=%zd\n", count);
                diminuto_dump(stderr, diminuto_framer_buffer(that), count);
                ASSERT(strcmp(DATA, buffer) == 0);
                diminuto_framer_dump(that);
                ++frames;
                diminuto_framer_reset(that);
            }
        } while (count >= 0);
        ASSERT(count == EOF);
        ASSERT(frames == 2);

        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country\xf8.";
        static const char TOOBIG[sizeof(DATA) + 1] = { '\0', };
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t count;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA)];
        int iterations = 0;

        TEST();

        CHECKPOINT("sizeof(DATA)=%zu\n", sizeof(DATA));
        diminuto_dump(stderr, DATA, sizeof(DATA));

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        count = diminuto_framer_abort(sink);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_abort=%zd\n", count);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        count = diminuto_framer_abort(sink);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_abort=%zd\n", count);

        count = diminuto_framer_write(sink, (void *)0, 0);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write(empty)=%zd\n", count);

        count = diminuto_framer_write(sink, TOOBIG, sizeof(TOOBIG));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write(toobig)=%zd\n", count);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        count = diminuto_framer_write(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);

        /*
         * Consume a single frame.
         */

        do {
            count = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", count);
        } while (count == 0);
        ASSERT(count > 0);
        ASSERT(diminuto_framer_buffer(that) == &buffer);
        ASSERT((count = diminuto_framer_length(that)) > 0);
        CHECKPOINT("diminuto_framer_length=%zd\n", count);
        diminuto_dump(stderr, diminuto_framer_buffer(that), count);
        ASSERT(strcmp(DATA, buffer) == 0);
        diminuto_framer_dump(that);

        /*
         * As a side effect, fini() sets the state machine in the IDLE
         * state, which consumes all further tokens until EOF, which
         * places the FSM into the FINAL state.
         */

        diminuto_framer_fini(that);

        do {
            count = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", count);
            ++iterations;
        } while (count == 0);
        ASSERT(iterations > 1);
        ASSERT(count == EOF);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA1[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country\xf8.";
        static const char DATA2[] = "England {expects} that \x11~every\x13 man will do \xf8his ~duty.";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t count;
        char buffer[256];
        static const char TOOBIG[sizeof(buffer) + 1] = { '\0', };
        int frames;

        TEST();

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        count = diminuto_framer_abort(sink);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_abort=%zd\n", count);

        /*
         * Multiple FLAG frame starts are filtered out.
         */

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        /*
         * ABORT abandons the current frame.
         */

        count = diminuto_framer_abort(sink);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_abort=%zd\n", count);

        /*
         * Empty frames are discarded.
         */

        count = diminuto_framer_write(sink, (void *)0, 0);
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write(empty)=%zd\n", count);

        CHECKPOINT("sizeof(DATA1)=%zu\n", sizeof(DATA1));
        diminuto_dump(stderr, DATA1, sizeof(DATA1));

        count = diminuto_framer_write(sink, DATA1, sizeof(DATA1));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        /*
         * Oversized frames are discarded albeit with log message.
         */

        count = diminuto_framer_write(sink, TOOBIG, sizeof(TOOBIG));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write(toobig)=%zd\n", count);

        CHECKPOINT("sizeof(DATA1)=%zu\n", sizeof(DATA2));
        diminuto_dump(stderr, DATA2, sizeof(DATA2));

        count = diminuto_framer_write(sink, DATA2, sizeof(DATA2));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_write=%zd\n", count);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        frames = 0;
        while (true) {
            count = diminuto_framer_read(source, buffer, sizeof(buffer));
            if (count == EOF) { break; }
            CHECKPOINT("diminuto_framer_read=%zd\n", count);
            diminuto_dump(stderr, buffer, count);
            ++frames;
            if (frames == 1) {
                ASSERT(strcmp(DATA1, buffer) == 0);
            } else if (frames == 2) {
                ASSERT(strcmp(DATA2, buffer) == 0);
            } else {
                /* Do nothing. */
            }
            memset(buffer, 0, sizeof(buffer));
        }
        ASSERT(frames == 2);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    EXIT();
}
