# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# contraption: BeagleBoard C4 running Android 2.2 but with a glibc overlay.

ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-none-$(OS)-gnueabi-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.32
KERNEL_DIR			:=	$(ROOT_DIR)/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
CPPARCH				:=	-isystem $(ROOT_DIR)/include-$(KERNEL_REV)/include
CARCH				:=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp -fPIC
CXXARCH				:=	$(CARCH)
#LDARCH				:=	-static
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
