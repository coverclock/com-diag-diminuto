# vi: set ts=4 shiftwidth=4:
# Copyright 2008-2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# notgnu: testing the build without the GNU options enabled.

MACHINE				:=	x86_64
include cfg/arch.mk
OS					:=	linux
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=	5.11.0-40-generic
KERNEL_DIR			:=	/lib/modules/$(KERNEL_REV)/build
# Try: sudo apt-get install linux-headers-$(uname -r)
# Or:  sudo apt-get install raspberrypi-kernel-headers
GNUARCH				:=	
CPPARCH				:=	-isystem /usr/src/linux-headers-$(KERNEL_REV) $(GNUARCH)
CARCH				:=	-rdynamic -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
