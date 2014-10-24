# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.h
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.
#
#	make default TARGET=<target>	# Build libraries, unit tests, programs.
#	make most TARGET=<target>		# Build default plus user-space loadable modules.
#	make all TARGET=<target>		# Build most plus kernel-space loadable modules.

########## Customizations

TITLE				:=	Diminuto

TARGET				:=	host# Build for an x86 running Ubuntu 14.04.
#TARGET				:=	diminuto# Build for the AT91RM9200-EK running BuildRoot.
#TARGET				:=	arroyo# Build for the AT91RM9200-EK running roll-your-own.
#TARGET				:=	cascada# Build for the BeagleBoard C4 running Angstrom.
#TARGET				:=	host1204# Build for an x86 running Ubuntu 12.04.
#TARGET				:=	contraption# Build for the BeagleBoard C4 running Android 2.2 but with glibc.
#TARGET				:=	cobbler# Build for the Raspberry Pi version B running Raspbian.
#TARGET				:=	uclibc# Build for the Raspberry Pi Version B running uClibc.
#TARGET				:=	stampede# Build for the Nvidia Jetson TK1 running Ubuntu 14.04.
#TARGET				:=	bionic# Build for CyanogenMod 10.2 on a Nexus-7 running bionic.

MAJOR				:=	25# API changes that may require that applications be modified.
MINOR				:=	3# Only functionality or features added with no legacy API changes.
BUILD				:=	8# Only bugs fixed with no API changes or new functionality.

# Some certification, defense, or intelligence agencies (e.g. the U.S. Federal
# Aviation Administration or FAA) require that software builds for safety
# critical or national security applications generate exactly the same binary
# images bit for bit if the source code has not changed. (This is theoretically
# a more stringent requirement than requiring that checksums or cryptographic
# hashes are the same, although in practice it is the same thing.) This allows
# agency inspectors to verify the integrity of the binary software images. This
# makes embedding timestamps inside compiled translation units problematic.
# If your application has this requirement, you can pass in any fixed string
# for the value of the VINTAGE make variable, and only use the value of this
# symbol as a build time stamp, and you should be able to generate identical
# images with subsequent builds of Diminuto. This string is embedded inside the
# Diminuto "vintage" application. The default build time stamp is an ISO-8601
# complaint string with the time specified in UTC with nanosecond resolution.
VINTAGE				:=	$(shell date -u +%Y-%m-%dT%H:%M:%S.%N%z)

# This stuff all gets embedded in the vintage application.
COPYRIGHT			:=	2008-2014 Digital Aggregates Corporation
LICENSE				:=	GNU Lesser General Public License 2.1
CONTACT				:=	coverclock@diag.com
HOMEPAGE			:=	http://www.diag.com/navigation/downloads/$(TITLE).html

# This is where I store collateral associated with projects that I have
# downloaded off the web and use without alteration. Examples: Linux kernel
# sources, toolchains, etc.
HOME_DIR			:=	$(HOME)/Projects

########## Directories

ARC_DIR				:=	arc# Archive files for static linking
BIN_DIR				:=	bin# Utility source files or stripped executable binaries
DEP_DIR				:=	dep# Generated dependencies and other make files
DOC_DIR				:=	doc# Documentation
DRV_DIR				:=	drv# Loadable kernel modules
ETC_DIR				:=	etc# Miscellaneous files
GEN_DIR				:=	gen# Generated source files
INC_DIR				:=	inc# Header files
LIB_DIR				:=	lib# Shared objects for dynamic linking
MOD_DIR				:=	mod# Loadable user modules
OUT_DIR				:=	out# Build artifacts
SRC_DIR				:=	src# Library source files
SYM_DIR				:=	sym# Unstripped executable binaries
SYS_DIR				:=	sys# Kernel module build directory
TGZ_DIR				:=	tgz# Compressed tarballs
TST_DIR				:=	tst# Unit tests

########## Variables

PROJECT				:=	$(shell echo $(TITLE) | tr '[A-Z]' '[a-z]')
SYMBOL				:=	$(shell echo $(TITLE) | tr '[a-z]' '[A-Z]')

HERE				:=	$(shell pwd)

OUT					:=	$(OUT_DIR)/$(TARGET)

TEMP_DIR			:=	/tmp
ROOT_DIR			:=	$(HOME_DIR)/$(PROJECT)

TIMESTAMP			:=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
DATESTAMP			:=	$(shell date +%Y%m%d)

SVNREPO				:=	svn://graphite/$(PROJECT)
SVNURL				:=	$(SVNREPO)/trunk/$(TITLE)
GITURL				:=	https://github.com/coverclock/com-diag-$(PROJECT).git

PROJECT_A			:=	lib$(PROJECT).a
PROJECTXX_A			:=	lib$(PROJECT)xx.a
PROJECT_SO			:=	lib$(PROJECT).so
PROJECTXX_SO		:=	lib$(PROJECT)xx.so

PROJECT_LIB			:=	$(PROJECT_SO).$(MAJOR).$(MINOR).$(BUILD)
PROJECTXX_LIB		:=	$(PROJECTXX_SO).$(MAJOR).$(MINOR).$(BUILD)

GENERATED			:=	vintage setup generate
SYNTHESIZED			:=	$(PROJECT)_release.h $(PROJECT)_vintage.h

ALIASES				:=	hex oct ntohs htons ntohl htonl

