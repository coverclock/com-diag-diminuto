# vi: set ts=4 shiftwidth=4:
# Copyright 2008-2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# cascada: BeagleBoard C4 running Angstrom.

MACHINE				:=	arm
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-none-$(OS)-gnueabi-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.32.7
KERNEL_DIR			:=	$(HOME_DIR)/arroyo/$(OS)-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
CARCH				:=	-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
