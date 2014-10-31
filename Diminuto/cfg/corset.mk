# Copyright 2008-2014 Digital Aggregates Corporation
# Licensed under the terms in README.hz
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# corset

ARCH				:=	arm
OS					:=	linux
TOOLCHAIN			:=	$(ARCH)-$(OS)-androideabi-
KERNELCHAIN			:=	$(ARCH)-eabi-
KERNEL_REV			:=	3.4.0-cyanogenmod-g7182995
CYANOGENMODPRODUCT	:=	flo
CYANOGENMODKERNEL	:=	google
CYANOGENMODPROCESSOR:=	msm
CYANOGENMODDEVICE	:=	asus
CYANOGENMOD_DIR		:=	$(ROOT_DIR)/cm-10.2
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
LDARCH				+=	-L$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
LDARCH				+=	-L$(OUT)/$(LIB_DIR)
LDARCH				+=	-Wl,-rpath-link=$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
LDARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtbegin_dynamic.o
LDARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/STATIC_LIBRARIES/libcompiler_rt-extras_intermediates/libcompiler_rt-extras.a
LDARCH				+=	-L$(OUT)/$(LIB_DIR)
LDARCH				+=	-Wl,-z,noexecstack
LDARCH				+=	-Wl,-z,relro
LDARCH				+=	-Wl,-z,now
LDARCH				+=	-Wl,--warn-shared-textrel
LDARCH				+=	-Wl,--fatal-warnings
LDARCH				+=	-Wl,--icf=safe
LDARCH				+=	-Wl,--fix-cortex-a8
LDARCH				+=	-Wl,--no-undefined
LDARCH				+=	$(CYANOGENMOD_DIR)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/libgcc.a
LDARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtend_android.o
MOARCH				:=
MOARCH				+=	-nostdlib
MOARCH				+=	-Wl,--gc-sections
MOARCH				+=	-Wl,-shared,-Bsymbolic
MOARCH				+=	-L$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
MOARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtbegin_so.o
MOARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/STATIC_LIBRARIES/libcompiler_rt-extras_intermediates/libcompiler_rt-extras.a
MOARCH				+=	-L$(OUT)/$(LIB_DIR)
MOARCH				+=	-Wl,-z,noexecstack
MOARCH				+=	-Wl,-z,relro
MOARCH				+=	-Wl,-z,now
MOARCH				+=	-Wl,--warn-shared-textrel
MOARCH				+=	-Wl,--fatal-warnings
MOARCH				+=	-Wl,--icf=safe
MOARCH				+=	-Wl,--fix-cortex-a8
MOARCH				+=	-Wl,--no-undefined
MOARCH				+=	$(CYANOGENMOD_DIR)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/libgcc.a
MOARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtend_so.o
SOARCH				:=
SOARCH				+=	-nostdlib
SOARCH				+=	-Wl,--gc-sections
SOARCH				+=	-Wl,-shared,-Bsymbolic
SOARCH				+=	-L$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib
SOARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtbegin_so.o
SOARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/STATIC_LIBRARIES/libcompiler_rt-extras_intermediates/libcompiler_rt-extras.a
SOARCH				+=	-Wl,-z,noexecstack
SOARCH				+=	-Wl,-z,relro
SOARCH				+=	-Wl,-z,now
SOARCH				+=	-Wl,--warn-shared-textrel
SOARCH				+=	-Wl,--fatal-warnings
SOARCH				+=	-Wl,--icf=safe
SOARCH				+=	-Wl,--fix-cortex-a8
SOARCH				+=	-Wl,--no-undefined
SOARCH				+=	$(CYANOGENMOD_DIR)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/libgcc.a
SOARCH				+=	$(CYANOGENMOD_DIR)/out/target/product/$(CYANOGENMODPRODUCT)/obj/lib/crtend_so.o
KERNELARCH			:=	O=$(CYANOGENMOD_DIR)/out/target/product/flo/obj/KERNEL_OBJ
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

.PHONY:	deploy

ADBFLAGS			:=

deploy:	$(PACKAGE).tgz
	adb $(ADBFLAGS) push $(PACKAGE).tgz /data/local/tmp
