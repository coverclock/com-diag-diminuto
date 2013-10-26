# Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Diminuto.html
# When I first started Diminuto, it began as both an exercise in using the open
# source buildroot tool and as an associated Linux systems programming library
# and toolkit written in C. But with later projects like Arroyo, Cascada and
# Contraption, the library and toolkit built on top of it became much more
# useful than the artifacts that came out of buildroot. I've left in the
# buildroot rules, but have not tested them in ages and would not expect them
# to work. The library and toolkit I use all the time. So the default behavior
# of this makefile is to just build the library and toolkit.

########## Customizations

COMPILEFOR	=	host
#COMPILEFOR	=	diminuto
#COMPILEFOR	=	arroyo
#COMPILEFOR	=	cascada
#COMPILEFOR	=	contraption

MAJOR		=	17# API changes requiring that applications be modified.
MINOR		=	0# Functionality or features added but no API changes.
BUILD		=	0# Bugs fixed but no API changes or new functionality.

# Some certification, defense, or intelligence agencies (e.g. the U.S. Federal
# Aviation Administration or FAA) require that software builds for safety
# critical or national security applications generate exactly the same binary
# images bit for bit if the source code has not changed. (This is theoretically
# a more stringent requirement than requiring that checksums or cryptographic
# hashes are the same, although in practice it is the same thing.) This allows
# agency inspectors to verify the integrity of the binary software images. This
# makes embedding timestamps inside compiled translation units problematic.
# If your application has this requirement, you can pass in any fixed string
# for the value of the VINTAGE make variable and you should be able to generate
# identical images with subsequent builds of Diminuto. This string is embedded
# inside the Diminuto vintage application.
VINTAGE		=	$(shell date -u +%Y-%m-%dT%H:%M:%S.%N%z)# UTC in ISO8601 format: yyyy-mm-ddThh:mm:ss.nnnnnnnnn-zzzz

# You can change the VINFO make variable into whatever tool you use to extract
# version information from your source code control system. For example, I
# use Subversion here, and hence VINFO is the "svn info" command. But elsewhere,
# I use Git, so maybe I'd use "git describe" in those circumstances. Or maybe
# I'd have my own shell script. Or I'd just set this to echo some constant
# string. If you don't have a source code control system, don't sweat it. If
# the VINFO command fails or does not exist, all that happens is no such version
# information is included in the Diminuto vintage application.
VINFO		=	svn info
#VINFO		=	git describe

HOME_DIR	=	$(HOME)/projects

########## Configurations

PROJECT		=	diminuto
PRODUCT		=	buildroot

ifeq ($(COMPILEFOR),diminuto)
# Build for the AT91RM9200-EK board with the BuildRoot kernel.
ARCH		=	arm
PLATFORM	=	linux
CPPARCH		=
CARCH		=	-march=armv4t
LDARCH		=	-Bdynamic
CROSS_COMPILE	=	$(ARCH)-$(PLATFORM)-
KERNEL_REV	=	2.6.25.10
KERNEL_DIR	=	$(HOME_DIR)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR	=	$(HOME_DIR)/$(PROJECT)/$(PRODUCT)/project_build_arm/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),arroyo)
# Build for the AT91RM9200-EK board with the Arroyo kernel.
ARCH		=	arm
PLATFORM	=	linux
CPPARCH		=
CARCH		=	-march=armv4t
LDARCH		=	-Bdynamic
CROSS_COMPILE	=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV	=	2.6.26.3
KERNEL_DIR	=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR	=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),cascada)
# Build for the BeagleBoard C4 with the Angstrom kernel.
ARCH		=	arm
PLATFORM	=	linux
CPPARCH		=
CARCH		=	-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC
LDARCH		=	-Bdynamic
CROSS_COMPILE	=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV	=	2.6.32.7
KERNEL_DIR	=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR	=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),contraption)
# Build for the BeagleBoard C4 with the FroYo Android 2.2 kernel.
ARCH		=	arm
PLATFORM	=	linux
CPPARCH		=
CARCH		=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp -fPIC
#LDARCH		=	-static
LDARCH		=	-Bdynamic
CROSS_COMPILE	=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV	=	2.6.32
KERNEL_DIR	=	$(HOME_DIR)/contraption/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
INCLUDE_DIR	=	$(HOME_DIR)/contraption/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),host)
# Build for my build server with the Ubuntu kernel.
ARCH		=	i386
PLATFORM	=	linux
CPPARCH		=
CARCH		=
LDARCH		=	
CROSS_COMPILE	=
KERNEL_REV	=	3.2.0-51
KERNEL_DIR	=	/usr/src/linux-headers-$(KERNEL_REV)-generic-pae
#INCLUDE_DIR	=	$(KERNEL_DIR)/include
INCLUDE_DIR	=	/usr/include
endif

