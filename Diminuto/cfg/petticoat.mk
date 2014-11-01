# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# petticoat

ARCH				:=	x86_64
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-cros-$(OS)-gnu-
KERNELCHAIN			:=
KERNEL_REV			:=	3.14
#KERNEL_REV			:=	3.8
KERNEL_DIR			:=	$(HOME)/trunk/src/third_party/kernel/$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME)/trunk/chroot/build/x86-generic/usr/include
CARCH				:=	-rdynamic -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=	O=$(HOME)/trunk/chroot/build/x86-generic/var/cache/portage/sys-kernel/chromeos-kernel-3_14
#KERNELARCH			:=	O=$(HOME)/trunk/chroot/build/x86-generic/var/cache/portage/sys-kernel/chromeos-kernel-3_8
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
