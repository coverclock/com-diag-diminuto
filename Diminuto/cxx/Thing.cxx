/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the C++ implementation file for the CXXCAPI test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the C++ implementation file for the CXXCAPI test.
 */

#include "com/diag/diminuto/diminuto_cxxcapi.h"

#include "Thing.hxx"

#include "com/diag/diminuto/diminuto_begin.h"
#   include "com/diag/diminuto/diminuto_unittest.h"
#   include "com/diag/diminuto/diminuto_log.h"
#   include "thing.h"
#include "com/diag/diminuto/diminuto_end.h"

Thing::Thing() { this->data = 0; }

Thing::Thing(int parm) { this->data = parm; }

Thing::~Thing() { }

int Thing::log() { DIMINUTO_LOG_NOTICE("this=%p data=0x%x null=%p\n", this, this->data, (void *)NULL); return this->data; }

CXXCAPI int thing_log(Thing * that) { return thing_get(that)->log(); }

int main(void)
{
    int result = -1;

    TEST();

    Thing thing(0xdeadbeef);

    result = thing.log();
    EXPECT(result == 0xdeadbeef);

    thing.set(0xc0edbabe);

    result = thing.log();
    EXPECT(result == 0xc0edbabe);

    result = logc(&thing);
    DIMINUTO_LOG_NOTICE("result=0x%x\n", result);
    EXPECT(result == 0xc0edbabe);

    STATUS();

    EXIT();
}
