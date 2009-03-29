# Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Diminuto.html

########## Variables

PRODUCT		=	buildroot
PROJECT		=	diminuto

TGT_IPADDR	=	192.168.1.223
BDI_IPADDR	=	192.168.1.224
BDI_PORT	=	2001
TFTP_DIR	=	/var/lib/tftpboot
TMP_DIR		=	/var/tmp
ROOT_DIR	=	$(HOME)/$(PROJECT)
OPT_DIR		=	/opt
TOOLCHAIN_DIR	=	$(OPT_DIR)/$(PROJECT)/$(PRODUCT)
TOOLBIN_DIR	=	${TOOLCHAIN_DIR}/usr/bin
LOCALBIN_DIR	=	${TOOLCHAIN_DIR}/usr/local/bin
LOCALLIB_DIR	=	${TOOLCHAIN_DIR}/usr/local/lib

ARCH		=	arm
VENDOR		=	Atmel
TARGET		=	at91rm9200ek
PLATFORM	=	linux

CROSS_COMPILE	=	# $(ARCH)-$(PLATFORM)-
CARCH		=	# -march=armv4t

MAJOR		=	0
MINOR		=	5
BUILD		=	0

BUILDROOT_REV	=	22987
KERNEL_REV	=	2.6.25.10
BUSYBOX_REV	=	1.11.1

BUILDROOT_DIR	=	$(ROOT_DIR)/$(PRODUCT)
PROJECT_DIR	=	$(BUILDROOT_DIR)/project_build_$(ARCH)/$(PROJECT)
FAKEROOT_DIR	=	$(PROJECT_DIR)/root
KERNEL_DIR	=	$(PROJECT_DIR)/$(PLATFORM)-$(KERNEL_REV)
BUSYBOX_DIR	=	$(PROJECT_DIR)/busybox-$(BUSYBOX_REV)
CONFIG_DIR	=	$(BUILDROOT_DIR)/target/device/$(VENDOR)/$(TARGET)
BINARIES_DIR	=	$(BUILDROOT_DIR)/binaries
DIMINUTO_DIR	=	$(ROOT_DIR)/$(PROJECT)/trunk/Diminuto
DESPERADO_DIR	=	$(ROOT_DIR)/desperado/trunk/Desperado
FICL_DIR	=	$(ROOT_DIR)/ficl-4.0.31
UTILS_DIR	=	$(BUILDROOT_DIR)/toolchain_build_$(ARCH)/uClibc-0.9.29/utils
DOC_DIR		=	doc

TIMESTAMP	=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
DATESTAMP	=	$(shell date +%Y%m%d)
IMAGE		=	$(PROJECT)-linux-$(KERNEL_REV)
SVNURL		=	svn://192.168.1.220/diminuto/trunk/Diminuto

CFILES		=	$(wildcard *.c)
HFILES		=	$(wildcard *.h)

HOSTPROGRAMS	=	dbdi dcscope dgdb diminuto dlib
TARGETOBJECTS	=	$(addsuffix .o,$(basename $(wildcard diminuto_*.c)))
TARGETSCRIPTS	=	S10provision
TARGETBINARIES	=	getubenv
TARGETUNITTESTS	=	$(basename $(wildcard unittest-*.c))
TARGETARCHIVES	=	lib$(PROJECT).a
TARGETSHARED	=	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
TARGETLIBRARIES	=	$(TARGETARCHIVES) $(TARGETSHARED)
TARGETPROGRAMS	=	$(TARGETSCRIPTS) $(TARGETBINARIES) $(TARGETUNITTESTS)
ARTIFACTS	=	$(TARGETLIBRARIES) doxygen-local.cf

DIMINUTO_SO	=	lib$(PROJECT).so
DESPERADO_SO	=	libdesperado.so
FICL_SO		=	libficl.so

DIMINUTO_LIB	=	$(DIMINUTO_SO).[0-9]*.[0-9]*.[0-9]*
DESPERADO_LIB	=	$(DESPERADO_DIR)/$(DESPERADO_SO).[0-9]*.[0-9]*.[0-9]*
FICL_LIB	=	$(FICL_DIR)/$(FICL_SO).[0-9]*.[0-9]*.[0-9]*

