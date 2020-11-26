/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2012-2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Hamming feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Hamming feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_hamming.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/diminuto_hamming.h"

static const uint8_t SYM[] = {
    0x00, 0xd2, 0x55, 0x87,
    0x99, 0x4b, 0xcc, 0x1e,
    0xe1, 0x33, 0xb4, 0x66,
    0x78, 0xaa, 0x2d, 0xff,
};

/*
 * Edna St. Vincent Milay died the same year Richard Hamming of Bell Telephone
 * Laboratories published his paper in the Bell System Technical Journal.
 */
static const char TXT[] =
    "SONNET NINETY-NINE\n"
    "Love is not all: It is not meat nor drink\n"
    "Nor slumber nor roof against the rain,\n"
    "Nor yet a floating spar to men that sink\n"
    "and rise and sink and rise and sink again.\n"
    "Love cannot fill the thickened lung with breath\n"
    "Nor cleanse the blood, nor set the fractured bone;\n"
    "Yet many a man is making friends with death\n"
    "even as I speak, for lack of love alone.\n"
    "It may well be that in a difficult hour,\n"
    "pinned down by pain and moaning for release\n"
    "or nagged by want past resolutions power,\n"
    "I might be driven to sell your love for peace,\n"
    "Or trade the memory of this night for food.\n"
    "It may well be. I do not think I would.\n"
    "-- Edna St. Vincent Milay (1892 - 1950)\n";


/**
 * Perform Hamming unit tests.
 */
