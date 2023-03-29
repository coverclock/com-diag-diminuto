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
#include <ctype.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    SETLOGMASK();

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
        ASSERT(framer.buffer == buffer);
        ASSERT(framer.size == sizeof(buffer));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_INITIALIZE);

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ff = diminuto_framer_reinit(&framer);
        ASSERT(ff == &framer);
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
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_IDLE);

        STATUS();
    }

    {
        diminuto_framer_t framer;
        diminuto_framer_t * ff;
        diminuto_framer_state_t state;

        ff = diminuto_framer_init(&framer, (void *)0, 0);

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

        ff = diminuto_framer_fini(&framer);
    }

    {
        static const char DATA[] = "Now is the {time} for all ~good men to come to the aid of \x1their\x13 country\xf8.";
        int fd[2];
        FILE * source;
        FILE * sink;
        int rc;
        ssize_t count;
        diminuto_framer_t framer;
        diminuto_framer_t * that;
        char buffer[sizeof(DATA) * 3];

        TEST();

        diminuto_dump(stderr, DATA, sizeof(DATA));

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        count = diminuto_framer_writer(sink, DATA, sizeof(DATA));
        ASSERT(count > 0);
        CHECKPOINT("diminuto_framer_writer=%zd\n", count);

        that = diminuto_framer_init(&framer, buffer, sizeof(buffer));
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

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    EXIT();
}