########## Variables

HERE		:=	$(shell pwd)

TGT_IPADDR	=	192.168.1.223
BDI_IPADDR	=	192.168.1.224
BDI_PORT	=	2001
TFTP_DIR	=	/var/lib/tftpboot
TMP_DIR		=	/var/tmp
ROOT_DIR	=	$(HOME_DIR)/$(PROJECT)
OPT_DIR		=	/opt
TOOLCHAIN_DIR	=	$(OPT_DIR)/$(PROJECT)/$(PRODUCT)
TOOLBIN_DIR	=	${TOOLCHAIN_DIR}/usr/bin
LOCALBIN_DIR	=	${TOOLCHAIN_DIR}/usr/local/bin
LOCALLIB_DIR	=	${TOOLCHAIN_DIR}/usr/local/lib

VENDOR		=	Atmel
TARGET		=	at91rm9200ek

BUILDROOT_REV	=	22987
BUSYBOX_REV		=	1.11.1

BUILDROOT_DIR	=	$(ROOT_DIR)/$(PRODUCT)
PROJECT_DIR		=	$(BUILDROOT_DIR)/project_build_$(ARCH)/$(PROJECT)
FAKEROOT_DIR	=	$(PROJECT_DIR)/root
BUSYBOX_DIR		=	$(PROJECT_DIR)/busybox-$(BUSYBOX_REV)
CONFIG_DIR		=	$(BUILDROOT_DIR)/target/device/$(VENDOR)/$(TARGET)
BINARIES_DIR	=	$(BUILDROOT_DIR)/binaries
DIMINUTO_DIR	=	$(ROOT_DIR)/$(PROJECT)/trunk/Diminuto
DESPERADO_DIR	=	$(ROOT_DIR)/desperado/trunk/Desperado
FICL_DIR		=	$(ROOT_DIR)/ficl-4.0.31
UTILS_DIR		=	$(BUILDROOT_DIR)/toolchain_build_$(ARCH)/uClibc-0.9.29/utils
DOC_DIR			=	doc

TIMESTAMP	=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
DATESTAMP	=	$(shell date +%Y%m%d)
IMAGE		=	$(PROJECT)-linux-$(KERNEL_REV)
SVNURL		=	svn://192.168.1.220/diminuto/trunk/Diminuto