static void hamming(void) {
    uint8_t nib;
    uint8_t sym[0x10];
    uint8_t dat[0x10];
    uint8_t off[0x10];
    uint16_t byt;
    int val;
    uint8_t inp;
    uint8_t bad;
    uint8_t fix;
    uint8_t err;
    uint8_t out;
    int cnt[3];
    char buf[sizeof(TXT) * 2];
    char txt[sizeof(TXT)];
    uint64_t ran;

    /*
     * Test encoding and validity checking for all possible data nibbles
     * against all possible valid symbol bytes.
     */

    TEST();

    for (nib = 0x0; nib <= 0xf; ++nib) {
        sym[nib] = diminuto_hamming84_nib2sym(nib);
    }
    for (nib = 0x0; nib <= 0xf; ++nib) {
        ASSERT(sym[nib] == SYM[nib]);
    }
    for (nib = 0x0; nib <= 0xf; ++nib) {
        ASSERT(diminuto_hamming84_sym_is_valid(sym[nib]));
    }

    STATUS();

    /*
     * Test validity checking across all possible symbol bytes, both valid
     * and invalid.
     */

    TEST();

    for (byt = 0x00; byt <= 0xff; ++byt) {
        val = 0;
        for (nib = 0x0; nib <= 0xf; ++nib) {
            if (byt == SYM[nib]) {
                val = !0;
                break;
            }
        }
        ASSERT(diminuto_hamming84_sym_is_valid(byt) == val);
    }

    STATUS();

    /*
     * Test decoding across all possible valid symbols.
     */

    TEST();

    for (nib = 0x0; nib <= 0xf; ++nib) {
        dat[nib] = diminuto_hamming84_sym2dat(sym[nib]);
    }
    for (nib = 0x0; nib <= 0xf; ++nib) {
        ASSERT(dat[nib] == nib);
    }

    STATUS();

    /*
     * Test offset computation against all possible valid symbols.
     */

    TEST();

    for (nib = 0x0; nib <= 0xf; ++nib) {
        off[nib] = diminuto_hamming84_sym2off(sym[nib]);
    }
    for (nib = 0x0; nib <= 0xf; ++nib) {
        ASSERT(off[nib] > 7);
    }

    STATUS();

    /*
     * Test offset computation and validity checking against all possible
     * single bit errors in other than the P4 bit, which is a special case.
     */

    TEST();

    for (nib = 0x0; nib <= 0xf; ++nib) {
        for (bad = 1; bad <= 7; ++bad) {
            inp = SYM[nib] ^ (1 << bad);
            ASSERT(diminuto_hamming84_sym_is_valid(inp) == 0);
            fix = diminuto_hamming84_sym2off(inp);
            ASSERT(fix == bad);
            inp ^= (1 << fix);
            ASSERT(diminuto_hamming84_sym_is_valid(inp) == !0);
        }
    }

    STATUS();

    /*
     * Test decoding across all possible single bit errors across all bits,
     * including the P4 bit.
     */

    TEST();

    for (nib = 0x0; nib <= 0xf; ++nib) {
        for (bad = 0; bad <= 7; ++bad) {
            inp = SYM[nib];
            ASSERT(diminuto_hamming84_sym_is_valid(inp) == !0);
            out = diminuto_hamming84_sym2nib(inp, &err);
            ASSERT(out == nib);
            ASSERT(err == 0);
            inp = SYM[nib] ^ (1 << bad);
            ASSERT(diminuto_hamming84_sym_is_valid(inp) == 0);
            out = diminuto_hamming84_sym2nib(inp, &err);
            ASSERT(out == nib);
            ASSERT(err == 1);
        }
    }

    STATUS();

    /*
     * Print a table of all possible symbols containing the symbol byte
     * ("SYM"), it's data nibble ("DAT"), the number of bit errors detected
     * if any ("BAD"), and the repaired symbol byte ("FIX") if it exists.
     * As a side effect, this checks for legitimate return values across
     * all possible eight-bit values.
     */

    TEST();

    cnt[0] = 0;
    cnt[1] = 0;
    cnt[2] = 0;
    CHECKPOINT("%4s %4s %4s %4s\n", "SYM", "DAT", "BAD", "FIX");
    for (byt = 0x00; byt <= 0xff; ++byt) {
        out = diminuto_hamming84_sym2nib(byt, &err);
        ASSERT((err == 0) || (err == 1) || ((err == 2) && (out == 0x00)));
        ++cnt[err];
        inp = diminuto_hamming84_nib2sym(out);
        switch (err) {
        case 0:
            CHECKPOINT("0x%2.2x 0x%2.2x\n", byt, out);
            break;
        case 1:
            CHECKPOINT("0x%2.2x 0x%2.2x %4d 0x%2.2x\n", byt, out, err, inp);
            break;
        case 2:
            CHECKPOINT("0x%2.2x 0x%2.2x %4d\n", byt, out, err);
            break;
        }
    }
    putchar('\n');
    for (err = 0; err <= 2; ++err) {
        CHECKPOINT("F(%u)=%d=%.0f%%\n", err, cnt[err], (100.0 * cnt[err]) / 256.0);
    }
    putchar('\n');

    STATUS();

    /*
     * Here is an example of sending and receiving symbols with zero or one
     * bit errors.
     */

    /*
     * Transmitter.
     */

    TEST();

    puts(TXT);
    putchar('\n');

    for (byt = 0; byt < sizeof(TXT); ++byt) {
        /* High order nibble. */
        inp = TXT[byt] >> 4;
        out = diminuto_hamming84_nib2sym(inp);
        ran = rand();
        fix = (ran * 15) / RAND_MAX; /* [0..RAND_MAX] => [0..15] */
        ASSERT((0 <= fix) && (fix <= 15));
        out ^= (1 << fix); /* [0..7]: single bit error, [8..15]: no error */
        buf[byt * 2] = out;
        /* Low order nibble. */
        inp = TXT[byt] & 0x0f;
        out = diminuto_hamming84_nib2sym(inp);
        ran = rand();
        fix = (ran * 15) / RAND_MAX; /* [0..RAND_MAX] => [0..15] */
        ASSERT((0 <= fix) && (fix <= 15));
        out ^= (1 << fix); /* [0..7]: single bit error, [8..15]: no error */
        buf[(byt * 2) + 1] = out;
    }

    STATUS();

    /*
     * Receiver.
     */

    TEST();

    cnt[0] = 0;
    cnt[1] = 0;
    cnt[2] = 0;

    for (byt = 0; byt < sizeof(buf); ++byt) {
        inp = buf[byt];
        err = 3;
        out = diminuto_hamming84_sym2nib(inp, &err);
        ASSERT((0 <= err) && (err <= 2));
        ++cnt[err];
        ASSERT((0x00 <= out) && (out <= 0x0f));
        if (byt & 0x1) {
            /* Low order nibble. */
            fix |= out;
            txt[byt / 2] = fix;
        } else {
            /* High order nibble. */
            fix = out << 4;
        }
    }
    ASSERT(cnt[2] == 0);
    ASSERT(memcmp(TXT, txt, sizeof(TXT)) == 0);

    puts(txt);
    putchar('\n');

    for (err = 0; err <= 2; ++err) {
        CHECKPOINT("F(%u)=%d=%.0f%%\n", err, cnt[err], (100.0 * cnt[err]) / sizeof(buf));
    }

    STATUS();
}

int main(void) {

    SETLOGMASK();

    hamming();

    EXIT();
}
