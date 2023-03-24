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
#include <ctype.h>
#include <string.h>

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
        uint8_t dd;
        unsigned char cc;
        uint8_t rr;

        TEST();

        for (dd = 0; dd <= 94; ++dd) {
            cc = diminuto_kermit_tochar(dd);
            ASSERT(cc == (dd + 32));
            ASSERT(isprint(cc));
            rr = diminuto_kermit_unchar(cc);
            ASSERT(dd == rr);
        }

        STATUS();
    }

    {
        uint16_t crc;
        unsigned char a;
        unsigned char b;
        unsigned char c;

        TEST();

        /*
         * da Cruz, p. 29, "6.3 Alternate Block Check Types"
         */

        crc = 0154321;
        a = '\0';
        b = '\0';
        c = '\0';
        diminuto_kermit_crc2chars(crc, &a, &b, &c);
        ASSERT(a == '-');
        ASSERT(b == 'C');
        ASSERT(c == '1');

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

    EXIT();
}
