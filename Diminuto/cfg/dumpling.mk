# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# dumpling

ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-buildroot-$(OS)-uclibcgnueabi-
KERNELCHAIN			:=	$(TOOLCHAIN)
BUILDROOT_REV		:=	2014.08
BUILDROOT_DIR		:=	$(HOME_DIR)/$(TARGET)/buildroot-$(BUILDROOT_REV)
KERNEL_REV			:=	3.12.26
KERNEL_DIR			:=	$(BUILDROOT_DIR)/output/build/linux-353043394c99ed37a4075da228af64468fdf6c06
CPPARCH				:=	-isystem $(BUILDROOT_DIR)/output/build/linux-headers-$(KERNEL_REV)/usr/include
CARCH				:=	-fPIC
CXXARCH				:=	$(CARCH)
#LDARCH				:=	-static -L$(OUT)/$(ARC_DIR)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
