# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# cascada: BeagleBoard C4 running Angstrom.

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
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
