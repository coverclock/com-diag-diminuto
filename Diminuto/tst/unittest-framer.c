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
#include "com/diag/diminuto/diminuto_file.h"
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

        ASSERT(sizeof(framer.length0) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.length0)));
        ASSERT(sizeof(framer.length) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.length)));
        ASSERT(sizeof(framer.sequence0) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.sequence0)));
        ASSERT(sizeof(framer.sequence) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.sequence)));
        ASSERT(sizeof(framer.a) == sizeof(uint8_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.a)));
        ASSERT(sizeof(framer.b) == sizeof(uint8_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.b)));
        ASSERT(sizeof(framer.crc) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.crc)));
        ASSERT(sizeof(framer.crc0) == sizeof(uint16_t));
        ASSERT(!diminuto_issigned(diminuto_typeof(framer.crc0)));
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
        diminuto_framer_t framer = DIMINUTO_FRAMER_INITIALIZER;

        TEST();

        diminuto_framer_dump(&framer);

        ASSERT(framer.buffer == (void *)0);
        ASSERT(framer.here == (uint8_t *)0);
        ASSERT(framer.size == 0);
        ASSERT(framer.limit == 0);
        ASSERT(framer.total == 0);
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_IDLE);
        ASSERT(framer.length0 == 0);
        ASSERT(framer.length == 0);
        ASSERT(framer.sequence0 == 0);
        ASSERT(framer.sequence == diminuto_maximumof(uint16_t));
        ASSERT(framer.previous == (framer.sequence - 1));
        ASSERT(framer.generated == diminuto_maximumof(uint16_t));
        ASSERT(framer.crc == 0);
        ASSERT(framer.crc0 == 0);
        ASSERT(framer.a == 0);
        ASSERT(framer.b == 0);
        ASSERT(framer.sum[0] == 0);
        ASSERT(framer.sum[1] == 0);
        ASSERT(framer.check[0] == ' ');
        ASSERT(framer.check[1] == ' ');
        ASSERT(framer.check[2] == ' ');
        ASSERT(!framer.throttle);
        ASSERT(!framer.debug);

        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);

        ASSERT(diminuto_framer_getmissing(&framer) == 0);
        ASSERT(diminuto_framer_getduplicated(&framer) == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

        STATUS();
    }

    {
        diminuto_framer_t framer;
        diminuto_framer_t * ff;
        char buffer1[64];
        char buffer2[64];

        TEST();

        ff = diminuto_framer_init(&framer, buffer1, sizeof(buffer1));

        diminuto_framer_dump(&framer);

        ASSERT(ff == &framer);

        ASSERT(framer.buffer == &(buffer1[0]));
        ASSERT(framer.here == (uint8_t *)0);
        ASSERT(framer.size == sizeof(buffer1));
        ASSERT(framer.limit == 0);
        ASSERT(framer.total == 0);
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_RESET);
        ASSERT(framer.length0 == 0);
        ASSERT(framer.length == 0);
        ASSERT(framer.sequence0 == 0);
        ASSERT(framer.sequence == diminuto_maximumof(uint16_t));
        ASSERT(framer.previous == (framer.sequence - 1));
        ASSERT(framer.generated == diminuto_maximumof(uint16_t));
        ASSERT(framer.crc == 0);
        ASSERT(framer.crc0 == 0);
        ASSERT(framer.a == 0);
        ASSERT(framer.b == 0);
        ASSERT(framer.sum[0] == 0);
        ASSERT(framer.sum[1] == 0);
        ASSERT(framer.check[0] == ' ');
        ASSERT(framer.check[1] == ' ');
        ASSERT(framer.check[2] == ' ');
        ASSERT(!framer.throttle);
        ASSERT(!framer.debug);

        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);

        ASSERT(diminuto_framer_getmissing(&framer) == 0);
        ASSERT(diminuto_framer_getduplicated(&framer) == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ff = diminuto_framer_reset(&framer);
        ASSERT(ff == &framer);
        diminuto_framer_dump(ff);
        ASSERT(framer.buffer == &(buffer1[0]));
        ASSERT(framer.size == sizeof(buffer1));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_RESET);

        ASSERT(framer.length0 == 0);
        ASSERT(framer.length == 0);
        framer.length = sizeof(buffer1) / 2;
        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);
        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        ASSERT(diminuto_framer_getbuffer(&framer) == &(buffer1[0]));
        ASSERT(diminuto_framer_getlength(&framer) == (sizeof(buffer1) / 2));

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ff = diminuto_framer_set(&framer, buffer2, sizeof(buffer2));
        ASSERT(ff == &framer);
        diminuto_framer_dump(ff);
        ASSERT(framer.buffer == &(buffer2[0]));
        ASSERT(framer.size == sizeof(buffer2));
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_RESET);

        ff = diminuto_framer_fini(&framer);
        ASSERT(ff == (diminuto_framer_t *)0);
        ASSERT(framer.state == DIMINUTO_FRAMER_STATE_IDLE);

        STATUS();
    }

    {
        static const size_t WINDOW = (size_t)diminuto_maximumof(uint16_t) + 1;
        diminuto_framer_t framer = DIMINUTO_FRAMER_INITIALIZER;
        size_t miss;
        size_t duplicate;

        TEST();

        CHECKPOINT("window=%zu\n", WINDOW);
        ASSERT(WINDOW == 65536);
        ASSERT(WINDOW == DIMINUTO_FRAMER_WINDOW);

        framer.previous = WINDOW - 3;
        framer.sequence = WINDOW - 2;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 0);
        ASSERT(duplicate == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));

        framer.previous += 1;
        framer.sequence += 1;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 0);
        ASSERT(duplicate == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));

        framer.previous += 1;
        framer.sequence += 1;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 0);
        ASSERT(duplicate == 0);
        ASSERT(diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));

        framer.previous += 1;
        framer.sequence += 1;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 0);
        ASSERT(duplicate == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));

        framer.previous += 1;
        framer.sequence += 1;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 0);
        ASSERT(duplicate == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));

        framer.previous += 1;
        framer.sequence = 0;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));
        ASSERT(diminuto_framer_didfarend(&framer));

        framer.previous = WINDOW - 1;
        framer.sequence += 1;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(diminuto_framer_didnearend(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));

        framer.previous = 2;
        framer.sequence = 4;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 1);
        ASSERT(duplicate == (WINDOW - miss));

        framer.previous = 2;
        framer.sequence = 4;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(miss == 1);
        ASSERT(duplicate == (WINDOW - miss));

        framer.previous = 4;
        framer.sequence = 2;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(duplicate == 3);
        ASSERT(miss == (WINDOW - duplicate));

        framer.previous = 0;
        framer.sequence = WINDOW - 1;
        miss = diminuto_framer_getmissing(&framer);
        duplicate = diminuto_framer_getduplicated(&framer);
        CHECKPOINT("previous=%u sequence=%u missing=%zu duplicated=%zu\n", framer.previous, framer.sequence, miss, duplicate);
        ASSERT(duplicate == 2);
        ASSERT(miss == (WINDOW - duplicate));

        STATUS();
    }

    {
        diminuto_framer_t framer;

        TEST();

        framer.state = DIMINUTO_FRAMER_STATE_RESET;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLAG;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_LENGTH;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_SEQUENCE;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLETCHER;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_PAYLOAD;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_PAYLOAD_ESCAPED;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_KERMIT;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_NEWLINE;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(!diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        ASSERT(diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FINAL;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_ABORT;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_FAILED;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_OVERFLOW;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_INVALID;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(diminuto_framer_iserror(&framer));

        framer.state = DIMINUTO_FRAMER_STATE_IDLE;
        ASSERT(!diminuto_framer_iscomplete(&framer));
        ASSERT(diminuto_framer_isterminal(&framer));
        ASSERT(!diminuto_framer_iserror(&framer));


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
        ASSERT(!diminuto_framer_isthrottled(&framer));
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_RESET);
        state = diminuto_framer_machine(ff, '~');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);
        state = diminuto_framer_machine(ff, '~');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLAG);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_LENGTH_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);

        ASSERT(ff->length0 == 0);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_SEQUENCE_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);

        ASSERT(ff->sequence0 == 0);

        state = diminuto_framer_machine(ff, '\0');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER);
        state = diminuto_framer_machine(ff, '}');
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_FLETCHER_ESCAPED);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);

        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_KERMIT);
        state = diminuto_framer_machine(ff, ' ');
        ASSERT(state == DIMINUTO_FRAMER_STATE_NEWLINE);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_NEWLINE);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_NEWLINE);
        state = diminuto_framer_machine(ff, '\n');
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);
        state = diminuto_framer_machine(ff, DC3);
        ASSERT(diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);
        state = diminuto_framer_machine(ff, DC1);
        ASSERT(!diminuto_framer_isthrottled(&framer));
        ASSERT(state == DIMINUTO_FRAMER_STATE_COMPLETE);

        ASSERT(framer.sequence0 == 0);
        ASSERT(framer.sequence == 0);
        ASSERT(framer.previous == 65535);
        ASSERT(framer.length0 == 0);
        ASSERT(framer.length == 0);

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

        ASSERT(!diminuto_framer_isterminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_isterminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_iserror(that));
        ASSERT(diminuto_framer_iscomplete(that));
        length = diminuto_framer_getlength(that);
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

        ASSERT(!diminuto_framer_isterminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_isterminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_iserror(that));
        ASSERT(diminuto_framer_iscomplete(that));
        result = diminuto_framer_getbuffer(that);
        ASSERT(result == line);
        length = diminuto_framer_getlength(that);
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

        ASSERT(!diminuto_framer_isterminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_isterminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_iserror(that));
        ASSERT(diminuto_framer_iscomplete(that));
        result = diminuto_framer_getbuffer(that);
        ASSERT(result == line);
        length = diminuto_framer_getlength(that);
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

        ASSERT(!diminuto_framer_isterminal(that));
        for (result = &(buffer[0]); result < &(buffer[received]); ++result) {
            /*
             * Don't confuse the character '\xff' with EOF due to sign
             * extension.
             */
            diminuto_framer_machine(that, (*result) & 0xff);
            if (diminuto_framer_isterminal(that)) { break; }
        }
        ASSERT(!diminuto_framer_iserror(that));
        ASSERT(diminuto_framer_iscomplete(that));
        result = diminuto_framer_getbuffer(that);
        ASSERT(result == line);
        length = diminuto_framer_getlength(that);
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
        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);
        diminuto_framer_debug(that, debug);

        ASSERT(diminuto_framer_getmissing(&framer) == 0);
        ASSERT(diminuto_framer_getduplicated(&framer) == 0);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

        sent = diminuto_framer_writer(sink, that, DATA, lengthof);
        CHECKPOINT("diminuto_framer_writer=%zd\n", sent);
        ASSERT(sent > lengthof);

        do {
            received = diminuto_framer_reader(source, that);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);

        ASSERT(diminuto_framer_getbuffer(that) == &buffer);
        length = diminuto_framer_getlength(that);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, diminuto_framer_getbuffer(that), length);
        ASSERT(memcmp(DATA, buffer, length) == 0);

        ASSERT(diminuto_framer_getmissing(&framer) == 0);
        ASSERT(diminuto_framer_getduplicated(&framer) == 0);
        ASSERT(diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

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
        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);
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

        ASSERT(diminuto_framer_getbuffer(that) == &buffer);
        length = diminuto_framer_getlength(that);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == sizeof(DATA));
        diminuto_dump(stderr, diminuto_framer_getbuffer(that), length);
        ASSERT(memcmp(DATA, buffer, length) == 0);

        ASSERT(diminuto_framer_getmissing(&framer) == 0);
        ASSERT(diminuto_framer_getduplicated(&framer) == 0);
        ASSERT(diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

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
        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);

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
                ASSERT(diminuto_framer_getbuffer(that) == &buffer);
                length = diminuto_framer_getlength(that);
                CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
                ASSERT(length == lengthof);
                diminuto_dump(stderr, diminuto_framer_getbuffer(that), length);
                ASSERT(memcmp(DATA, buffer, length) == 0);
                ++frames;
                ASSERT(diminuto_framer_getmissing(&framer) == ((frames == 2) ? 1 : 0));
                ASSERT(!diminuto_framer_didrollover(&framer));
                ASSERT(!diminuto_framer_didfarend(&framer));
                ASSERT(!diminuto_framer_didnearend(&framer));
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
        unsigned int iterations = 0;

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
        ASSERT(diminuto_framer_getbuffer(&framer) == (void *)0);
        ASSERT(diminuto_framer_getlength(&framer) == EOF);

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
        ASSERT(diminuto_framer_getbuffer(that) == &buffer);
        length = diminuto_framer_getlength(that);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, diminuto_framer_getbuffer(that), length);
        ASSERT(memcmp(DATA, buffer, length) == 0);

        ASSERT(diminuto_framer_getmissing(&framer) == 1);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

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

        ASSERT(diminuto_framer_getmissing(&framer) == 1);
        ASSERT(!diminuto_framer_didrollover(&framer));
        ASSERT(!diminuto_framer_didfarend(&framer));
        ASSERT(!diminuto_framer_didnearend(&framer));

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
        length = diminuto_framer_getlength(&reader);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        ASSERT(diminuto_framer_didrollover(&reader));
        ASSERT(!diminuto_framer_didfarend(&reader));
        ASSERT(!diminuto_framer_didnearend(&reader));

        /*
         * Consume the second frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_getlength(&reader);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        ASSERT(!diminuto_framer_didrollover(&reader));
        ASSERT(!diminuto_framer_didfarend(&reader));
        ASSERT(!diminuto_framer_didnearend(&reader));

        /*
         * Consume the third frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_getlength(&reader);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == lengthof);
        ASSERT(received == sent);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        ASSERT(!diminuto_framer_didrollover(&reader));
        ASSERT(diminuto_framer_didfarend(&reader));
        ASSERT(!diminuto_framer_didnearend(&reader));

        /*
         * Consume the fourth frame.
         */

        do {
            received = diminuto_framer_reader(source, &reader);
            CHECKPOINT("diminuto_framer_reader=%zd\n", received);
        } while (received == 0);
        ASSERT(received > lengthof);
        ASSERT(received == sent);
        length = diminuto_framer_getlength(&reader);
        CHECKPOINT("diminuto_framer_getlength=%zd\n", length);
        ASSERT(length == lengthof);
        diminuto_dump(stderr, buffer, length);
        ASSERT(memcmp(DATA, buffer, length) == 0);
        diminuto_framer_reset(&reader);

        ASSERT(!diminuto_framer_didrollover(&reader));
        ASSERT(!diminuto_framer_didfarend(&reader));
        ASSERT(!diminuto_framer_didnearend(&reader));

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
        size_t length;
        char buffer[sizeof(DATA) * 3];
        unsigned int ii;
        unsigned int jj;
        unsigned int iterations;
        diminuto_framer_t writer;
        diminuto_framer_t reader;
        uint16_t sequence;
        uint16_t expected;

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
        expected = 0;

        for (ii = 0x00; ii <= 0xff; ++ii) {
            for (jj = 0x00; jj <= 0xff; ++jj) {

                DATA[0] = ii;
                DATA[1] = jj;

                sent = diminuto_framer_writer(sink, &writer, DATA, sizeof(DATA));
                sequence = diminuto_framer_getoutgoing(&writer);
                COMMENT("diminuto_framer_writer=%zd #%u\n", sent, sequence);
                ASSERT(sent > sizeof(DATA));
                ASSERT(sequence == expected);

                do {
                    received = diminuto_framer_reader(source, &reader);
                } while (received == 0);
                length = diminuto_framer_getlength(&reader);
                sequence = diminuto_framer_getincoming(&reader);
                COMMENT("diminuto_framer_reader=%zd [%zu] #%u\n", received, length, sequence);
                ASSERT(received > sizeof(DATA));
                ASSERT(length == sizeof(DATA));
                ASSERT(sequence == expected);

                ASSERT(memcmp(DATA, buffer, length) == 0);

                diminuto_framer_reset(&reader);

                ++iterations;
                ++expected;

            }
        }

        CHECKPOINT("iterations=%d\n", iterations);
        ASSERT(iterations == (256 * 256));

        CHECKPOINT("outoing=%u sequence=%u previous=%u\n", writer.generated, reader.sequence, reader.previous);

        /*
         * One more iteration and all of the sequence counters would have
         * rolled over.
         */

        ASSERT(writer.generated == (iterations - 1));
        ASSERT(reader.sequence == (iterations - 1));
        ASSERT(reader.previous == (iterations - 2));

        /*
         * Do it all again, to prove we can.
         */

        for (ii = 0x00; ii <= 0xff; ++ii) {
            for (jj = 0x00; jj <= 0xff; ++jj) {

                DATA[0] = ii;
                DATA[1] = jj;

                sent = diminuto_framer_writer(sink, &writer, DATA, sizeof(DATA));
                sequence = diminuto_framer_getoutgoing(&writer);
                COMMENT("diminuto_framer_writer=%zd #%u\n", sent, sequence);
                ASSERT(sent > sizeof(DATA));
                ASSERT(sequence == expected);

                do {
                    received = diminuto_framer_reader(source, &reader);
                } while (received == 0);
                length = diminuto_framer_getlength(&reader);
                sequence = diminuto_framer_getincoming(&reader);
                COMMENT("diminuto_framer_reader=%zd [%zu] #%u\n", received, length, sequence);
                ASSERT(received > sizeof(DATA));
                ASSERT(length == sizeof(DATA));
                ASSERT(sequence == expected);

                ASSERT(memcmp(DATA, buffer, length) == 0);

                diminuto_framer_reset(&reader);

                ++iterations;
                ++expected;

            }
        }

        CHECKPOINT("iterations=%d\n", iterations);
        ASSERT(iterations == (2 * (256 * 256)));

        CHECKPOINT("outoing=%u sequence=%u previous=%u\n", writer.generated, reader.sequence, reader.previous);

        ASSERT(writer.generated == ((iterations / 2) - 1));
        ASSERT(reader.sequence == ((iterations / 2) - 1));
        ASSERT(reader.previous == ((iterations / 2) - 2));

        diminuto_framer_fini(&writer);
        diminuto_framer_fini(&reader);

        rc = fclose(source);    
        ASSERT(rc == 0);

        rc = fclose(sink);    
        ASSERT(rc == 0);

        STATUS();
    }

    {
        diminuto_framer_t framer = DIMINUTO_FRAMER_INITIALIZER;
        diminuto_log_mask_t mask;
        int fd[2];
        int rc;
        FILE * source;
        FILE * sink;
        ssize_t result;

        TEST();

        mask = diminuto_log_mask;
        diminuto_log_mask = 0xff;
        NOTIFY("diminuto_log_mask was 0x%2.2x now 0x%2.2x\n", mask, diminuto_log_mask);

        rc = pipe(fd);
        ASSERT(rc == 0);

        source = fdopen(fd[0], "r");
        ASSERT(source != (FILE *)0);

        sink = fdopen(fd[1], "w");
        ASSERT(source != (FILE *)0);

        rc = fputc('\xaa', sink);
        ASSERT(rc != EOF);
        rc = fflush(sink);
        ASSERT(rc != EOF);

        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        framer.sequence = 4;
        framer.total = 2;
        framer.length = 1;
        result = diminuto_framer_reader(source, &framer);
        ASSERT(result == 3);

        rc = fputc('\xbb', sink);
        ASSERT(rc != EOF);
        rc = fflush(sink);
        ASSERT(rc != EOF);

        framer.state = DIMINUTO_FRAMER_STATE_COMPLETE;
        framer.length = 0;
        result = diminuto_framer_reader(source, &framer);
        ASSERT(result == 0);

        rc = fputc('\xcc', sink);
        ASSERT(rc != EOF);
        rc = fflush(sink);
        ASSERT(rc != EOF);

        framer.state = DIMINUTO_FRAMER_STATE_ABORT;
        result = diminuto_framer_reader(source, &framer);
        ASSERT(result == 0);

        rc = fputc('\xdd', sink);
        ASSERT(rc != EOF);
        rc = fflush(sink);
        ASSERT(rc != EOF);

        framer.state = DIMINUTO_FRAMER_STATE_FAILED;
        framer.sum[0] = '\x11';
        framer.sum[1] = '\x22';
        framer.a = 0x33;
        framer.b = 0x44;
        framer.crc0 = 0x5566;
        framer.crc = 0x7788;
        result = diminuto_framer_reader(source, &framer);
        ASSERT(result == 0);

        rc = fputc('\xee', sink);
        ASSERT(rc != EOF);
        rc = fflush(sink);
        ASSERT(rc != EOF);

        framer.state = DIMINUTO_FRAMER_STATE_OVERFLOW;
        framer.length0 = 2;
        framer.size = 1;

        rc = fputc('\xff', sink);
        ASSERT(rc != EOF);
        rc = fflush(sink);
        ASSERT(rc != EOF);

        framer.state = DIMINUTO_FRAMER_STATE_INVALID;
        result = diminuto_framer_reader(source, &framer);
        ASSERT(result == 0);

        rc = fclose(sink);
        ASSERT(rc == 0);

        framer.state = DIMINUTO_FRAMER_STATE_FINAL;
        result = diminuto_framer_reader(source, &framer);
        ASSERT(result == EOF);

        rc = fclose(source);
        ASSERT(rc == 0);

        diminuto_log_mask = mask;
        
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
        unsigned int iterations;

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
