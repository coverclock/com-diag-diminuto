# Copyright 2015 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# betty: Raspberry Pi 2 running Raspbian.

# REFERENCES
#
# https://www.raspberrypi.org/documentation/linux/kernel/building.md
# https://github.com/raspberrypi/tools
# https://github.com/raspberrypi/linux

ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-gnueabihf-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	3.18.11-v7
KERNEL_DIR			:=	$(ROOT_DIR)/linux
CPPARCH				:=	-isystem $(ROOT_DIR)/include
CARCH				:=	-fPIC
CXXARCH				:=	$(CARCH)
#LDARCH				:=	-static
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
