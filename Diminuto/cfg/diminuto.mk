# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# diminuto: AT91RM9200-EK running BuildRoot with uClibc.

ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.25.10
KERNEL_DIR			:=	$(ROOT_DIR)/$(OS)-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/$(PROJECT)/builtroot/project_build_arm/$(PROJECT)/$(OS)-$(KERNEL_REV)/include
CARCH				:=	-march=armv4t -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
