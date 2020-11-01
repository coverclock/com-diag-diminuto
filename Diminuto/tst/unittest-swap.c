/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Swap feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Swap feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_swap.h"
#include <stdint.h>

int main(void)
{

    SETLOGMASK();

#define TEST_SWAP(_T_, _A_, _B_) \
    { \
	    _T_ a = (_T_)(_A_); \
	    _T_ b = (_T_)(_B_); \
	    TEST(); \
        diminuto_dump(stderr, &a, sizeof(a)); \
        diminuto_dump(stderr, &b, sizeof(b)); \
	    EXPECT(a == (_T_)(_A_)); \
	    EXPECT(b == (_T_)(_B_)); \
	    DIMINUTO_SWAP(a, b); \
        diminuto_dump(stderr, &a, sizeof(a)); \
        diminuto_dump(stderr, &b, sizeof(b)); \
	    EXPECT(a == (_T_)(_B_)); \
	    EXPECT(b == (_T_)(_A_)); \
	    DIMINUTO_SWAP(a, b); \
        diminuto_dump(stderr, &a, sizeof(a)); \
        diminuto_dump(stderr, &b, sizeof(b)); \
	    EXPECT(a == (_T_)(_A_)); \
	    EXPECT(b == (_T_)(_B_)); \
	    STATUS(); \
    }

    TEST_SWAP(int8_t, 0xa5, 0x5a);
    TEST_SWAP(uint8_t, 0xa5, 0x5a);

    TEST_SWAP(int16_t, 0xa5a5, 0x5a5a);
    TEST_SWAP(uint16_t, 0xa5a5, 0x5a5a);

    TEST_SWAP(int32_t, 0xa5a5a5a5, 0x5a5a5a5a);
    TEST_SWAP(uint32_t, 0xa5a5a5a5, 0x5a5a5a5a);

    TEST_SWAP(int64_t, 0xa5a5a5a5a5a5a5a5LL, 0x5a5a5a5a5a5a5a5aLL);
    TEST_SWAP(uint64_t, 0xa5a5a5a5a5a5a5a5ULL, 0x5a5a5a5a5a5a5a5aULL);

    EXIT();
}
