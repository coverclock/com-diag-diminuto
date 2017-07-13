# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# stampede: Nvidia Jetson TK1 running Ubuntu 14.04.

ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-gnueabihf-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	3.10.24-jetson
KERNEL_DIR			:=	$(ROOT_DIR)/linux-$(KERNEL_REV)
CPPARCH				:=	-isystem $(ROOT_DIR)/linux-headers-$(KERNEL_REV)/include
CARCH				:=	-rdynamic -fno-omit-frame-pointer -funwind-tables -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
