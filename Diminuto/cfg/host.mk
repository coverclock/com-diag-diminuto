# vi: set ts=4 shiftwidth=4:
# Copyright 2008-2019 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# host: most Linux/GNU systems hosting the native toolchain.

# (I have yet to find a reliable way to determine under what
# ARCH for which the host Linux kernel was built. You would
# think uname would do this for you.)

MACHINE				:=	$(shell uname -m)
ifeq ($(MACHINE),x86_64)
ARCH				:=	x86_64
OPTARCH				:=
endif
ifeq ($(MACHINE),armv7l)
ARCH				:=	arm
OPTARCH				:=	-O0
endif
OS					:=	linux
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=	$(shell uname -r)
KERNEL_DIR			:=	/lib/modules/$(KERNEL_REV)/build
# Try: sudo apt-get install linux-headers-$(uname -r)
# Or:  sudo apt-get install raspberrypi-kernel-headers
GNUARCH				:=	-D__USE_GNU -D_GNU_SOURCE
CPPARCH				:=	-isystem /usr/src/linux-headers-$(KERNEL_REV) $(GNUARCH)
CARCH				:=	-rdynamic -fPIC $(OPTARCH)
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
