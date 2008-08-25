#! /bin/sh
# Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA
# Licensed under the GPL V2
# Chip Overclock <coverclock@diag.com>

. `which diminuto`

export LD_LIBRARY_PATH=$(TOOLCHAIN)/lib:$(TOOLCHAIN)/usr/arm-linux-uclibc/lib:$(TOOLCHAIN)/usr/lib:$(TOOLCHAIN)/usr/local/lib:$(DESPERADO):$(FICL)
