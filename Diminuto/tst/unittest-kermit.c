/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Kermit feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Kermit feature.
 */

#include "com/diag/diminuto/diminuto_kermit.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "../src/diminuto_framer.h"
#include <ctype.h>
#include <string.h>

/*
 * Returns true if the octet is a Framer (HDLC) special token.
 */
static bool special(uint8_t token)
{
    bool result = false;

    switch (token) {
    case FLAG:
    case ESCAPE:
    case XON:
    case XOFF:
    case RESERVED:
        result = true;
        break;
    default:
        break;
    }

    return result;
}

/*
 * da Cruz, p. 29, "6.3 Alternate Block Check Types"
 */
static uint16_t dacruz(const void * buffer, size_t length)
{
    uint16_t crc = 0;
    uint16_t qq;
    uint8_t cc;
    uint8_t * pp;

    pp = (uint8_t *)buffer;
    while (length--) {
        cc = *(pp++);
        qq = (crc ^ cc) & 15;
        crc = (crc / 16) ^ (qq * 4225);
        qq = (crc ^ (cc / 16)) & 15;
        crc = (crc / 16) ^ (qq * 4225);
    }

    return crc;
}

int main(void)
{
    SETLOGMASK();

    {
        unsigned int ii;
        uint8_t token;

        /*
         * Make sure our own test function works as expected.
         */

        TEST();

        for (ii = 0x00; ii <= 0xff; ++ii) {
            token = ii;
            switch (token) {
            case '~':
            case '}':
            case '\x11':
            case '\x13':
            case (uint8_t)'\xf8':
                ASSERT(special(token));
                break;
            default:
                ASSERT(!special(token));
                break;
            }
        }

        STATUS();
    }

    {
        uint16_t dd;
        uint16_t rr;
        unsigned char cc;

        TEST();

        for (dd = 0x00; dd <= 0x0f; ++dd) {
            cc = diminuto_kermit_tochar(dd);
            ASSERT(cc == (' ' + dd));
            ASSERT(isprint(cc));
            ASSERT(diminuto_kermit_firstisvalid(cc));
            ASSERT(!special(cc));
            rr = diminuto_kermit_unchar(cc);
            ASSERT(dd == rr);
        }

        STATUS();
    }

    {
        uint16_t dd;
        uint16_t rr;
        unsigned char cc;

        TEST();

        for (dd = 0x00; dd <= 0x3f; ++dd) {
            cc = diminuto_kermit_tochar(dd);
            ASSERT(cc == (' ' + dd));
            ASSERT(isprint(cc));
            ASSERT(diminuto_kermit_secondisvalid(cc));
            ASSERT(!special(cc));
            rr = diminuto_kermit_unchar(cc);
            ASSERT(dd == rr);
        }

        STATUS();
    }

    {
        uint16_t dd;
        uint16_t rr;
        unsigned char cc;

        TEST();

        for (dd = 0x00; dd <= 0x3f; ++dd) {
            cc = diminuto_kermit_tochar(dd);
            ASSERT(cc == (' ' + dd));
            ASSERT(isprint(cc));
            ASSERT(diminuto_kermit_thirdisvalid(cc));
            ASSERT(!special(cc));
            rr = diminuto_kermit_unchar(cc);
            ASSERT(dd == rr);
        }

        STATUS();
    }

    {
        unsigned int ii;
        unsigned char cc;

        for (ii = '\0'; ii < ' '; ++ii) {
            cc = ii;
            ASSERT(!diminuto_kermit_firstisvalid(cc));
            ASSERT(!diminuto_kermit_secondisvalid(cc));
            ASSERT(!diminuto_kermit_thirdisvalid(cc));
        }

        for (ii = ' '; ii <= '/'; ++ii) {
            cc = ii;
            ASSERT(diminuto_kermit_firstisvalid(cc));
            ASSERT(diminuto_kermit_secondisvalid(cc));
            ASSERT(diminuto_kermit_thirdisvalid(cc));
            ASSERT(!special(cc));
        }

        for (ii = '0'; ii <= '_'; ++ii) {
            cc = ii;
            ASSERT(!diminuto_kermit_firstisvalid(cc));
            ASSERT(diminuto_kermit_secondisvalid(cc));
            ASSERT(diminuto_kermit_thirdisvalid(cc));
            ASSERT(!special(cc));
        }

        for (ii = '`'; ii <= 255; ++ii) {
            cc = ii;
            ASSERT(!diminuto_kermit_firstisvalid(cc));
            ASSERT(!diminuto_kermit_secondisvalid(cc));
            ASSERT(!diminuto_kermit_thirdisvalid(cc));
        }
    }

    {
        TEST();

        ASSERT(diminuto_kermit_chars2crc(' ', ' ', ' ') == 0);

        STATUS();
    }

    {
        uint16_t crc;
        uint16_t rev;
        unsigned char aa;
        unsigned char bb;
        unsigned char cc;

        TEST();

        /*
         * da Cruz, p. 29, "6.3 Alternate Block Check Types"
         */

        crc = 0154321;
        aa = '\0';
        bb = '\0';
        cc = '\0';
        diminuto_kermit_crc2chars(crc, &aa, &bb, &cc);
        ASSERT(aa == '-');
        ASSERT(bb == 'C');
        ASSERT(cc == '1');
        rev = diminuto_kermit_chars2crc(aa, bb, cc);
        ASSERT(crc == rev);

        STATUS();
    }

    {
        unsigned int ii;
        uint16_t crc;
        uint16_t rev;
        unsigned char aa;
        unsigned char bb;
        unsigned char cc;
        unsigned char na = 0xff;
        unsigned char nb = 0xff;
        unsigned char nc = 0xff;
        unsigned char xa = 0x00;
        unsigned char xb = 0x00;
        unsigned char xc = 0x00;

        TEST();

        for (ii = 0; ii <= 0xffff; ++ii) {
            crc = ii;
            aa = '\0';
            bb = '\0';
            cc = '\0';
            diminuto_kermit_crc2chars(crc, &aa, &bb, &cc);
            ASSERT(isprint(aa));
            ASSERT(isprint(bb));
            ASSERT(isprint(cc));
            ASSERT(diminuto_kermit_firstisvalid(aa));
            ASSERT(diminuto_kermit_secondisvalid(bb));
            ASSERT(diminuto_kermit_thirdisvalid(cc));
            if (aa < na) { na = aa; }
            if (bb < nb) { nb = bb; }
            if (cc < nc) { nc = cc; }
            if (aa > xa) { xa = aa; }
            if (bb > xb) { xb = bb; }
            if (cc > xc) { xc = cc; }
            rev = diminuto_kermit_chars2crc(aa, bb, cc);
            ASSERT(crc == rev);
        }

        /*
         * a: (0x00 + 0x20) == ' ' .. (0x0f + 0x20) == 0x2f == '/'
         * b: (0x00 + 0x20) == ' ' .. (0x3f + 0x20) == 0x5f == '_'
         * c: (0x00 + 0x20) == ' ' .. (0x3f + 0x20) == 0x5f == '_'
         */

        CHECKPOINT("a:[0x%x..0x%x] b:[0x%x..0x%x] c:[0x%x..0x%x]\n", na, xa, nb, xb, nc, xc);
        ASSERT(na == ' ');
        ASSERT(nb == ' ');
        ASSERT(nc == ' ');
        ASSERT(xa == '/');
        ASSERT(xb == '_');
        ASSERT(xc == '_');

        STATUS();

    }

    {
        unsigned int ii;
        uint16_t crc;
        uint16_t rev;
        unsigned char aa;
        unsigned char bb;
        unsigned char cc;

        TEST();

        for (ii = 0; ii <= 0xffff; ++ii) {
            crc = ii;
            aa = '\0';
            bb = '\0';
            cc = '\0';
            diminuto_kermit_crc2chars(crc, &aa, &bb, &cc);
            ASSERT(isprint(aa));
            ASSERT(isprint(bb));
            ASSERT(isprint(cc));
            ASSERT(diminuto_kermit_firstisvalid(aa));
            ASSERT(diminuto_kermit_secondisvalid(bb));
            ASSERT(diminuto_kermit_thirdisvalid(cc));
            ASSERT((' ' <= aa) && (aa <= '/'));
            ASSERT((' ' <= bb) && (bb <= '_'));
            ASSERT((' ' <= cc) && (cc <= '_'));
            rev = diminuto_kermit_chars2crc(aa, bb, cc);
            ASSERT(crc == rev);
        }

        STATUS();
    }

    {
        unsigned char aa;
        unsigned char bb;
        unsigned char cc;
        unsigned char dd;
        unsigned char ee;
        unsigned char ff;
        uint16_t crc;
        uint16_t rev;

        TEST();

        for (aa = ' '; aa <= '/'; ++aa) {
            for (bb = ' '; bb <= '_'; ++bb) {
                for (cc = ' '; cc <= '_'; ++cc) {
                    ASSERT(isprint(aa));
                    ASSERT(isprint(bb));
                    ASSERT(isprint(cc));
                    ASSERT(diminuto_kermit_firstisvalid(aa));
                    ASSERT(diminuto_kermit_secondisvalid(bb));
                    ASSERT(diminuto_kermit_thirdisvalid(cc));
                    crc = diminuto_kermit_chars2crc(aa, bb, cc);
                    dd = '\0';
                    ee = '\0';
                    ff = '\0';
                    diminuto_kermit_crc2chars(crc, &dd, &ee, &ff);
                    ASSERT(isprint(dd));
                    ASSERT(isprint(ee));
                    ASSERT(isprint(ff));
                    ASSERT(diminuto_kermit_firstisvalid(dd));
                    ASSERT(diminuto_kermit_secondisvalid(ee));
                    ASSERT(diminuto_kermit_thirdisvalid(ff));
                    ASSERT(aa == dd);
                    ASSERT(bb == ee);
                    ASSERT(cc == ff);
                    rev = diminuto_kermit_chars2crc(dd, ee, ff);
                    ASSERT(crc == rev);
                }
            }
        }

        STATUS();
    }

    {
        uint16_t crc;
        uint16_t ref;

        TEST();

        crc = 0;
        crc = diminuto_kermit_16((void *)0, 0, crc);
        ref = dacruz((void *)0, 0);
        CHECKPOINT("0: CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);
        EXPECT(crc == 0x0000);

        STATUS();
    }

    {
        uint16_t crc;
        uint16_t ref;
        const char data[] = { 'A', };

        TEST();

        crc = 0;
        crc = diminuto_kermit_16(data, sizeof(data), crc);
        ref = dacruz(data, sizeof(data));
        CHECKPOINT("A: CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);
        EXPECT(crc == 0x538d);

        STATUS();
    }

    {
        uint16_t crc;
        uint16_t ref;
        const char data[] = "123456789";

        TEST();

        crc = 0;
        crc = diminuto_kermit_16(data, strlen(data), crc);
        ref = dacruz(data, strlen(data));
        CHECKPOINT("123456789: CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);
        EXPECT(crc == 0x2189);

        STATUS();
    }

    {
        uint16_t crc;
        uint16_t ref;
        const char data[] = {
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
            'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
        };

        TEST();

        ASSERT(sizeof(data) == 256);
        crc = 0;
        crc = diminuto_kermit_16(data, sizeof(data), crc);
        ref = dacruz(data, sizeof(data));
        CHECKPOINT("A*: CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);
        EXPECT(crc == 0xe7e2);

        STATUS();
    }

    {
        uint16_t crc;
        uint16_t ref;
        uint8_t data[256];
        uint8_t atad[256];
        int ii;

        TEST();

        for (ii = 0; ii < sizeof(data); ++ii) {
            data[ii] = ii;
        }

        crc = 0;
        crc = diminuto_kermit_16(data, sizeof(data), crc);
        ref = dacruz(data, sizeof(data));
        CHECKPOINT("N: CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);

        crc = 0;
        for (ii = 0; ii < sizeof(data); ++ii) {
            crc = diminuto_kermit_16(&(data[ii]), 1, crc);
        }
        CHECKPOINT("N>: CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);

        for (ii = 0; ii < sizeof(atad); ++ii) {
            atad[ii] = sizeof(atad) - ii - 1;
        }

        crc = 0;
        for (ii = sizeof(data) - 1; ii >= 0; --ii) {
            crc = diminuto_kermit_16(&(data[ii]), 1, crc);
        }
        ref = dacruz(atad, sizeof(atad));
        CHECKPOINT("N< CRC=0x%x REF=0x%x\n", crc, ref);
        EXPECT(crc == ref);

        STATUS();
    }

    EXIT();
}
