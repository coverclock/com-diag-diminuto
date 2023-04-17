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
 *
 * The Linux pipe capacity is an astonishing 64KB or 16 virtual
 * memory pages of 4KB each.
 *
 * REFERENCES
 *
 * pipe(7)
 */

#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_typeof.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "../src/diminuto_framer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const int DC1 = '\x11'; /* a.k.a. XON */
static const int DC3 = '\x13'; /* a.k.a. XOFF */

static void sequences(const diminuto_framer_t * that, int line)
{
    size_t missing;
    size_t duplicated;
    bool rolledover;
    bool farend;

    missing = diminuto_framer_missing(that);
    duplicated = diminuto_framer_duplicated(that);
    rolledover = diminuto_framer_rolledover(that);
    farend = diminuto_framer_farend(that);

    ASSERT(diminuto_framer_incoming(that) == that->sequence);
    ASSERT(diminuto_framer_outgoing(that) == that->generated);

    CHECKPOINT("sequences[%d]: sequence=%u previous=%u missing=%zu duplicated=%zu rolledover=%d farend=%d\n", line, that->sequence, that->previous, missing, duplicated, rolledover, farend);
}

int main(int argc, char * argv[])
{
    bool debug = false;
    
    SETLOGMASK();

    if (argc <= 1) {
        /* Do nothing. */
    } else if (strcmp(argv[1], "-d") != 0) {
        /* Do nothing. */
    } else {
        debug = true;
    }

    /*
     * SANITY
     */

    {
        TEST();

        ASSERT(EOF == -1);
        ASSERT(FLAG == '~');
        ASSERT(FLAG == '\x7e');
        ASSERT(ESCAPE == '}');
        ASSERT(ESCAPE == '\x7d');
        ASSERT(XON == '\x11');
        ASSERT(XOFF == '\x13');
        ASSERT(XON == DC1);
        ASSERT(XOFF == DC3);
        ASSERT(NEWLINE == '\n');
        ASSERT(NEWLINE == '\x0a');
        ASSERT(MASK == ' ');

        STATUS();
    }

    {
        diminuto_framer_t framer;

        TEST();

        CHECKPOINT("sizeof(framer)=%zu\n", sizeof(framer));

        ASSERT(sizeof(framer.length) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.length)));
        ASSERT(sizeof(framer.sequence) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.sequence)));
        ASSERT(sizeof(framer.a) == sizeof(uint8_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.a)));
        ASSERT(sizeof(framer.b) == sizeof(uint8_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.b)));
        ASSERT(sizeof(framer.crc) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.crc)));
        ASSERT(diminuto_countof(framer.sum) == 2);
        ASSERT(sizeof(framer.sum[0]) == sizeof(uint8_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.sum[0])));
        ASSERT(diminuto_countof(framer.check) == 3);
        ASSERT(sizeof(framer.check[0]) == sizeof(uint8_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.check[0])));

        STATUS();
    }

    {
        diminuto_framer_state_t state = DIMINUTO_FRAMER_STATE_RESET;

        TEST();

        /*
         * If I botch the definition of the state enumeration, this
         * should get an error or warning at compile time. Similarly
         * for the subsequent tests.
         */

        switch (state) {
        case DIMINUTO_FRAMER_STATE_ABORT:               break;
        case DIMINUTO_FRAMER_STATE_COMPLETE:            break;
        case DIMINUTO_FRAMER_STATE_FAILED:              break;
        case DIMINUTO_FRAMER_STATE_FINAL:               break;
        case DIMINUTO_FRAMER_STATE_FLAG:                break;
        case DIMINUTO_FRAMER_STATE_FLETCHER:            break;
        case DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED:    break;
        case DIMINUTO_FRAMER_STATE_IDLE:                break;
        case DIMINUTO_FRAMER_STATE_INVALID:             break;
        case DIMINUTO_FRAMER_STATE_KERMIT:              break;
        case DIMINUTO_FRAMER_STATE_LENGTH:              break;
        case DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED:      break;
        case DIMINUTO_FRAMER_STATE_NEWLINE:             break;
        case DIMINUTO_FRAMER_STATE_OVERFLOW:            break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD:             break;
        case DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED:     break;
        case DIMINUTO_FRAMER_STATE_RESET:               break;
        case DIMINUTO_FRAMER_STATE_SEQUENCE:            break;
        case DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED:    break;
        }

        STATUS();
    }

    {
        int token = CONSUME;

        TEST();

        switch (token) {
        case ESCAPE:    break;
        case FLAG:      break;
        case NEWLINE:   break;
        case XOFF:      break;
        case XON:       break;
        }

        STATUS();
            
    }

    {
        action_t action = RESET;

        TEST();

        switch (action) {
        case CONSUME:   break;
        case FLETCHER:  break;
        case KERMIT:    break;
        case LENGTH:    break;
        case PAYLOAD:   break;
        case RESET:     break;
        case SEQUENCE:  break;
        case STORE:     break;
        case TERMINATE: break;
        }

        STATUS();
    }

    {
        diminuto_framer_t framer = DIMINUTO_FRAMER_INIT;

        TEST();

        diminuto_framer_dump(&framer);

        ASSERT(framer.buffer == (void *)0);
        ASSERT(framer.here == (uint8_t *)0);
        ASSERT(framer.size == 0);
        ASSERT(framer.limit == 0);
        ASSERT(framer.total == 0);
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_IDLE);
        ASSERT(framer.length == 0);
        ASSERT(framer.candidate == 0);
        ASSERT(framer.sequence == diminuto_maximumof(uint16_t));
        ASSERT(framer.previous == (framer.sequence - 1));
        ASSERT(framer.generated == diminuto_maximumof(uint16_t));
        ASSERT(framer.crc == 0);
        ASSERT(framer.a == 0);
        ASSERT(framer.b == 0);
        ASSERT(framer.sum[0] == 0);
        ASSERT(framer.sum[1] == 0);
        ASSERT(framer.check[0] == ' ');
        ASSERT(framer.check[1] == ' ');
        ASSERT(framer.check[2] == ' ');
        ASSERT(!framer.throttle);
        ASSERT(!framer.debug);

        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);

        ASSERT(diminuto_framer_missing(&framer) == 0);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(!diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

        STATUS();
    }

    {
        diminuto_framer_t framer;
        diminuto_framer_t * ff;
        char buffer[64];

        TEST();

        ff = diminuto_framer_init(&framer, buffer, sizeof(buffer));

        diminuto_framer_dump(&framer);

        ASSERT(ff == &framer);

        ASSERT(framer.buffer == &(buffer[0]));
        ASSERT(framer.here == (uint8_t *)0);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.limit == 0);
        ASSERT(framer.total == 0);
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_RESET);
        ASSERT(framer.length == 0);
        ASSERT(framer.candidate == 0);
        ASSERT(framer.sequence == diminuto_maximumof(uint16_t));
        ASSERT(framer.previous == (framer.sequence - 1));
        ASSERT(framer.generated == diminuto_maximumof(uint16_t));
        ASSERT(framer.crc == 0);
        ASSERT(framer.a == 0);
        ASSERT(framer.b == 0);
        ASSERT(framer.sum[0] == 0);
        ASSERT(framer.sum[1] == 0);
        ASSERT(framer.check[0] == ' ');
        ASSERT(framer.check[1] == ' ');
        ASSERT(framer.check[2] == ' ');
        ASSERT(!framer.throttle);
        ASSERT(!framer.debug);

        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);

        ASSERT(diminuto_framer_missing(&framer) == 0);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(!diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ff = diminuto_framer_reset(&framer);
        ASSERT(ff == &framer);
        diminuto_framer_dump(ff);
        ASSERT(framer.buffer == buffer);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_RESET);

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

        framer.state = DIMINUTO_FRAMER_STATE_RESET;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLAG;
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

        framer.state = DIMINUTO_FRAMER_STATE_SEQUENCE;
        ASSERT(!diminuto_framer_complete(&framer));
        ASSERT(!diminuto_framer_terminal(&framer));
        ASSERT(!diminuto_framer_error(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED;
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

        framer.state = DIMINUTO_FRAMER_STATE_NEWLINE;
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

        framer.state = DIMINUTO_FRAMER_STATE_INVALID;
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

        state = framer.state;
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        ASSERT(!framer.throttle);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, '~');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, '~');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);

        ASSERT(ff->length == 0);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);

        ASSERT(ff->candidate == 0);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);

        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_NEWLINE);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_NEWLINE);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_NEWLINE);
        state = diminuto_framer_machine(ff, '\n');
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!framer.throttle);
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);

        ASSERT(framer.previous == 65535);
        ASSERT(framer.candidate == 0);

        diminuto_framer_dump(ff);
        ff = diminuto_framer_fini(&framer);
    }

    /*
     * LOW-LEVEL API EXAMPLES
     */

    {
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[64];
        char * result;
        char line[sizeof(buffer) * 3];

        TEST();

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        that = diminuto_framer_init(&framer, (void *)0, 0);
        diminuto_framer_dump(that);

        sent = diminuto_framer_writer(sink, that, (void *)0, 0);
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        /*
         * This is a gross misuse of fgets(3).
         */
        result = fgets(buffer, sizeof(buffer), source); 
        ASSERT(result == buffer);

        /*
         * strlen(3) and strchr(3) cannot be used because the buffer
         * may contain '\0's.
         */
        buffer[sizeof(buffer) - 1] = '\n';
        received = 0;
        while (buffer[received++] != '\n') {}

        CHECKPOINT("wire[%zd]:\n", received);
        diminuto_dump(stderr, buffer, received);
        ASSERT(received == sent);
        ASSERT(buffer[0] == '~');
        ASSERT(buffer[received - 1] == '\n');

        ASSERT(!diminuto_framer_terminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_terminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_error(that));
        ASSERT(diminuto_framer_complete(that));
        length = diminuto_framer_length(that);
        CHECKPOINT("line[%zd]:\n", length);
        diminuto_dump(stderr, line, length);
        ASSERT(length == 0);

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        size_t lengthof2;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];
        char line[sizeof(DATA)]; /* Includes space for payload NUL. */
        char * result;

        /*
         * This version transmits the terminating NUL as part of the payload
         * but does not have room for the appended NUL.
         */

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        (void)memset(line, 0xff, sizeof(line));

        that = diminuto_framer_init(&framer, line, sizeof(line));
        diminuto_framer_dump(that);

        sent = diminuto_framer_writer(sink, that, DATA, sizeof(DATA)); /* Sends NUL. */
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > lengthof);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        /*
         * This is a gross misuse of fgets(3).
         */
        result = fgets(buffer, sizeof(buffer), source); 
        ASSERT(result == buffer);

        /*
         * strlen(3) and strchr(3) cannot be used because the buffer
         * may contain '\0's.
         */
        buffer[sizeof(buffer) - 1] = '\n';
        received = 0;
        while (buffer[received++] != '\n') {}

        CHECKPOINT("wire[%zd]:\n", received);
        diminuto_dump(stderr, buffer, received);
        ASSERT(received == sent);
        ASSERT(buffer[0] == '~');
        ASSERT(buffer[received - 1] == '\n');

        ASSERT(!diminuto_framer_terminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_terminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_error(that));
        ASSERT(diminuto_framer_complete(that));
        result = diminuto_framer_buffer(that);
        ASSERT(result == line);
        length = diminuto_framer_length(that);
        CHECKPOINT("line[%zd]:\n", length);
        diminuto_dump(stderr, line, length);
        ASSERT(length == sizeof(DATA));
        ASSERT(memcmp(DATA, line, length) == 0);
        lengthof2 = strlen(line);
        ASSERT(lengthof2 == lengthof);

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        size_t lengthof2;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];
        char line[sizeof(DATA)]; /* Room for appended NUL, terminating NUL not sent. */
        char * result;

        /*
         * This version does not include the payload NUL, but depends on
         * the optional appended NUL.
         */

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        (void)memset(line, 0xff, sizeof(line));

        that = diminuto_framer_init(&framer, line, sizeof(line));
        diminuto_framer_dump(that);

        sent = diminuto_framer_writer(sink, that, DATA, lengthof); /* NUL not transmitted. */
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > lengthof);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        /*
         * This is a gross misuse of fgets(3).
         */
        result = fgets(buffer, sizeof(buffer), source); 
        ASSERT(result == buffer);

        /*
         * strlen(3) and strchr(3) cannot be used because the buffer
         * may contain '\0's.
         */
        buffer[sizeof(buffer) - 1] = '\n';
        received = 0;
        while (buffer[received++] != '\n') {}

        CHECKPOINT("wire[%zd]:\n", received);
        diminuto_dump(stderr, buffer, received);
        ASSERT(received == sent);
        ASSERT(buffer[0] == '~');
        ASSERT(buffer[received - 1] == '\n');

        ASSERT(!diminuto_framer_terminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_terminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_error(that));
        ASSERT(diminuto_framer_complete(that));
        result = diminuto_framer_buffer(that);
        ASSERT(result == line);
        length = diminuto_framer_length(that);
        CHECKPOINT("line[%zd]:\n", length);
        diminuto_dump(stderr, line, length);
        ASSERT(length == lengthof);
        ASSERT(memcmp(DATA, line, length) == 0);
        lengthof2 = strlen(line);
        ASSERT(lengthof2 == lengthof);

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];
        char line[sizeof(DATA) - 1];
        char * result;

        /*
         * This version neither sends the terminating NUL nor has room for
         * the appended NUL.
         */

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        (void)memset(line, 0xff, sizeof(line));

        that = diminuto_framer_init(&framer, line, sizeof(line));
        diminuto_framer_dump(that);

        sent = diminuto_framer_writer(sink, that, DATA, lengthof); /* NUL not transmitted. */
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > lengthof);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        /*
         * This is a gross misuse of fgets(3).
         */
        result = fgets(buffer, sizeof(buffer), source); 
        ASSERT(result == buffer);

        /*
         * strlen(3) and strchr(3) cannot be used because the buffer
         * may contain '\0's.
         */
        buffer[sizeof(buffer) - 1] = '\n';
        received = 0;
        while (buffer[received++] != '\n') {}

        CHECKPOINT("wire[%zd]:\n", received);
        diminuto_dump(stderr, buffer, received);
        ASSERT(received == sent);
        ASSERT(buffer[0] == '~');
        ASSERT(buffer[received - 1] == '\n');

        ASSERT(!diminuto_framer_terminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_terminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_error(that));
        ASSERT(diminuto_framer_complete(that));
        result = diminuto_framer_buffer(that);
        ASSERT(result == line);
        length = diminuto_framer_length(that);
        CHECKPOINT("line[%zd]:\n", length);
        diminuto_dump(stderr, line, length);
        ASSERT(length == lengthof);
        ASSERT(memcmp(DATA, line, length) == 0);

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    /*
     * MID-LEVEL API EXAMPLES
     */

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);
        diminuto_framer_debug(that, debug);

        ASSERT(diminuto_framer_missing(&framer) == 0);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(!diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > lengthof);

        do {
            received = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);

        ASSERT(diminuto_framer_buffer(that) == &buffer);
        length = diminuto_framer_length(that);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, diminuto_framer_buffer(that), length);
        ASSERT(memcmp(DATA, buffer, length) == 0);

        sequences(that, __LINE__);
        ASSERT(diminuto_framer_missing(&framer) == 0);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

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
        ssize_t sent;
        ssize_t received;
        ssize_t length;
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

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);
        diminuto_framer_debug(that, debug);

        sent = diminuto_framer_writer(sink, that, DATA, sizeof(DATA));
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > sizeof(DATA));

        do {
            received = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > sizeof(DATA));
        ASSERT(received == sent);

        ASSERT(diminuto_framer_buffer(that) == &buffer);
        length = diminuto_framer_length(that);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == sizeof(DATA));
        diminuto_dump(stderr, diminuto_framer_buffer(that), length);
        ASSERT(memcmp(DATA, buffer, length) == 0);

        sequences(that, __LINE__);
        ASSERT(diminuto_framer_missing(&framer) == 0);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        static const char TOOBIG[sizeof(DATA) + 1] = { '\0', };
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA)];
        int frames;

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        sent = diminuto_framer_abort(sink);
        CHECKPOINT("diminuto_framer_abort=%zd\n", sent);
        ASSERT(sent > 0);

        /*
         * Multiple FLAG frame starts are filtered out.
         */

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);

        /*
         * ABORT abandons the current frame.
         */

        sent = diminuto_framer_abort(sink);
        CHECKPOINT("diminuto_framer_abort=%zd\n", sent);
        ASSERT(sent > 0);

        /*
         * Empty frames are discarded.
         */

        sent = diminuto_framer_writer(sink, that, (void *)0, 0);
        CHECKPOINT("diminuto_framer_writer(empty)=%zd\n", sent);
        ASSERT(sent > 0);

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        /*
         * Oversized frames are discarded albeit with log message.
         */

        sent = diminuto_framer_writer(sink, that, TOOBIG, sizeof(TOOBIG));
        CHECKPOINT("diminuto_framer_writer(toobig)=%zd\n", sent);
        ASSERT(sent > sizeof(TOOBIG));

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        frames = 0;
        do {
            received = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
            if (received > 0) {
                ASSERT(received > lengthof);
                ASSERT(received == sent);
                ASSERT(diminuto_framer_buffer(that) == &buffer);
                length = diminuto_framer_length(that);
                CHECKPOINT("diminuto_framer_length=%zd\n", length);
                ASSERT(length == lengthof);
                diminuto_dump(stderr, diminuto_framer_buffer(that), length);
                ASSERT(memcmp(DATA, buffer, length) == 0);
                ++frames;
                sequences(that, __LINE__);
                ASSERT(diminuto_framer_missing(&framer) == ((frames == 2) ? 1 : 0));
                ASSERT(diminuto_framer_duplicated(&framer) == 0);
                ASSERT(!diminuto_framer_rolledover(&framer));
                ASSERT(!diminuto_framer_farend(&framer));
                ASSERT(!diminuto_framer_nearend(&framer));
                diminuto_framer_reset(that);
            }
        } while (received >= 0);
        ASSERT(received == EOF);
        ASSERT(frames == 2);

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        static const char TOOBIG[sizeof(DATA) + 1] = { '\0', };
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA)];
        int iterations = 0;

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        sent = diminuto_framer_abort(sink);
        CHECKPOINT("diminuto_framer_abort=%zd\n", sent);
        ASSERT(sent > 0);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        rc = fputc('~', sink);
        ASSERT(rc != EOF);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
        diminuto_framer_dump(that);
        ASSERT(diminuto_framer_buffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_length(&framer) == -1);

        sent = diminuto_framer_abort(sink);
        CHECKPOINT("diminuto_framer_abort=%zd\n", sent);
        ASSERT(sent > 0);

        sent = diminuto_framer_writer(sink, that, (void *)0, 0);
        CHECKPOINT("diminuto_framer_writer(empty)=%zd\n", sent);
        ASSERT(sent > 0);

        sent = diminuto_framer_writer(sink, that, TOOBIG, sizeof(TOOBIG));
        CHECKPOINT("diminuto_framer_writer(toobig)=%zd\n", sent);
        ASSERT(sent > sizeof(TOOBIG));

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        /*
         * Consume a single frame.
         */

        do {
            received = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        ASSERT(diminuto_framer_buffer(that) == &buffer);
        length = diminuto_framer_length(that);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, diminuto_framer_buffer(that), length);
        ASSERT(memcmp(DATA, buffer, length) == 0);

        sequences(that, __LINE__);
        ASSERT(diminuto_framer_missing(&framer) == 1);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(!diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

        /*
         * As a side effect, fini() sets the state machine in the IDLE
         * state, which consumes all further tokens until EOF, which
         * places the FSM into the FINAL state.
         */

        diminuto_framer_dump(that);
        diminuto_framer_fini(that);
        diminuto_framer_dump(that);

        do {
            received = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
            ++iterations;
        } while (received == 0);
        ASSERT(iterations > 1);
        ASSERT(received == EOF);

        sequences(that, __LINE__);
        ASSERT(diminuto_framer_missing(&framer) == 1);
        ASSERT(diminuto_framer_duplicated(&framer) == 0);
        ASSERT(!diminuto_framer_rolledover(&framer));
        ASSERT(!diminuto_framer_farend(&framer));
        ASSERT(!diminuto_framer_nearend(&framer));

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        ssize_t length;
        size_t lengthof;
        diminuto_framer_t writer;
        diminuto_framer_t reader;
        char buffer[sizeof(DATA)];

        TEST();

        lengthof = strlen(DATA);
        CHECKPOINT("sizeof(DATA)=%zu strlen(DATA)=%zu\n", sizeof(DATA), lengthof);
        diminuto_dump(stderr, DATA, lengthof);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        diminuto_framer_init(&writer, (void *)0, 0);
        diminuto_framer_init(&reader, buffer, sizeof(buffer));

        sent = diminuto_framer_writer(sink, &writer, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        sent = diminuto_framer_writer(sink, &writer, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        /*
         * Restart the far-end.
         */

        diminuto_framer_init(&writer, (void *)0, 0);

        sent = diminuto_framer_writer(sink, &writer, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        sent = diminuto_framer_writer(sink, &writer, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer(data)=%zd\n", sent);
        ASSERT(sent > lengthof);

        diminuto_framer_fini(&writer);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        /*
         * Consume the first frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_length(&reader);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        sequences(&reader, __LINE__);
        ASSERT(diminuto_framer_missing(&reader) == 0);
        ASSERT(diminuto_framer_duplicated(&reader) == 0);
        ASSERT(diminuto_framer_rolledover(&reader));
        ASSERT(!diminuto_framer_farend(&reader));
        ASSERT(!diminuto_framer_nearend(&reader));

        /*
         * Consume the second frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_length(&reader);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        sequences(&reader, __LINE__);
        ASSERT(diminuto_framer_missing(&reader) == 0);
        ASSERT(diminuto_framer_duplicated(&reader) == 0);
        ASSERT(!diminuto_framer_rolledover(&reader));
        ASSERT(!diminuto_framer_farend(&reader));
        ASSERT(!diminuto_framer_nearend(&reader));

        /*
         * Consume the third frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_length(&reader);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == lengthof);
        ASSERT(received == sent);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        sequences(&reader, __LINE__);
        ASSERT(diminuto_framer_missing(&reader) == 0);
        ASSERT(diminuto_framer_duplicated(&reader) == 0);
        ASSERT(!diminuto_framer_rolledover(&reader));
        ASSERT(diminuto_framer_farend(&reader));
        ASSERT(!diminuto_framer_nearend(&reader));

        /*
         * Consume the fourth frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_length(&reader);
        CHECKPOINT("diminuto_framer_length=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        sequences(&reader, __LINE__);
        ASSERT(diminuto_framer_missing(&reader) == 0);
        ASSERT(diminuto_framer_duplicated(&reader) == 0);
        ASSERT(!diminuto_framer_rolledover(&reader));
        ASSERT(!diminuto_framer_farend(&reader));
        ASSERT(!diminuto_framer_nearend(&reader));

        rc = fclose(source);    
        ASSERT(rc == 0);

        diminuto_framer_fini(&reader);

        STATUS();
    }

    {
        uint8_t DATA[2];
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        char buffer[sizeof(DATA) * 3];
        unsigned int ii;
        unsigned int jj;
        int iterations;
        diminuto_framer_t writer;
        diminuto_framer_t reader;

        TEST();

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        diminuto_framer_init(&writer, (void *)0, 0);
        diminuto_framer_init(&reader, buffer, sizeof(buffer));

        iterations = 0;

        for (ii = 0x00; ii <= 0xff; ++ii) {
            for (jj = 0x00; jj <= 0xff; ++jj) {

                DATA[0] = ii;
                DATA[1] = jj;

                sent = diminuto_framer_writer(sink, &writer, DATA, sizeof(DATA));
                COMMENT("diminuto_framer_writer=%zd\n", sent);
                ASSERT(sent > sizeof(DATA));

                do {
                    received = diminuto_framer_reader(source, &reader);
                } while (received == 0);
                COMMENT("diminuto_framer_reader=%zd\n", received);
                ASSERT(received > sizeof(DATA));

                ASSERT(memcmp(DATA, buffer, reader.length) == 0);

                diminuto_framer_reset(&reader);

                ++iterations;

            }
        }

        CHECKPOINT("iterations=%d\n", iterations);
        ASSERT(iterations == (256 * 256));

        CHECKPOINT("outoing=%u sequence=%u previous=%u\n", writer.generated, reader.sequence, reader.previous);

        /*
         * One more iteration and all of the sequence counters would have
         * rolled over.
         */

        --iterations;
        ASSERT(writer.generated == iterations);
        ASSERT(reader.sequence == iterations);
        ASSERT(reader.previous == (iterations - 1));

        diminuto_framer_fini(&writer);
        diminuto_framer_fini(&reader);

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    /*
     * HIGH-LEVEL API EXAMPLES
     */

    {
        static const char DATA1[] = "Now is the {time} for all ~good men to come to the aid of \x11their\x13 country.\n";
        static const char DATA2[] = "England {expects} that \x11~every\x13 man will do his ~duty.\r";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
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

        sent = diminuto_framer_abort(sink);
        CHECKPOINT("diminuto_framer_abort=%zd\n", sent);
        ASSERT(sent > 0);

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

        sent = diminuto_framer_abort(sink);
        CHECKPOINT("diminuto_framer_abort=%zd\n", sent);
        ASSERT(sent > 0);

        /*
         * Empty frames are discarded.
         */

        sent = diminuto_framer_write(sink, (void *)0, 0);
        CHECKPOINT("diminuto_framer_write(empty)=%zd\n", sent);
        ASSERT(sent == 0);

        CHECKPOINT("sizeof(DATA1)=%zu\n", sizeof(DATA1));
        diminuto_dump(stderr, DATA1, sizeof(DATA1));

        sent = diminuto_framer_write(sink, DATA1, sizeof(DATA1));
        CHECKPOINT("diminuto_framer_write=%zd\n", sent);
        ASSERT(sent == sizeof(DATA1));

        /*
         * Oversized frames are discarded albeit with log message.
         */

        sent = diminuto_framer_write(sink, TOOBIG, sizeof(TOOBIG));
        CHECKPOINT("diminuto_framer_write(toobig)=%zd\n", sent);
        ASSERT(sent == sizeof(TOOBIG));

        CHECKPOINT("sizeof(DATA1)=%zu\n", sizeof(DATA2));
        diminuto_dump(stderr, DATA2, sizeof(DATA2));

        sent = diminuto_framer_write(sink, DATA2, sizeof(DATA2));
        CHECKPOINT("diminuto_framer_write=%zd\n", sent);
        ASSERT(sent == sizeof(DATA2));

        rc = fclose(sink);    
        ASSERT(rc == 0);

        frames = 0;
        while (true) {
            received = diminuto_framer_read(source, buffer, sizeof(buffer));
            if (received == EOF) { break; }
            CHECKPOINT("diminuto_framer_read=%zd\n", received);
            diminuto_dump(stderr, buffer, received);
            ++frames;
            if (frames == 1) {
                ASSERT(received == sizeof(DATA1));
                ASSERT(memcmp(DATA1, buffer, received) == 0);
            } else if (frames == 2) {
                ASSERT(received == sizeof(DATA2));
                ASSERT(memcmp(DATA2, buffer, received) == 0);
            } else {
                ASSERT((frames != 1) && (frames != 2));
            }
            memset(buffer, 0, sizeof(buffer));
        }
        ASSERT(frames == 2);

        rc = fclose(source);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        uint8_t DATA[2];
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t sent;
        ssize_t received;
        char buffer[sizeof(DATA) * 3];
        unsigned int ii;
        unsigned int jj;
        int iterations;

        TEST();

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        iterations = 0;

        for (ii = 0x00; ii <= 0xff; ++ii) {
            for (jj = 0x00; jj <= 0xff; ++jj) {

                DATA[0] = ii;
                DATA[1] = jj;

                COMMENT("iteration=%d DATA=0x%2.2x%2.2x\n", iterations, DATA[0], DATA[1]);

                sent = diminuto_framer_write(sink, DATA, sizeof(DATA));
                COMMENT("diminuto_framer_write=%zd\n", sent);
                ASSERT(sent == sizeof(DATA));

                received = diminuto_framer_read(source, buffer, sizeof(buffer));
                COMMENT("diminuto_framer_read=%zd\n", received);
                ASSERT(received == sent);

                ASSERT(memcmp(DATA, buffer, received) == 0);

                ++iterations;

            }
        }

        CHECKPOINT("iterations=%d\n", iterations);
        ASSERT(iterations == (256 * 256));

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    EXIT();
}
