# vi: set ts=4 shiftwidth=4:
# Copyright 2008-2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.
#
# host: most Linux/GNU systems hosting the native toolchain.
#
# Adapted from kernel.org:linux-5.15.11/scripts/subarch.include .
#
# Below is a list of directories from kernel.org:linux-5.15.11/arch .
#
#   alpha
#   arc
#   arm
#   arm64
#   csky
#   h8300
#   hexagon
#   ia64
#   m68k
#   microblaze
#   mips
#   nds32
#   nios2
#   openrisc
#   parisc
#   powerpc
#   riscv
#   s390
#   sh
#   sparc
#   um
#   x86
#   xtensa

MACHINE				:=	$(shell uname -m)
ARCH				:=	$(shell uname -m | sed -e 's/i.86/x86/' -e 's/x86_64/x86/' -e 's/sun4u/sparc64/' -e 's/arm.*/arm/' -e 's/sa110/arm/' -e 's/s390x/s390/' -e 's/ppc.*/powerpc/' -e 's/mips.*/mips/' -e 's/sh[234].*/sh/' -e 's/aarch64.*/arm64/' -e 's/riscv.*/riscv/')
OS					:=	$(shell uname -o)
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=	$(shell uname -r)
KERNEL_DIR			:=	/lib/modules/$(KERNEL_REV)/build
GNUARCH				:=	-D__USE_GNU -D_GNU_SOURCE
# Try: sudo apt-get install linux-headers-$(uname -r)
# Or:  sudo apt-get install raspberrypi-kernel-headers
CPPARCH				:=	-isystem /usr/src/linux-headers-$(KERNEL_REV) $(GNUARCH)
CARCH				:=	-rdynamic -fPIC -Wall
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