NEW					:=	dummy
OLD					:=	dummy

########## Configurations

ifeq ($(TARGET),diminuto)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.25.10
KERNEL_DIR			:=	$(HOME_DIR)/$(PROJECT)/$(OS)-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/$(PROJECT)/builtroot/project_build_arm/$(PROJECT)/$(OS)-$(KERNEL_REV)/include
CARCH				:=	-march=armv4t -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),arroyo)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-none-$(OS)-gnueabi-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.26.3
KERNEL_DIR			:=	$(HOME_DIR)/arroyo/$(OS)-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
CARCH				:=	-march=armv4t -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),cascada)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-none-$(OS)-gnueabi-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.32.7
KERNEL_DIR			:=	$(HOME_DIR)/arroyo/$(OS)-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
CARCH				:=	-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),host1204)
ARCH				:=	x86
OS					:=	linux
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=	3.2.0-51
KERNEL_DIR			:=	$(HOME)/src/linux-source-3.2.0
CPPARCH				:=	-isystem /usr/src/linux-headers-$(KERNEL_REV)-generic-pae
CARCH				:=	-fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),contraption)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-none-$(OS)-gnueabi-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	2.6.32
KERNEL_DIR			:=	$(HOME_DIR)/contraption/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
CPPARCH				:=	-isystem $(HOME_DIR)/contraption/include-$(KERNEL_REV)/include
CARCH				:=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp -fPIC
CXXARCH				:=	$(CARCH)
#LDARCH				:=	-static
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),cobbler)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-gnueabihf-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	rpi-3.6.y
KERNEL_DIR			:=	$(HOME_DIR)/cobbler/linux-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/cobbler/include-$(KERNEL_REV)/include
CARCH				:=	-fPIC
CXXARCH				:=	$(CARCH)
#LDARCH				:=	-static
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),uclibc)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-buildroot-$(OS)-uclibcgnueabihf-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	rpi-3.6.y
KERNEL_DIR			:=	$(HOME_DIR)/cobbler/linux-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/cobbler/include-$(KERNEL_REV)/include
CARCH				:=	-fPIC
CXXARCH				:=	$(CARCH)
#LDARCH				:=	-static -L$(OUT)/$(ARC_DIR)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),host)
ARCH				:=	x86
OS					:=	linux
TOOLCHAIN			:=
KERNELCHAIN			:=
KERNEL_REV			:=	3.13.0-24
KERNEL_DIR			:=	$(HOME)/src/linux-source-3.13.0
CPPARCH				:=	-isystem /usr/src/linux-headers-$(KERNEL_REV)-generic
CARCH				:=	-rdynamic -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),stampede)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-gnueabihf-
KERNELCHAIN			:=	$(TOOLCHAIN)
KERNEL_REV			:=	3.10.24-jetson
KERNEL_DIR			:=	$(HOME_DIR)/stampede/linux-$(KERNEL_REV)
CPPARCH				:=	-isystem $(HOME_DIR)/stampede/linux-headers-$(KERNEL_REV)/include
CARCH				:=	-rdynamic -fno-omit-frame-pointer -funwind-tables -fPIC
CXXARCH				:=	$(CARCH)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
MOARCH				:=	-L$(OUT)/$(LIB_DIR)
SOARCH				:=
KERNELARCH			:=
LDLIBRARIES			:=	-lpthread -lrt -ldl -lm
LDXXLIBRARIES		:=	$(LDLIBRARIES)
endif

