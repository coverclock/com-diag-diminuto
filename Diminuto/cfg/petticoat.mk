# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# petticoat: Acer C720 running ChrUbuntu using ChromiumOS R37 toolchain.
#
# This project really didn't have a completely satisfactory conclusion. I
# wanted to build this library for an Acer C720-3404 ChromeBook, an X86_64
# device that runs ChromeOS 37.0.2062.120 on platform 5978.98.1 and the
# 3.8.11 Linux kernel on the "peppy" motherboard. I tried building the
# ChromiumOS release-R37-5978.B branch but was never able to get it to build
# completely. However, it did build enough that I could at least build the
# user space portions of Diminuto. But I was never able to get them to execute
# under the ChromeOS shell; that appears to be a restriction in the shell itself
# (a security thing). But I was able to run the user space unit tests under
# ChrUbuntu, which runs Ubuntu under a chroot on the Acer. That was sufficient
# for my needs, but not the more general solution I would have hoped for.

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