SCRIPT		=	dummy

CC		=	$(CROSS_COMPILE)gcc
CXX		=	$(CROSS_COMPILE)g++
AR		=	$(CROSS_COMPILE)ar
RANLIB		=	$(CROSS_COMPILE)ranlib

ARFLAGS		=	rcv
CPPFLAGS	=
CXXFLAGS	=	$(CARCH) -g
CFLAGS		=	$(CARCH) -g
CPFLAGS		=	-i
MVFLAGS		=	-i
LDFLAGS		=	-L. -Bdynamic -ldiminuto

BROWSER		=	firefox

########## Main Entry Points

default:	$(TARGETLIBRARIES) $(TARGETUNITTESTS)

all:	$(HOSTPROGRAMS) $(TARGETLIBRARIES) $(TARGETPROGRAMS)

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

dist:	distribution

distribution:
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
	$(MAKE) script SCRIPT=dbdi

dcscope:	dcscope.sh
	$(MAKE) script SCRIPT=dcscope

dgdb:	dgdb.sh diminuto
	$(MAKE) script SCRIPT=dgdb

diminuto:	diminuto.sh
	$(MAKE) script SCRIPT=diminuto

diminuto.sh:	Makefile
	echo "# GENERATED FILE! DO NOT EDIT!" > diminuto.sh
	echo ARCH=\"$(ARCH)\" >> diminuto.sh
	echo BDIADDRESS=\"$(BDI_IPADDR)\" >> diminuto.sh
	echo BDIPORT=\"$(BDI_PORT)\" >> diminuto.sh
	echo BINARIES=\"$(BINARIES_DIR)\" >> diminuto.sh
	echo BUILDROOT=\"$(BUILDROOT_DIR)\" >> diminuto.sh
	echo CONFIG=\"$(CONFIG_DIR)\" >> diminuto.sh
	echo CROSS_COMPILE=\"$(CROSS_COMPILE)\" >> diminuto.sh
	echo DATESTAMP=\"$(DATESTAMP)\" >> diminuto.sh
	echo TOOLCHAIN=\"$(TOOLCHAIN_DIR)\" >> diminuto.sh
	echo DIMINUTO=\"$(DIMINUTO_DIR)\" >> diminuto.sh
	echo DESPERADO=\"$(DESPERADO_DIR)\" >> diminuto.sh
	echo FICL=\"$(FICL_DIR)\" >> diminuto.sh
	echo IMAGE=\"$(IMAGE)\" >> diminuto.sh
	echo KERNEL=\"$(KERNEL_DIR)\" >> diminuto.sh
	echo PLATFORM=\"$(PLATFORM)\" >> diminuto.sh
	echo PROJECT=\"$(PROJECT)\" >> diminuto.sh
	echo RELEASE=\"$(KERNEL_REV)\" >> diminuto.sh
	echo TARGET=\"$(TARGET)\" >> diminuto.sh
	echo TFTP=\"$(TFTP_DIR)\" >> diminuto.sh
	echo TGTADDRESS=\"$(TGT_IPADDR)\" >> diminuto.sh
	echo TMPDIR=\"$(TMP_DIR)\" >> diminuto.sh
	echo 'echo $${PATH} | grep -q "$(TOOLBIN_DIR)" || export PATH=$(TOOLBIN_DIR):$${PATH}' >> diminuto.sh
	echo 'echo $${PATH} | grep -q "$(LOCALBIN_DIR)" || export PATH=$(LOCALBIN_DIR):$${PATH}' >> diminuto.sh

dlib:	dlib.sh
	$(MAKE) script SCRIPT=dlib

########## Target Scripts

S10provision:	S10provision.sh getubenv
	$(MAKE) script SCRIPT=S10provision

########## Install Libraries