ifeq ($(TARGET),bionic)
ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-androideabi-
KERNELCHAIN			:=	$(ARCH)-eabi-
KERNEL_REV			:=	3.4.0-cyanogenmod-g7182995
CYANOGENMODPRODUCT	:=	flo
CYANOGENMODKERNEL	:=	google
CYANOGENMODPROCESSOR:=	msm
CYANOGENMODDEVICE	:=	asus
CYANOGENMOD_DIR		:=	$(HOME)/src/cm-10.2
KERNEL_DIR			:=	$(CYANOGENMOD_DIR)/kernel/$(CYANOGENMODKERNEL)/$(CYANOGENMODPROCESSOR)
CPPARCH				:=
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libc/arch-arm/include
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libc/include
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libstdc++/include
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libc/kernel/common
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libc/kernel/arch-arm
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libm/include
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libm/include/arm
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/bionic/libthread_db/include
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/system/core/include
CPPARCH				+=	-include $(CYANOGENMOD_DIR)/build/core/combo/include/arch/linux-arm/AndroidConfig.h
CPPARCH				+=	-I $(CYANOGENMOD_DIR)/build/core/combo/include/arch/linux-arm/
CPPARCH				+=	-isystem $(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/KERNEL_OBJ/include#/generated/autoconf.h
CPPARCH				+=	-DANDROID
CPPARCH				+=	-D_FORTIFY_SOURCE=1
CPPARCH				+=	-DNDEBUG
CPPARCH				+=	-UDEBUG
CPPARCH				+=	-DHAVE_UNISTD_H
CPPARCH				+=	-DHAVE_ERRNO_H
CPPARCH				+=	-DHAVE_NETINET_IN_H
CPPARCH				+=	-DHAVE_SYS_IOCTL_H
CPPARCH				+=	-DHAVE_SYS_MMAN_H
CPPARCH				+=	-DHAVE_SYS_MOUNT_H
CPPARCH				+=	-DHAVE_SYS_PRCTL_H
CPPARCH				+=	-DHAVE_SYS_RESOURCE_H
CPPARCH				+=	-DHAVE_SYS_SELECT_H
CPPARCH				+=	-DHAVE_SYS_STAT_H
CPPARCH				+=	-DHAVE_SYS_TYPES_H
CPPARCH				+=	-DHAVE_STDLIB_H
CPPARCH				+=	-DHAVE_STRDUP
CPPARCH				+=	-DHAVE_MMAP
CPPARCH				+=	-DHAVE_UTIME_H
CPPARCH				+=	-DHAVE_GETPAGESIZE
CPPARCH				+=	-DHAVE_LSEEK64
CPPARCH				+=	-DHAVE_LSEEK64_PROTOTYPE
CPPARCH				+=	-DHAVE_EXT2_IOCTLS
CPPARCH				+=	-DHAVE_LINUX_FD_H
CPPARCH				+=	-DHAVE_TYPE_SSIZE_T
CPPARCH				+=	-DHAVE_SYS_TIME_H
CPPARCH				+=	-DHAVE_SYS_PARAM_H
CPPARCH				+=	-DHAVE_SYSCONF
CPPARCH				+=	-DPREEMPT_ACTIVE=0x40000000#kernel/google/msm/arch/arm/include/asm/thread_info.h
CARCH				:=
CARCH				+=	-fno-exceptions
CARCH				+=	-Wno-multichar
CARCH				+=	-msoft-float
CARCH				+=	-fpic
CARCH				+=	-fPIE
CARCH				+=	-ffunction-sections
CARCH				+=	-fdata-sections
CARCH				+=	-funwind-tables
CARCH				+=	-fstack-protector
CARCH				+=	-Wa,--noexecstack
CARCH				+=	-Werror=format-security
CARCH				+=	-fno-short-enums
CARCH				+=	-mcpu=cortex-a9
CARCH				+=	-mfloat-abi=softfp
CARCH				+=	-mfpu=neon
CARCH				+=	-Wno-unused-but-set-variable
CARCH				+=	-fno-builtin-sin
CARCH				+=	-fno-strict-volatile-bitfields
CARCH				+=	-Wno-psabi
CARCH				+=	-mthumb-interwork
CARCH				+=	-fmessage-length=0
CARCH				+=	-W
CARCH				+=	-Wall
CARCH				+=	-Wno-unused
CARCH				+=	-Winit-self
CARCH				+=	-Wpointer-arith
CARCH				+=	-Werror=return-type
CARCH				+=	-Werror=non-virtual-dtor
CARCH				+=	-Werror=address
CARCH				+=	-Werror=sequence-point
CARCH				+=	-Wstrict-aliasing=2
CARCH				+=	-fgcse-after-reload
CARCH				+=	-frerun-cse-after-loop
CARCH				+=	-frename-registers
CARCH				+=	-mthumb
CARCH				+=	-Os
CARCH				+=	-fomit-frame-pointer
CARCH				+=	-fno-strict-aliasing
CARCH				+=	-W
CARCH				+=	-Wall
CXXARCH				:=	$(CARCH)
LDARCH				:=
LDARCH				+=	-nostdlib
LDARCH				+=	-Bdynamic
LDARCH				+=	-fPIE
LDARCH				+=	-pie
LDARCH				+=	-Wl,-dynamic-linker,/system/bin/linker
LDARCH				+=	-Wl,--gc-sections
LDARCH				+=	-Wl,-z,nocopyreloc
LDARCH				+=	-L${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
LDARCH				+=	-L$(OUT)/$(LIB_DIR)
LDARCH				+=	-Wl,-rpath-link=${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
LDARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtbegin_dynamic.o
LDARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/STATIC_LIBRARIES/libcompiler_rt-extras_intermediates/libcompiler_rt-extras.a
LDARCH				+=	-L$(OUT)/$(LIB_DIR)
LDARCH				+=	-Wl,-z,noexecstack
LDARCH				+=	-Wl,-z,relro
LDARCH				+=	-Wl,-z,now
LDARCH				+=	-Wl,--warn-shared-textrel
LDARCH				+=	-Wl,--fatal-warnings
LDARCH				+=	-Wl,--icf=safe
LDARCH				+=	-Wl,--fix-cortex-a8
LDARCH				+=	-Wl,--no-undefined
LDARCH				+=	${CYANOGENMOD_DIR}/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/libgcc.a
LDARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtend_android.o
MOARCH				:=
MOARCH				+=	-nostdlib
MOARCH				+=	-Wl,--gc-sections
MOARCH				+=	-Wl,-shared,-Bsymbolic
MOARCH				+=	-L${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
MOARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtbegin_so.o
MOARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/STATIC_LIBRARIES/libcompiler_rt-extras_intermediates/libcompiler_rt-extras.a
MOARCH				+=	-L$(OUT)/$(LIB_DIR)
MOARCH				+=	-Wl,-z,noexecstack
MOARCH				+=	-Wl,-z,relro
MOARCH				+=	-Wl,-z,now
MOARCH				+=	-Wl,--warn-shared-textrel
MOARCH				+=	-Wl,--fatal-warnings
MOARCH				+=	-Wl,--icf=safe
MOARCH				+=	-Wl,--fix-cortex-a8
MOARCH				+=	-Wl,--no-undefined
MOARCH				+=	${CYANOGENMOD_DIR}/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/libgcc.a
MOARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtend_so.o
SOARCH				:=
SOARCH				+=	-nostdlib
SOARCH				+=	-Wl,--gc-sections
SOARCH				+=	-Wl,-shared,-Bsymbolic
SOARCH				+=	-L${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
SOARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtbegin_so.o
SOARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/STATIC_LIBRARIES/libcompiler_rt-extras_intermediates/libcompiler_rt-extras.a
SOARCH				+=	-Wl,-z,noexecstack
SOARCH				+=	-Wl,-z,relro
SOARCH				+=	-Wl,-z,now
SOARCH				+=	-Wl,--warn-shared-textrel
SOARCH				+=	-Wl,--fatal-warnings
SOARCH				+=	-Wl,--icf=safe
SOARCH				+=	-Wl,--fix-cortex-a8
SOARCH				+=	-Wl,--no-undefined
SOARCH				+=	${CYANOGENMOD_DIR}/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/libgcc.a
SOARCH				+=	${CYANOGENMOD_DIR}/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtend_so.o
KERNELARCH			:=	O=${CYANOGENMOD_DIR}/out/target/product/flo/obj/KERNEL_OBJ
LDLIBRARIES			:=
LDLIBRARIES			+=	-llog
LDLIBRARIES			+=	-ldl
LDLIBRARIES			+=	-lc
LDLIBRARIES			+=	-lm
LDXXLIBRARIES		:=
LDXXLIBRARIES		+=	-llog
LDXXLIBRARIES		+=	-ldl
LDXXLIBRARIES		+=	-lc
LDXXLIBRARIES		+=	-lstdc++
LDXXLIBRARIES		+=	-lm
endif

########## Commands and Option Flags

CROSS_COMPILE		:=	$(TOOLCHAIN)

CC					:=	$(CROSS_COMPILE)gcc
CXX					:=	$(CROSS_COMPILE)g++
AR					:=	$(CROSS_COMPILE)ar
RANLIB				:=	$(CROSS_COMPILE)ranlib
STRIP				:=	$(CROSS_COMPILE)strip

CDEFINES			:=

ARFLAGS				:=	crsv
#CPPFLAGS			:=	$(CDEFINES) -I $(SRC_DIR) -I $(INC_DIR) $(CPPARCH)
CPPFLAGS			:=	$(CDEFINES) -iquote $(SRC_DIR) -iquote $(INC_DIR) -iquote $(OUT)/$(INC_DIR) $(CPPARCH)
CFLAGS				:=	$(CARCH) -g
#CFLAGS				:=	$(CARCH) -O3
CXXFLAGS			:=	$(CXXARCH) -g
#CXXFLAGS			:=	$(CXXARCH) -O3
CPFLAGS				:=	-i
MVFLAGS				:=	-i
LDFLAGS				:=	$(LDARCH) -l$(PROJECT) $(LDLIBRARIES)
LDXXFLAGS			:=	$(LDARCH) -l$(PROJECT)xx -l$(PROJECT) $(LDXXLIBRARIES)
MOFLAGS				:=	$(MOARCH) -l$(PROJECT) $(LDLIBRARIES)
MOXXFLAGS			:=	$(MOARCH) -l$(PROJECT)xx -l$(PROJECT) $(LDXXLIBRARIES)
SOFLAGS				:=	$(SOARCH) $(LDLIBRARIES)
SOXXFLAGS			:=	$(SOARCH) $(LDXXLIBRARIES)

BROWSER				:=	firefox

########## Build Artifacts

TARGETOBJECTS		:=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.c))))
TARGETOBJECTSXX		:=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.cpp))))
TARGETDRIVERS		:=	$(addprefix $(OUT)/,$(addsuffix .ko,$(basename $(wildcard $(DRV_DIR)/*.c))))
TARGETMODULES		:=	$(addprefix $(OUT)/,$(addsuffix .so,$(basename $(wildcard $(MOD_DIR)/*.c))))
TARGETSCRIPTS		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.sh)))
TARGETBINARIES		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.c)))
TARGETGENERATED		:=	$(addprefix $(OUT)/$(BIN_DIR)/,$(GENERATED))
TARGETSYNTHESIZED	:=	$(addprefix $(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/,$(SYNTHESIZED))

TARGETALIASES		:=	$(addprefix $(OUT)/$(BIN_DIR)/,$(ALIASES))
TARGETUNITTESTS		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.c)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.cpp)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.sh)))

TARGETARCHIVE		:=	$(OUT)/$(ARC_DIR)/$(PROJECT_A)
TARGETARCHIVEXX		:=	$(OUT)/$(ARC_DIR)/$(PROJECTXX_A)
TARGETSHARED		:=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR).$(BUILD)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO)
TARGETSHAREDXX		:=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR).$(BUILD)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO)

TARGETLIBRARIES		:=	$(TARGETARCHIVE) $(TARGETSHARED)
TARGETLIBRARIESXX	:=	$(TARGETARCHIVEXX) $(TARGETSHAREDXX)
TARGETPROGRAMS		:=	$(TARGETBINARIES) $(TARGETALIASES) $(TARGETUNITTESTS) $(TARGETGENERATED) $(TARGETSCRIPTS)
TARGETDEFAULT		:=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) $(TARGETPROGRAMS)
TARGETMOST			:=	$(TARGETDEFAULT) $(TARGETMODULES)
TARGETALL			:=	$(TARGETMOST) $(TARGETDRIVERS)

########## Main Entry Points

.PHONY:	default all dist clean pristine

default:	$(TARGETDEFAULT)

most:	$(TARGETMOST)

all:	$(TARGETALL)

dist:	distribution

clean:
	rm -rf $(OUT)

pristine:	clean
	rm -rf $(OUT_DIR)

########## Packaging and Distribution

.PHONY:	package

PACKAGE := $(OUT)/$(TGZ_DIR)/$(PROJECT)-$(TARGET)

MANIFEST := $(BIN_DIR) $(DRV_DIR) $(LIB_DIR) $(MOD_DIR) $(TST_DIR)

package $(PACKAGE).tgz:
	D=`dirname $(PACKAGE)`; mkdir -p $$D; \
	for M in $(MANIFEST); do mkdir -p $(OUT)/$$M; done; \
	T=`mktemp -d "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
	B=`basename $(PACKAGE)`; mkdir -p $$T/$$B; \
	tar -C $(OUT) --exclude-vcs -cvf - $(MANIFEST) | tar -C $$T/$$B -xvf -; \
	tar -C $$T --exclude-vcs -cvzf - $$B > $(PACKAGE).tgz; \
	rm -rf $$T

ifeq ($(TARGET),bionic)

.PHONY:	deploy

deploy:	$(PACKAGE).tgz
	adb $(ADBFLAGS) push $(PACKAGE).tgz /data/local/tmp

endif

.PHONY:	distribution

DISTRIBUTION := $(OUT)/$(TGZ_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)

distribution $(DISTRIBUTION).tgz:
	D=`dirname $(DISTRIBUTION)`; mkdir -p $$D; \
	T=`mktemp -d "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
	B=`basename $(DISTRIBUTION)`; \
	( cd $$T; svn export $(SVNURL) $$B; ); \
	tar -C $$T --exclude-vcs -cvzf - $$B > $(DISTRIBUTION).tgz; \
	rm -rf $$T/$$B; \
	tar -C $$T -xvzf - < $(DISTRIBUTION).tgz; \
	( cd $$T/$$B; make all TARGET=host OUT=out/host && $(OUT)/bin/generate > ./setup && . ./setup && vintage ); \
	rm -rf $$T

########## Source Code Control

.PHONY: commit dcommit push origin init

commit:
	git commit .

dcommit:
	git svn dcommit

push:
	git push origin master

pull:
	git pull origin master

init:
	git svn show-ignore > .gitignore
	git add .gitignore
	git remote add origin $(GITURL)

########## Target C Libraries

$(OUT)/$(ARC_DIR)/lib$(PROJECT).a:	$(TARGETOBJECTS)
	D=`dirname $@`; mkdir -p $$D
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(ARC_DIR)/lib$(PROJECT).a
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CFLAGS) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o $@ $(SOFLAGS) -Wl,--whole-archive $< -Wl,--no-whole-archive

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 
	
########## Target C++ Libraries

$(OUT)/$(ARC_DIR)/lib$(PROJECT)xx.a:	$(TARGETOBJECTSXX)
	D=`dirname $@`; mkdir -p $$D
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(ARC_DIR)/lib$(PROJECT)xx.a $(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) -o $@ $(SOXXFLAGS) -Wl,--whole-archive $< -Wl,--no-whole-archive $(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

########## Target Unstripped Binaries

$(OUT)/$(SYM_DIR)/%:	$(BIN_DIR)/%.c $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
########## Target Aliases

$(OUT)/$(BIN_DIR)/hex $(OUT)/$(BIN_DIR)/oct $(OUT)/$(BIN_DIR)/ntohs $(OUT)/$(BIN_DIR)/htons $(OUT)/$(BIN_DIR)/ntohl $(OUT)/$(BIN_DIR)/htonl:	$(OUT)/$(BIN_DIR)/dec
	ln -f $< $@
	
########## Unit Tests

# Generally if the unit tests work they exit with an exit code of zero;
# otherwise they exit with something else (or core dump). But left to their
# own devices, the unit tests aren't very chatty. If you want to see more log
# output from them, you can tune the logging using the environmental variable
# "DIMINUTO_LOG_MASK". For example, this bash command turns on everything.
#
#	export DIMINUTO_LOG_MASK=0xffff
#
# The Diminuto logging system sends log messages to the system log (syslog)
# when the emitter is a kernel-space module or the emitting process is a child
# of process 1 (that is, the emitting process is a daemon). Unfortunately
# different GNU/Linux distributions write syslog messages to different places.
# My current Ubuntu server uses "/var/log/syslog". Your mileage may vary.
#
# Some platforms may route syslog messages from processes to the kernel log
# buffer in memory by writing them to "/dev/kmesg". You can adjust the level of
# logging used by the kernel. Level 8 means "everything".
#
#	echo 8 > /proc/sys/kernel/printk
#
# The kernel log memory buffer can be displayed by using the "dmesg" utility.
#
# Even though the Android bionic library supports the openlog(3) and syslog(3)
# functions, syslog appears to be broken in the CyanogenMod 10.2 that I use.
# The library expects to send the log message as a datagram to a UNIX domain
# socket at "/dev/kmsg". But since that's a character device driver as described
# above, any attempt to connect(2) to it results in the connection being refused
# (you can use strace(1) to see this). This smells like a BSD (bionic) versus
# Linux (kernel) incompatibility.

$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.c $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.cpp $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Generated Source Files

# For embedding in a system where it can be executed from a shell.
# The major.minor.build is emitted to standard output, a bunch more
# metadata to standard error. Hence, they can be redirected to separate
# files. The metadata file is intended to parsable as a standard properties
# file (although I have not tested it).
$(OUT)/$(GEN_DIR)/vintage.c:	$(TARGETSYNTHESIZED) Makefile
	D=`dirname $@`; mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_platform.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_platform.h"' >> $@
	echo '#include <stdio.h>' >> $@
	echo 'static const char METADATA[] =' >> $@
	echo "    \"Title = $(TITLE)\\\\n\"" >> $@
	echo "    \"Copyright = $(COPYRIGHT)\\\\n\"" >> $@
	echo "    \"Contact = $(CONTACT)\\\\n\"" >> $@
	echo "    \"License = $(LICENSE)\\\\n\"" >> $@
	echo "    \"Homepage = $(HOMEPAGE)\\\\n\"" >> $@
	echo "    \"Release = $(MAJOR).$(MINOR).$(BUILD)\\\\n\"" >> $@
	echo "    \"Vintage = $(VINTAGE)\\\\n\"" >> $@
	echo "    \"Host = $(shell hostname)\\\\n\"" >> $@
	echo "    \"Directory = $(shell pwd)\\\\n\"" >> $@
	echo "    \"Arch = $(ARCH)\\\\n\"" >> $@
	echo "    \"Target = $(TARGET)\\\\n\"" >> $@
	echo "    \"Os = $(OS)\\\\n\"" >> $@
	echo "    \"Toolchain = $(TOOLCHAIN)\\\\n\"" >> $@
	echo ';' >> $@
	echo "extern const char COM_DIAG_DIMINUTO_PLATFORM_KEYWORD[];" >> $@
	echo "extern const char * COM_DIAG_DIMINUTO_PLATFORM_VALUE;" >> $@
	echo 'int main(void) {' >> $@
	echo '    fputs(METADATA, stderr);' >> $@
	echo '    fprintf(stderr, "RELEASE_KEYWORD = %s\\n", COM_DIAG_DIMINUTO_RELEASE_KEYWORD);' >> $@
	echo '    fprintf(stderr, "RELEASE_VALUE = %s\\n", COM_DIAG_DIMINUTO_RELEASE_VALUE);' >> $@
	echo '    fprintf(stderr, "VINTAGE_KEYWORD = %s\\n", COM_DIAG_DIMINUTO_VINTAGE_KEYWORD);' >> $@
	echo '    fprintf(stderr, "VINTAGE_VALUE = %s\\n", COM_DIAG_DIMINUTO_VINTAGE_VALUE);' >> $@
	echo '    fprintf(stderr, "PLATFORM_KEYWORD = %s\\n", COM_DIAG_DIMINUTO_PLATFORM_KEYWORD);' >> $@
	echo '    fprintf(stderr, "PLATFORM_VALUE = %s\\n", COM_DIAG_DIMINUTO_PLATFORM_VALUE);' >> $@
	echo '    fputs("$(MAJOR).$(MINOR).$(BUILD)\\n", stdout);' >> $@
	echo '    return 0;' >> $@
	echo '}' >> $@

# For embedding in an application where it can be interrogated or displayed.
$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_release.h:	Makefile
	D=`dirname $@`; mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_RELEASE_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_RELEASE_' >> $@
	echo "static const char COM_DIAG_DIMINUTO_RELEASE_KEYWORD[] = \"COM_DIAG_DIMINUTO_RELEASE=$(MAJOR).$(MINOR).$(BUILD)\";" >> $@
	echo "static const char * COM_DIAG_DIMINUTO_RELEASE_VALUE = &COM_DIAG_DIMINUTO_RELEASE_KEYWORD[sizeof(\"COM_DIAG_DIMINUTO_RELEASE=\") - 1];" >> $@
	echo '#endif' >> $@

# For embedding in the library archive and shared object.
$(OUT)/$(SRC_DIR)/diminuto_release.o:	$(TARGETSYNTHESIZED)

# For embedding in an application where it can be interrogated or displayed.
$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_vintage.h:	Makefile
	D=`dirname $@`; mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_VINTAGE_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_VINTAGE_' >> $@
	echo "static const char COM_DIAG_DIMINUTO_VINTAGE_KEYWORD[] = \"COM_DIAG_DIMINUTO_VINTAGE=$(VINTAGE)\";" >> $@
	echo "static const char * COM_DIAG_DIMINUTO_VINTAGE_VALUE = &COM_DIAG_DIMINUTO_VINTAGE_KEYWORD[sizeof(\"COM_DIAG_DIMINUTO_VINTAGE=\") - 1];" >> $@
	echo '#endif' >> $@

# For embedding in the library archive and shared object.
$(OUT)/$(SRC_DIR)/diminuto_vintage.o:	$(TARGETSYNTHESIZED)

# For executing from the command line during testing.
$(OUT)/$(SYM_DIR)/vintage:	$(OUT)/$(GEN_DIR)/vintage.c
	D=`dirname $@`; mkdir -p $$D	
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

# For sourcing into a bash shell (for example, ". setup").
$(OUT)/$(BIN_DIR)/setup:	Makefile
	D=`dirname $@`; mkdir -p $$D	
	echo 'COM_DIAG_$(SYMBOL)_PATH=`dirname $${BASH_ARGV[0]}`; COM_DIAG_$(SYMBOL)_ROOT=`cd $$COM_DIAG_$(SYMBOL)_PATH; pwd`' > $@
	echo 'export PATH=$$PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../bin:$$COM_DIAG_$(SYMBOL)_ROOT/../tst' >> $@
	echo 'export LD_DRIVER_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../drv' >> $@
	echo 'export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../lib' >> $@
	echo 'export LD_MODULE_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../mod' >> $@
	chmod 666 $@

# For generating a setup script for a bash shell (for example, "bash generate > setup").
# (Because the Android bash doesn't seem to implement BASH_ARGV.)
$(OUT)/$(BIN_DIR)/generate:	Makefile
	D=`dirname $@`; mkdir -p $$D	
	echo 'COM_DIAG_$(SYMBOL)_PATH=`dirname $$0`; COM_DIAG_$(SYMBOL)_ROOT=`cd $$COM_DIAG_$(SYMBOL)_PATH; pwd`' > $@
	echo 'echo export PATH=$$PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../bin:$$COM_DIAG_$(SYMBOL)_ROOT/../tst' >> $@
	echo 'echo export LD_DRIVER_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../drv' >> $@
	echo 'echo export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../lib' >> $@
	echo 'echo export LD_MODULE_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../mod' >> $@
	chmod 777 $@

########## User-Space Loadable Modules

LDWHOLEARCHIVES := # These archives will be linked into the shared object in their entirety.

$(OUT)/$(MOD_DIR)/%.so:	$(MOD_DIR)/%.c
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -o $@ $< $(MOFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

$(OUT)/$(MOD_DIR)/%.so:	$(MOD_DIR)/%.cpp
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -o $@ $< $(MOXXFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

########## Kernel-Space Loadable Modules

.PHONY:	drivers drivers-clean drivers-make

OBJ_M := $(addsuffix .o,$(basename $(shell cd $(DRV_DIR); ls *.c)))

$(OUT)/$(SYS_DIR)/Makefile:	Makefile
	D=`dirname $@`; mkdir -p $$D	
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo "obj-m := $(OBJ_M)" >> $@
	echo "EXTRA_CFLAGS := -iquote $(HERE)/$(INC_DIR) -iquote $(HERE)/$(TST_DIR)" >> $@

$(OUT)/$(SYS_DIR)/%.c:	$(DRV_DIR)/%.c
	D=`dirname $@`; mkdir -p $$D	
	cp $< $@

$(OUT)/$(DRV_DIR)/%.ko:	$(OUT)/$(SYS_DIR)/%.ko
	D=`dirname $@`; mkdir -p $$D	
	cp $< $@

# Was your kernel built with CONFIG_MODULES=y?
# Especially problematic in Android kernels.
$(OUT)/$(SYS_DIR)/diminuto_mmdriver.ko $(OUT)/$(SYS_DIR)/diminuto_utmodule.ko $(OUT)/$(SYS_DIR)/diminuto_kernel_datum.ko $(OUT)/$(SYS_DIR)/diminuto_kernel_map.ko:	$(OUT)/$(SYS_DIR)/Makefile $(OUT)/$(SYS_DIR)/diminuto_mmdriver.c $(OUT)/$(SYS_DIR)/diminuto_utmodule.c $(OUT)/$(SYS_DIR)/diminuto_kernel_datum.c $(OUT)/$(SYS_DIR)/diminuto_kernel_map.c
	make -C $(KERNEL_DIR) $(KERNELARCH) M=$(shell cd $(OUT)/$(SYS_DIR); pwd) CROSS_COMPILE=$(KERNELCHAIN) ARCH=$(ARCH) modules

drivers:	$(OUT)/$(DRV_DIR)/diminuto_mmdriver.ko $(OUT)/$(DRV_DIR)/diminuto_utmodule.ko $(OUT)/$(DRV_DIR)/diminuto_kernel_datum.ko $(OUT)/$(DRV_DIR)/diminuto_kernel_map.ko

drivers-clean:
	make -C $(KERNEL_DIR) M=$(shell cd $(OUT)/$(SYS_DIR); pwd) CROSS_COMPILE=$(KERNELCHAIN) ARCH=$(ARCH) clean
	rm -rf $(OUT)/$(SYS_DIR)

# e.g. DRIVERS=modules_prepare or DRIVERS=mrproper or DRIVERS=menuconfig
drivers-make:
	make -C $(KERNEL_DIR) $(KERNELARCH) CROSS_COMPILE=$(KERNELCHAIN) ARCH=$(ARCH) $(DRIVERS)

########## Helpers

.PHONY:	backup gccversion implicit defines iquotes isystems

backup:	../$(PROJECT).bak.tgz
	mv $(MVFLAGS) ../$(PROJECT).bak.tgz ../$(PROJECT).$(TIMESTAMP).tgz

../$(PROJECT).bak.tgz:
	tar cvzf - . > $@

gccversion:
	@$(CC) -x c $(CPPFLAGS) $(CFLAGS) -E -v - < /dev/null

implicit:
	@$(CC) $(CFLAGS) -dM -E - < /dev/null
	
defines:
	@$(CC) $(CPPFLAGS) $(CFLAGS) -dM -E - < /dev/null

iquotes:
	@$(CC) -x c $(CPPFLAGS) $(CFLAGS) -E -v - < /dev/null 2>&1 | awk 'BEGIN { S=0; } ($$0=="#include \"...\" search starts here:") { S=1; next; } ($$0=="#include <...> search starts here:") { S=0; next; } (S!=0) { print $$1; } { next; }'

isystems:
	@$(CC) -x c $(CPPFLAGS) $(CFLAGS) -E -v - < /dev/null 2>&1 | awk 'BEGIN { S=0; } ($$0=="#include <...> search starts here:") { S=1; next; } ($$0=="End of search list.") { S=0; next; } (S!=0) { print $$1; } { next; }'

########## Documentation

.PHONY:	documentation browse refman manpages

DOCCOOKED := $(shell echo $(OUT)/$(DOC_DIR) | sed 's/\//\\\//g')

documentation $(OUT)/$(DOC_DIR)/latex $(OUT)/$(DOC_DIR)/man $(OUT)/$(DOC_DIR)/pdf:
	mkdir -p $(OUT)/$(DOC_DIR)/pdf
	cat doxygen.cf | \
		sed -e "s/\\\$$PROJECT_NUMBER\\\$$/$(MAJOR).$(MINOR).$(BUILD)/" | \
		sed -e "s/\\\$$OUTPUT_DIRECTORY\\\$$/$(DOCCOOKED)/" | \
		cat > $(OUT)/$(DOC_DIR)/doxygen-local.cf
	doxygen $(OUT)/$(DOC_DIR)/doxygen-local.cf

# This appears to be broken in Doxygen 1.8.6. in Ubuntu 14.04. Used to work.
documentation-ancillary:	$(OUT)/$(DOC_DIR)/latex $(OUT)/$(DOC_DIR)/man $(OUT)/$(DOC_DIR)/pdf
	$(MAKE) -C $(OUT)/$(DOC_DIR)/latex refman.pdf
	cp $(OUT)/$(DOC_DIR)/latex/refman.pdf $(OUT)/$(DOC_DIR)/pdf
	cat $(OUT)/$(DOC_DIR)/man/man3/*.3 | groff -man -Tps - > $(OUT)/$(DOC_DIR)/pdf/manpages.ps
	ps2pdf $(OUT)/$(DOC_DIR)/pdf/manpages.ps $(OUT)/$(DOC_DIR)/pdf/manpages.pdf

browse:
	$(BROWSER) file:$(OUT)/$(DOC_DIR)/html/index.html

refman:
	$(BROWSER) file:$(OUT)/$(DOC_DIR)/pdf/refman.pdf

manpages:
	$(BROWSER) file:$(OUT)/$(DOC_DIR)/pdf/manpages.pdf

########## Diffs and Patches

.PHONY:	patch

patch:	$(OLD) $(NEW)
	diff -purN $(OLD) $(NEW)

########## Rules

$(OUT)/%.txt:	%.cpp
	D=`dirname $@`; mkdir -p $$D
	$(CXX) -E $(CPPFLAGS) -c $< > $@

$(OUT)/%.o:	%.cpp
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OUT)/%.txt:	%.c
	D=`dirname $@`; mkdir -p $$D
	$(CC) -E $(CPPFLAGS) -c $< > $@

$(OUT)/%.o:	%.c
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

$(OUT)/%:	%.sh
	D=`dirname $@`; mkdir -p $$D
	cp $< $@
	chmod 755 $@
	
.SECONDARY:

$(OUT)/$(BIN_DIR)/%:	$(OUT)/$(SYM_DIR)/%
	D=`dirname $@`; mkdir -p $$D
	$(STRIP) -o $@ $<

########## Dependencies

.PHONY:	depend

DEPENDENCIES := $(OUT)/$(DEP_DIR)/dependencies.mk

DEPENDENCIESC := $(SRC_DIR) $(TST_DIR) $(MOD_DIR) $(DRV_DIR)

DEPENDENCIESCXX := $(SRC_DIR) $(TST_DIR)

depend:	$(TARGETSYNTHESIZED)
	M=`dirname $(DEPENDENCIES)`; mkdir -p $$M
	cp /dev/null $(DEPENDENCIES)
	for S in $(DEPENDENCIESC); do \
		if [ -d $$S ]; then \
			for F in $$S/*.c; do \
				D=`dirname $$F`; \
				T=`mktemp "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
				echo -n "$(OUT)/$$D/" > $$T; \
				$(CC) $(CPPFLAGS) -MM -MG $$F >> $$T && cat $$T >> $(DEPENDENCIES); \
				rm -f $$T; \
			done; \
		fi; \
	done
	for S in $(DEPENDENCIESCXX); do \
		if [ -d $$S ]; then \
			for F in $$S/*.cpp; do \
				D=`dirname $$F`; \
				T=`mktemp "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
				echo -n "$(OUT)/$$D/" > $$T; \
				$(CXX) $(CPPFLAGS) -MM -MG $$F >> $$T && cat $$T >> $(DEPENDENCIES); \
				rm -f $$T; \
			done; \
		fi; \
	done

-include $(DEPENDENCIES)
