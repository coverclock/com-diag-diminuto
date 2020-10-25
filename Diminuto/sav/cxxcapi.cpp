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

int main(void)
{
    DIMINUTO_LOG_NOTICE("Hello, world!\n");
}
