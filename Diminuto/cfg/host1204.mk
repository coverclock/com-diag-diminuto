# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# host1204: Dell Inspiron 530 x86 running Ubuntu 12.04 LTS.

ARCH				:=	x86
OS					:=	linux
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=	3.2.0-51
KERNEL_DIR			:=	$(ROOT_DIR)/linux-source-3.2.0
CPPARCH				:=	-isystem /usr/src/linux-headers-$(KERNEL_REV)-generic-pae
CARCH				:=	-fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
SOXXARCH			:=	-L$(OUT)/$(LIB_DIR) -l$(PROJECT)
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
