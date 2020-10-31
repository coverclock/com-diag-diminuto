/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Zone portion of the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Zone portion of the Time feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_time.h"

int main(int argc, char ** argv)
{
    diminuto_sticks_t frequency;

    frequency = diminuto_frequency(); /* 1 second */
    frequency *= 3600; /* 1 hour */

    TEST();

    EXPECT(diminuto_time_zonename(frequency * -12) == 'Y');
    EXPECT(diminuto_time_zonename(frequency * -11) == 'X');
    EXPECT(diminuto_time_zonename(frequency * -10) == 'W');
    EXPECT(diminuto_time_zonename(frequency *  -9) == 'V');
    EXPECT(diminuto_time_zonename(frequency *  -8) == 'U');
    EXPECT(diminuto_time_zonename(frequency *  -7) == 'T');
    EXPECT(diminuto_time_zonename(frequency *  -6) == 'S');
    EXPECT(diminuto_time_zonename(frequency *  -5) == 'R');
    EXPECT(diminuto_time_zonename(frequency *  -4) == 'Q');
    EXPECT(diminuto_time_zonename(frequency *  -3) == 'P');
    EXPECT(diminuto_time_zonename(frequency *  -2) == 'O');
    EXPECT(diminuto_time_zonename(frequency *  -1) == 'N');
    EXPECT(diminuto_time_zonename(frequency *   0) == 'Z');
    EXPECT(diminuto_time_zonename(frequency *   1) == 'A');
    EXPECT(diminuto_time_zonename(frequency *   2) == 'B');
    EXPECT(diminuto_time_zonename(frequency *   3) == 'C');
    EXPECT(diminuto_time_zonename(frequency *   4) == 'D');
    EXPECT(diminuto_time_zonename(frequency *   5) == 'E');
    EXPECT(diminuto_time_zonename(frequency *   6) == 'F');
    EXPECT(diminuto_time_zonename(frequency *   7) == 'G');
    EXPECT(diminuto_time_zonename(frequency *   8) == 'H');
    EXPECT(diminuto_time_zonename(frequency *   9) == 'I');
    EXPECT(diminuto_time_zonename(frequency *  10) == 'K');
    EXPECT(diminuto_time_zonename(frequency *  11) == 'L');
    EXPECT(diminuto_time_zonename(frequency *  12) == 'M');

    STATUS();

    TEST();

    EXPECT(diminuto_time_zonename(frequency * -13) == 'J');
    EXPECT(diminuto_time_zonename((frequency / 2) + (frequency / 4)) == 'J'); /* Eucla, Australia +:45 */
    EXPECT(diminuto_time_zonename((frequency *  4) + (frequency / 2)) == 'J'); /* Afghanistan +4:30 */
    EXPECT(diminuto_time_zonename((frequency *  5) + (frequency / 2)) == 'J'); /* India +5:30 */
    EXPECT(diminuto_time_zonename((frequency * 12) + (frequency / 2) + (frequency / 4)) == 'J'); /* Chatham Islands, New Zealand +12:45 */
    EXPECT(diminuto_time_zonename(frequency *  13) == 'J');

    STATUS();

    EXIT();
}
