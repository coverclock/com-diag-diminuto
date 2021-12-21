# vi: set ts=4 shiftwidth=4:
# Copyright 2008-2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# diminuto: AT91RM9200-EK running BuildRoot with uClibc.

MACHINE				:=	arm
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
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
