#if 0
g++ -o cxxcapi -I inc sav/cxxcapi.cpp out/host/arc/libdiminuto.a -lpthread -lrt -ldl
#endif

#include "com/diag/diminuto/diminuto_cxxcapi.h"

#include "com/diag/diminuto/diminuto_begin.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_end.h"

#include <stdio.h>

CXXCINLINE void notice1() { DIMINUTO_LOG_NOTICE("Hello, world 1!\n"); }

CXXCAPI void notice2() { DIMINUTO_LOG_NOTICE("Hello, world 2!\n"); }

int main(void)
{
    notice1();
    notice2();
}
