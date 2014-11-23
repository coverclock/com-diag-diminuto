# Copyright 2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# host: Windows 8.1 Pro running Ubuntu Cygwin 2.850.

# To run the unit tests I had to copy the DLLs from the library directory
# to the current directory, otherwise the shell couldn't find them. Still
# trying to figure that one out. LD_LIBRARY_PATH seemed to have no effect.

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
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
SO					:=	dll
