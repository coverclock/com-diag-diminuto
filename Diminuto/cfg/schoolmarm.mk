# vi: set ts=4 shiftwidth=4:
# Copyright 2014-2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# schoolmarm: Windows 8.1 Pro running Ubuntu Cygwin 2.850.

# IMPORTANT SAFETY TIP: I had to copy all the DLLs from the library directory
# to the current directory, otherwise the shell couldn't find them. Still
# trying to figure that one out. LD_LIBRARY_PATH seemed to have no effect
# despite the Cygwin documentation.

MACHINE				:=	x86_64
ARCH				:=	x86_64
OS					:=	cygwin
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=
KERNEL_DIR			:=
CPPARCH				:=
CARCH				:=	-rdynamic
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
SO					:=	dll