$(LOCALLIB_DIR)/lib$(PROJECT).so:	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)
	( cd $(LOCALLIB_DIR); rm -f lib$(PROJECT).so )
	( cd $(LOCALLIB_DIR); ln -s lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so )

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)
	( cd $(LOCALLIB_DIR); rm -f lib$(PROJECT).so.$(MAJOR) )
	( cd $(LOCALLIB_DIR); ln -s lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR) )

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	( cd $(LOCALLIB_DIR); rm -f lib$(PROJECT).so.$(MAJOR).$(MINOR) )
	( cd $(LOCALLIB_DIR); ln -s lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR).$(MINOR) )

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)
	cp $(CPFLAGS) lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)

########## Target Libraries

lib$(PROJECT).so:	lib$(PROJECT).so.$(MAJOR)
	rm -f lib$(PROJECT).so
	ln -s lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so

lib$(PROJECT).so.$(MAJOR):	lib$(PROJECT).so.$(MAJOR).$(MINOR)
	rm -f lib$(PROJECT).so.$(MAJOR)
	ln -s lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR)

lib$(PROJECT).so.$(MAJOR).$(MINOR):	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f lib$(PROJECT).so.$(MAJOR).$(MINOR)
	ln -s lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).so.$(MAJOR).$(MINOR)

lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(TARGETOBJECTS)
	$(CC) $(CARCH) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(TARGETOBJECTS)

lib$(PROJECT).a:	$(TARGETOBJECTS)
	$(AR) $(ARFLAGS) lib$(PROJECT).a $(TARGETOBJECTS)
	$(RANLIB) lib$(PROJECT).a

########## Target Binaries

getubenv:	getubenv.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-unittest:	unittest-unittest.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-time:	unittest-time.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-daemonize:	unittest-daemonize.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-hangup:	unittest-hangup.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-number:	unittest-number.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-timer:	unittest-timer.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-periodic:	unittest-periodic.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

########## Helpers

backup:	../$(PROJECT).bak.tgz
	mv $(MVFLAGS) ../$(PROJECT).bak.tgz ../$(PROJECT).$(TIMESTAMP).tgz

../$(PROJECT).bak.tgz:
	tar cvzf - . > ../diminuto.bak.tgz

acquire:	$(HOME)/$(PROJECT)
	cd $(HOME)/$(PROJECT)
	svn co svn://uclibc.org/trunk/buildroot

clean:
	rm -f $(HOSTPROGRAMS) $(TARGETPROGRAMS) $(ARTIFACTS) *.o
	rm -rf $(DOC_DIR)

binaries-clean:
	 rm -f $(BINARIES_DIR)/$(PROJECT)/$(PLATFORM)-kernel-$(KERNEL_REV)-$(ARCH) $(BINARIES_DIR)/$(PROJECT)/rootfs.*

########## Patches

$(PROJECT)-$(kERNEL_REV)-head.patch:
	$(MAKE) patch OLD=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV).orig/arch/arm/kernel/head.S NEW=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/arch/arm/kernel/head.S > $(PROJECT)-$(KERNEL_REV)-head.patch

$(PROJECT)-$(KERNEL_REV)-vmlinuxlds.patch:
	$(MAKE) patch OLD=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV).orig/arch/arm/kernel/vmlinux.lds.S NEW=project_build_$(ARCH)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/arch/arm/kernel/vmlinux.lds.S > $(PROJECT)-$(KERNEL_REV)-vmlinuxlds.patch

$(PROJECT)-$(PRODUCT)-devicetable.patch:
	$(MAKE) patch OLD=target/device/Atmel/root/device_table.txt.orig NEW=target/device/Atmel/root/device_table.txt > $(PROJECT)-$(PRODUCT)-devicetable.patch

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

$(HOME)/$(PROJECT):
	mkdir -p $(HOME)/$(PROJECT)

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

########## Dependencies

.PHONY:	depend

depend:	dependencies.mk

dependencies.mk:	Makefile $(CFILES) $(HFILES)
	$(CC) $(CPPFLAGS) -M -MG $(CFILES) > dependencies.mk

-include dependencies.mk