CFILES		=	$(wildcard *.c)
CXXFILES	=	$(wildcard *.cpp)
HFILES		=	$(wildcard *.h)
MFILES		=	$(wildcard modules/*.c)

DIMINUTO_SO		=	lib$(PROJECT).so
DIMINUTOXX_SO	=	lib$(PROJECT)xx.so
DESPERADO_SO	=	libdesperado.so
FICL_SO			=	libficl.so

DIMINUTO_LIB	=	$(DIMINUTO_SO).$(MAJOR).$(MINOR).$(BUILD)
DIMINUTOXX_LIB	=	$(DIMINUTOXX_SO).$(MAJOR).$(MINOR).$(BUILD)
DESPERADO_LIB	=	$(DESPERADO_DIR)/$(DESPERADO_SO).[0-9]*.[0-9]*.[0-9]*
FICL_LIB		=	$(FICL_DIR)/$(FICL_SO).[0-9]*.[0-9]*.[0-9]*

HOSTPROGRAMS		=	dbdi dcscope dgdb diminuto dlib
TARGETOBJECTS		=	$(addsuffix .o,$(basename $(wildcard $(PROJECT)_*.c)))
TARGETOBJECTSXX		=	$(addsuffix .o,$(basename $(wildcard [A-Z]*.cpp)))
TARGETMODULES		=	modules/diminuto_mmdriver.ko modules/diminuto_utmodule.ko modules/diminuto_kernel_datum.ko modules/diminuto_kernel_map.ko
TARGETSCRIPTS		=	S10provision
TARGETBINARIES		=	getubenv ipcalc coreable memtool mmdrivertool phex dump dec usectime usecsleep vintage
TARGETALIASES		=	hex oct ntohs htons ntohl htonl
TARGETUNSTRIPPED	=	$(addsuffix _unstripped,$(TARGETBINARIES))
TARGETUNITTESTS		=	$(basename $(wildcard unittest-*.c)) $(basename $(wildcard unittest-*.cpp)) $(basename $(wildcard unittest-*.sh))

TARGETARCHIVE		=	lib$(PROJECT).a
TARGETARCHIVEXX		=	lib$(PROJECT)xx.a
TARGETSHARED		=	$(DIMINUTO_SO).$(MAJOR).$(MINOR).$(BUILD) $(DIMINUTO_SO).$(MAJOR).$(MINOR) $(DIMINUTO_SO).$(MAJOR) $(DIMINUTO_SO)
TARGETSHAREDXX		=	$(DIMINUTOXX_SO).$(MAJOR).$(MINOR).$(BUILD) $(DIMINUTOXX_SO).$(MAJOR).$(MINOR) $(DIMINUTOXX_SO).$(MAJOR) $(DIMINUTOXX_SO)
TARGETLIBRARIES		=	$(TARGETARCHIVE) $(TARGETSHARED)
TARGETLIBRARIESXX	=	$(TARGETARCHIVEXX) $(TARGETSHAREDXX)
TARGETPROGRAMS		=	$(TARGETSCRIPTS) $(TARGETUNSTRIPPED) $(TARGETBINARIES) $(TARGETALIASES) $(TARGETUNITTESTS)
TARGETDEFAULT		=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) $(TARGETPROGRAMS)
TARGETPACKAGE		=	$(TARGETDEFAULT) $(TARGETMODULES)
ARTIFACTS			=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) doxygen-local.cf

SCRIPT		=	dummy

CC			=	$(CROSS_COMPILE)gcc
CXX			=	$(CROSS_COMPILE)g++
AR			=	$(CROSS_COMPILE)ar
RANLIB		=	$(CROSS_COMPILE)ranlib
STRIP		=	$(CROSS_COMPILE)strip

CDEFINES	=	

ARFLAGS		=	rcv
CPPFLAGS	=	$(CPPARCH) -iquoteinclude -isystem $(INCLUDE_DIR) $(CDEFINES)
CXXFLAGS	=	$(CARCH) -g
CFLAGS		=	$(CARCH) -g
#CXXFLAGS	=	$(CARCH) -O3
#CFLAGS		=	$(CARCH) -O3
CPFLAGS		=	-i
MVFLAGS		=	-i
LDFLAGS		=	$(LDARCH) -L. -ldiminuto -lpthread -lrt
LDXXFLAGS	=	$(LDARCH) -L. -ldiminutoxx -ldiminuto -lpthread -lrt

BROWSER		=	firefox

########## Main Entry Points

.PHONY:	default all install host-install target-path target-install config-backup tftp-install package distribution distributionfull build

default:	$(TARGETDEFAULT)

all:	$(HOSTPROGRAMS) $(TARGETPACKAGE)

install:	host-install target-install

host-install:	$(HOSTPROGRAMS) $(LOCALBIN_DIR) $(LOCALLIB_DIR)/lib$(PROJECT).so
	cp $(CPFLAGS) $(HOSTPROGRAMS) $(LOCALBIN_DIR)

target-patch:	patches
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(KERNEL_REV)-head.patch
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(KERNNEL_REV)-vmlinuxlds.patch
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(PRODUCT)-devicetable.patch

target-install:	$(TARGETSHARED) $(TARGETPROGRAMS) $(FAKEROOT_DIR)/usr/local/bin $(FAKEROOT_DIR)/usr/local/lib
	cp $(CPFLAGS) S10provision $(FAKEROOT_DIR)/etc/init.d
	cp $(CPFLAGS) getubenv $(FAKEROOT_DIR)/usr/local/bin
	cp $(CPFLAGS) $(UTILS_DIR)/ldconfig $(FAKEROOT_DIR)/sbin
	cp $(CPFLAGS) $(UTILS_DIR)/ldd $(FAKEROOT_DIR)/usr/bin
	echo "/usr/local/lib" > $(FAKEROOT_DIR)/etc/ld.so.conf
	rm -f $(FAKEROOT_DIR)/usr/local/lib/$(DIMINUTO_SO)*
	cp $(CPFLAGS) $(DIMINUTO_LIB) $(FAKEROOT_DIR)/usr/local/lib
	rm -f $(FAKEROOT_DIR)/usr/local/lib/$(DESPERADO_SO)* && cp $(CPFLAGS) $(DESPERADO_LIB) $(FAKEROOT_DIR)/usr/local/lib
	rm -f $(FAKEROOT_DIR)/usr/local/lib/$(FICL_SO)* && cp $(CPFLAGS) $(FICL_LIB) $(FAKEROOT_DIR)/usr/local/lib

config-backup:
	cp $(CPFLAGS) $(BUILDROOT_DIR)/.config diminuto-buildroot-$(BUILDROOT_REV).config
	cp $(CPFLAGS) $(KERNEL_DIR)/.config diminuto-linux-$(KERNEL_REV).config
	cp $(CPFLAGS) $(BUSYBOX_DIR)/.config diminuto-busybox-$(BUSYBOX_REV).config

tftp-install:
	cp $(CPFLAGS) $(BINARIES_DIR)/$(PROJECT)/$(PLATFORM)-kernel-$(KERNEL_REV)-$(ARCH) $(TFTP_DIR)/$(IMAGE)

package:	$(TARGETPACKAGE)
	tar cvzf - $(TARGETPACKAGE) > $(COMPILEFOR)-$(MAJOR).$(MINOR).$(BUILD).tgz

dist:	distribution

distribution:
	rm -rf $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	svn export $(SVNURL) $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	( cd $(TMP_DIR); tar cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) ) > $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz

distributionfull:
	rm -rf $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	svn export $(SVNURL) $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	( cd $(TMP_DIR); tar cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) ) > $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
	( cd $(OPT_DIR); tar cvzf - $(PROJECT)/$(PRODUCT) ) > $(TMP_DIR)/$(PROJECT)-toolchain.tgz

build:
	cp $(CPFLAGS) $(KERNEL_DIR)/.config diminuto-linux-$(KERNEL_REV).bak
	cp /dev/null $(TMP_DIR)/config-initramfs-source.ex
	echo "g/^CONFIG_INITRAMFS_SOURCE=/s/\.arm-[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]\./.arm-$(DATESTAMP)./" >> $(TMP_DIR)/config-initramfs-source.ex
	echo "wq" >> $(TMP_DIR)/config-initramfs-source.ex
	cat $(TMP_DIR)/config-initramfs-source.ex
	ex -S $(TMP_DIR)/config-initramfs-source.ex $(KERNEL_DIR)/.config
	rm -f $(TMP_DIR)/config-initramfs-source.ex
	cp $(CPFLAGS) $(KERNEL_DIR)/.config $(CONFIG_DIR)/$(TARGET)-$(PLATFORM)-$(KERNEL_REV).config
	( cd $(BUILDROOT_DIR); $(MAKE) 2>&1 | tee LOG )

########## Host Scripts

dbdi:	dbdi.sh diminuto
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dbdi

dcscope:	dcscope.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dcscope

dgdb:	dgdb.sh diminuto
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dgdb

dlib:	dlib.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dlib

diminuto:	diminuto.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=diminuto

diminuto.sh:	Makefile
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo ARCH=\"$(ARCH)\" >> $@
	echo BDIADDRESS=\"$(BDI_IPADDR)\" >> $@
	echo BDIPORT=\"$(BDI_PORT)\" >> $@
	echo BINARIES=\"$(BINARIES_DIR)\" >> $@
	echo BUILDROOT=\"$(BUILDROOT_DIR)\" >> $@
	echo CONFIG=\"$(CONFIG_DIR)\" >> $@
	echo CROSS_COMPILE=\"$(CROSS_COMPILE)\" >> $@
	echo DATESTAMP=\"$(DATESTAMP)\" >> $@
	echo TOOLCHAIN=\"$(TOOLCHAIN_DIR)\" >> $@
	echo DIMINUTO=\"$(DIMINUTO_DIR)\" >> $@
	echo DESPERADO=\"$(DESPERADO_DIR)\" >> $@
	echo FICL=\"$(FICL_DIR)\" >> $@
	echo IMAGE=\"$(IMAGE)\" >> $@
	echo KERNEL=\"$(KERNEL_DIR)\" >> $@
	echo PLATFORM=\"$(PLATFORM)\" >> $@
	echo PROJECT=\"$(PROJECT)\" >> $@
	echo RELEASE=\"$(KERNEL_REV)\" >> $@
	echo TARGET=\"$(TARGET)\" >> $@
	echo TFTP=\"$(TFTP_DIR)\" >> $@
	echo TGTADDRESS=\"$(TGT_IPADDR)\" >> $@
	echo TMPDIR=\"$(TMP_DIR)\" >> $@
	echo 'echo $${PATH} | grep -q "$(TOOLBIN_DIR)" || export PATH=$(TOOLBIN_DIR):$${PATH}' >> $@
	echo 'echo $${PATH} | grep -q "$(LOCALBIN_DIR)" || export PATH=$(LOCALBIN_DIR):$${PATH}' >> $@

########## Target Scripts

S10provision:	S10provision.sh getubenv
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=S10provision

########## Install Libraries

$(LOCALLIB_DIR)/lib$(PROJECT).so:	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)
	( cd $(LOCALLIB_DIR); rm -f lib$(PROJECT).so )
	( cd $(LOCALLIB_DIR); ln -s -f lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so )

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)
	( cd $(LOCALLIB_DIR); rm -f lib$(PROJECT).so.$(MAJOR) )
	( cd $(LOCALLIB_DIR); ln -s -f lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR) )

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	( cd $(LOCALLIB_DIR); rm -f lib$(PROJECT).so.$(MAJOR).$(MINOR) )
	( cd $(LOCALLIB_DIR); ln -s -f lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR).$(MINOR) )

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)
	cp $(CPFLAGS) lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)

########## Target C Libraries

lib$(PROJECT).so:	lib$(PROJECT).so.$(MAJOR)
	rm -f lib$(PROJECT).so
	ln -s -f lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so

lib$(PROJECT).so.$(MAJOR):	lib$(PROJECT).so.$(MAJOR).$(MINOR)
	rm -f lib$(PROJECT).so.$(MAJOR)
	ln -s -f lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR)

lib$(PROJECT).so.$(MAJOR).$(MINOR):	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f lib$(PROJECT).so.$(MAJOR).$(MINOR)
	ln -s -f lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR).$(MINOR)

lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(TARGETOBJECTS)
	$(CC) $(CARCH) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(TARGETOBJECTS)

lib$(PROJECT).a:	$(TARGETOBJECTS)
	$(AR) $(ARFLAGS) lib$(PROJECT).a $(TARGETOBJECTS)
	$(RANLIB) lib$(PROJECT).a
	
########## Target C++ Libraries

lib$(PROJECT)xx.so:	lib$(PROJECT)xx.so.$(MAJOR)
	rm -f lib$(PROJECT)xx.so
	ln -s -f lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT)xx.so

lib$(PROJECT)xx.so.$(MAJOR):	lib$(PROJECT)xx.so.$(MAJOR).$(MINOR)
	rm -f lib$(PROJECT)xx.so.$(MAJOR)
	ln -s -f lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT)xx.so.$(MAJOR)

lib$(PROJECT)xx.so.$(MAJOR).$(MINOR):	lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f lib$(PROJECT)xx.so.$(MAJOR).$(MINOR)
	ln -s -f lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT)xx.so.$(MAJOR).$(MINOR)

lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD):	$(TARGETOBJECTSXX)
	$(CXX) $(CARCH) -shared -Wl,-soname,lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) -o lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) $(TARGETOBJECTSXX)

lib$(PROJECT)xx.a:	$(TARGETOBJECTSXX)
	$(AR) $(ARFLAGS) lib$(PROJECT)xx.a $(TARGETOBJECTSXX)
	$(RANLIB) lib$(PROJECT)xx.a

########## Target Binaries

getubenv_unstripped:	getubenv.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) -I $(KERNEL_DIR)/include $(CFLAGS) -o $@ $< $(LDFLAGS)

ipcalc_unstripped:	ipcalc.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

memtool_unstripped:	memtool.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

mmdrivertool_unstripped:	mmdrivertool.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

dec_unstripped:	dec.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

usectime_unstripped:	usectime.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

usecsleep_unstripped:	usecsleep.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

hex:	dec
	ln -f dec hex

oct:	dec
	ln -f dec oct

ntohs:	dec
	ln -f dec ntohs

htons:	dec
	ln -f dec htons

ntohl:	dec
	ln -f dec ntohl

htonl:	dec
	ln -f dec htonl

phex_unstripped:	phex.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

dump_unstripped:	dump.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

coreable_unstripped:	coreable.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-unittest:	unittest-unittest.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-time:	unittest-time.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-daemon:	unittest-daemon.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-hangup:	unittest-hangup.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-number:	unittest-number.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-timer:	unittest-timer.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-periodic:	unittest-periodic.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-stacktrace:	unittest-stacktrace.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -O0 -rdynamic -o $@ $< $(LDFLAGS)

unittest-log:	unittest-log.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-lock:	unittest-lock.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-map:	unittest-map.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-mmdriver-at91rm9200ek:	unittest-mmdriver-at91rm9200ek.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=unittest-mmdriver-at91rm9200ek

unittest-phex:	unittest-phex.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-dump:	unittest-dump.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-endianess:	unittest-endianess.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-barrier:	unittest-barrier.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-serial:	unittest-serial.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-memtool-at91rm9200ek:	unittest-memtool-at91rm9200ek.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=unittest-memtool-at91rm9200ek

unittest-memtool2-at91rm9200ek:	unittest-memtool2-at91rm9200ek.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=unittest-memtool2-at91rm9200ek

unittest-coreable:	unittest-coreable.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-countof:	unittest-countof.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-list:	unittest-list.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-ipc:	unittest-ipc.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-string:	unittest-string.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-heap:	unittest-heap.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-pool:	unittest-pool.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-datum:	unittest-datum.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-discrete:	unittest-discrete.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-alignment:	unittest-alignment.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

unittest-version:	unittest-version.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
unittest-well:	unittest-well.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
unittest-well-cpp:	unittest-well-cpp.cpp $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)
	
unittest-well-perf:	unittest-well-perf.cpp $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)
	
unittest-escape:	unittest-escape.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
unittest-epoch:	unittest-epoch.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
unittest-frequency:	unittest-frequency.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

########## Generated

vintage_unstripped:	vintage.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY:	vintage.c diminuto_release.h diminuto_vintage.h

# For embedding in a system where it can be executed from a shell.
vintage.c:	diminuto_release.h diminuto_vintage.h
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "diminuto_release.h"' >> $@
	echo '#include "diminuto_release.h"' >> $@
	echo '#include "diminuto_vintage.h"' >> $@
	echo '#include "diminuto_vintage.h"' >> $@
	echo '#include <stdio.h>' >> $@
	echo 'static const char VINTAGE[] =' >> $@
	echo '"DIMINUTO_VERSION_BEGIN\n"' >> $@
	echo "\"Release: $(MAJOR).$(MINOR).$(BUILD)\\n\"" >> $@
	echo "\"Vintage: $(VINTAGE)\\n\"" >> $@
	echo "\"Host: $(shell hostname)\\n\"" >> $@
	echo "\"Domain: $(shell dnsdomainname)\\n\"" >> $@
	echo "\"Directory: $(shell pwd)\\n\"" >> $@
	echo "\"User: $(shell logname)\\n\"" >> $@
	$(VINFO) | sed 's/"/\\"/g' | awk '/^$$/ { next; } { print "\""$$0"\\n\""; }' >> $@ || true
	echo '"DIMINUTO_VERSION_END\n";' >> $@
	echo 'int main(void) { fputs(VINTAGE, stdout); }' >> $@

# For embedding in an application where it can be interrogated or displayed.
diminuto_release.h:
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_DIMINUTO_RELEASE_' >> $@
	echo '#define _H_COM_DIAG_DIMINUTO_RELEASE_' >> $@
	echo "static const char DIMINUTO_RELEASE[] = \"DIMINUTO_RELEASE=$(MAJOR).$(MINOR).$(BUILD)\";" >> $@
	echo '#endif' >> $@

# For embedding in an application where it can be interrogated or displayed.
diminuto_vintage.h:
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_DIMINUTO_VINTAGE_' >> $@
	echo '#define _H_COM_DIAG_DIMINUTO_VINTAGE_' >> $@
	echo "static const char DIMINUTO_VINTAGE[] = \"DIMINUTO_VINTAGE=$(VINTAGE)\";" >> $@
	echo '#endif' >> $@

########## Drivers

.PHONY:	drivers drivers-clean

modules/Makefile:	Makefile
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo "obj-m := diminuto_utmodule.o diminuto_mmdriver.o diminuto_kernel_datum.o diminuto_kernel_map.o" >> $@
	echo "EXTRA_CFLAGS := -I$(HERE) -I$(HERE)/include" >> $@
	#echo "EXTRA_CFLAGS := -I$(HERE) -I$(HERE)/include -DDEBUG" >> $@

${TARGETMODULES}:	modules/Makefile modules/diminuto_mmdriver.c modules/diminuto_utmodule.c modules/diminuto_kernel_datum.c modules/diminuto_kernel_map.c
	make -C $(KERNEL_DIR) M=$(shell cd modules; pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

drivers:	modules/Makefile modules/diminuto_mmdriver.c modules/diminuto_utmodule.c modules/diminuto_kernel_datum.c modules/diminuto_kernel_map.c
	make -C $(KERNEL_DIR) M=$(shell cd modules; pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

drivers-clean:
	make -C $(KERNEL_DIR) M=$(shell cd modules; pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) clean
	rm -f modules/Makefile

########## Helpers

backup:	../$(PROJECT).bak.tgz
	mv $(MVFLAGS) ../$(PROJECT).bak.tgz ../$(PROJECT).$(TIMESTAMP).tgz

../$(PROJECT).bak.tgz:
	tar cvzf - . > ../diminuto.bak.tgz

acquire:	$(HOME_DIR)/$(PROJECT)
	cd $(HOME_DIR)/$(PROJECT)
	svn co svn://uclibc.org/trunk/buildroot

clean:
	rm -f $(HOSTPROGRAMS) $(TARGETPROGRAMS) $(ARTIFACTS) *.o
	rm -rf $(DOC_DIR)

binaries-clean:
	 rm -f $(BINARIES_DIR)/$(PROJECT)/$(PLATFORM)-kernel-$(KERNEL_REV)-$(ARCH) $(BINARIES_DIR)/$(PROJECT)/rootfs.*

libraries-clean:
	rm -f $(TARGETLIBRARIES)

########## Patches

$(PROJECT)-$(kERNEL_REV)-head.patch:
	$(MAKE) COMPILEFOR=$(COMPILEFOR) patch OLD=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV).orig/arch/arm/kernel/head.S NEW=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/arch/arm/kernel/head.S > $(PROJECT)-$(KERNEL_REV)-head.patch

$(PROJECT)-$(KERNEL_REV)-vmlinuxlds.patch:
	$(MAKE) COMPILEFOR=$(COMPILEFOR) patch OLD=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV).orig/arch/arm/kernel/vmlinux.lds.S NEW=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/arch/arm/kernel/vmlinux.lds.S > $(PROJECT)-$(KERNEL_REV)-vmlinuxlds.patch

$(PROJECT)-$(PRODUCT)-devicetable.patch:
	$(MAKE) COMPILEFOR=$(COMPILEFOR) patch OLD=target/device/Atmel/root/device_table.txt.orig NEW=target/device/Atmel/root/device_table.txt > $(PROJECT)-$(PRODUCT)-devicetable.patch

patches:	$(PROJECT)-$(kERNEL_REV)-head.patch $(PROJECT)-$(KERNEL_REV)-vmlinuxlds.patch $(PROJECT)-$(PRODUCT)-devicetable.patch

########## Documentation

documentation:	$(DOC_DIR)/pdf
	sed -e "s/\\\$$Name.*\\\$$/$(MAJOR).$(MINOR).$(BUILD)/" < doxygen.cf > doxygen-local.cf
	doxygen doxygen-local.cf
	( cd $(DOC_DIR)/latex; $(MAKE) refman.pdf; cp refman.pdf ../pdf )
	cat $(DOC_DIR)/man/man3/*.3 | groff -man -Tps - > $(DOC_DIR)/pdf/manpages.ps
	ps2pdf $(DOC_DIR)/pdf/manpages.ps $(DOC_DIR)/pdf/manpages.pdf

browse:
	$(BROWSER) file:doc/html/index.html

refman:
	$(BROWSER) file:doc/pdf/refman.pdf

manpages:
	$(BROWSER) file:doc/pdf/manpages.pdf

########## Submakes

.PHONY:	script patch

script:	$(SCRIPT).sh
	cp $(SCRIPT).sh $(SCRIPT)
	chmod 755 $(SCRIPT)

patch:
	cd $(BUILDROOT_DIR)
	echo "diff -purN $(OLD) $(NEW)"
	diff -purN $(OLD) $(NEW)

########## Directories

$(CONFIG_DIR):
	mkdir -p $(CONFIG_DIR)

$(LOCALBIN_DIR):
	mkdir -p $(LOCALBIN_DIR)

$(LOCALLIB_DIR):
	mkdir -p $(LOCALLIB_DIR)

$(FAKEROOT_DIR)/usr/local/bin:
	mkdir -p $(FAKEROOT_DIR)/usr/local/bin

$(FAKEROOT_DIR)/usr/local/lib:
	mkdir -p $(FAKEROOT_DIR)/usr/local/lib

$(HOME_DIR)/$(PROJECT):
	mkdir -p $(HOME_DIR)/$(PROJECT)

$(DOC_DIR)/pdf:
	mkdir -p $(DOC_DIR)/pdf

########## Rules

%.txt:	%.cpp
	$(CXX) -E $(CPPFLAGS) -c $< > $*.txt

%.o:	%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.txt:	%.c
	$(CC) -E $(CPPFLAGS) -c $< > $*.txt

%.o:	%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%:	%_unstripped
	$(STRIP) -o $@ $<

########## Dependencies

.PHONY:	depend

depend:
	$(CC) $(CPPFLAGS) -M -MG $(CFILES) $(MFILES) > dependencies.mk

-include dependencies.mk

