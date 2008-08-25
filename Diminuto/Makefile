# Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA
# Licensed under the GNU GPL V2
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Diminuto.html

########## Variables

PRODUCT		=	buildroot
PROJECT		=	diminuto
TGT_IPADDR	=	192.168.1.223
BDI_IPADDR	=	192.168.1.224
TFTP_DIR	=	/var/lib/tftpboot
TMP_DIR		=	/var/tmp
ROOT_DIR	=	$(HOME)/$(PROJECT)
OPT_DIR		=	/opt
TOOLCHAIN_DIR	=	$(OPT_DIR)/$(PROJECT)/$(PRODUCT)
TOOLBIN_DIR	=	${TOOLCHAIN_DIR}/usr/bin
LOCALBIN_DIR	=	${TOOLCHAIN_DIR}/usr/local/bin
LOCALLIB_DIR	=	${TOOLCHAIN_DIR}/usr/local/lib

MAJOR		=	0
MINOR		=	0
BUILD		=	0

ARCH		=	arm
VENDOR		=	Atmel
TARGET		=	at91rm9200ek
PLATFORM	=	linux
RELEASE		=	2.6.25.10
BDI_PORT	=	2001
BUILDROOT_DIR	=	$(ROOT_DIR)/$(PRODUCT)
PROJECT_DIR	=	$(BUILDROOT_DIR)/project_build_$(ARCH)/$(PROJECT)
FAKEROOT_DIR	=	$(PROJECT_DIR)/root
KERNEL_DIR	=	$(PROJECT_DIR)//$(PLATFORM)-$(RELEASE)
CONFIG_DIR	=	$(BUILDROOT_DIR)/target/device/$(VENDOR)/$(TARGET)
BINARIES_DIR	=	$(BUILDROOT_DIR)/binaries
DIMINUTO_DIR	=	$(ROOT_DIR)/$(PROJECT)/trunk/Diminuto
DESPERADO_DIR	=	$(ROOT_DIR)/desperado/trunk/Desperado
FICL_DIR	=	$(ROOT_DIR)/ficl-4.0.31
UTILS_DIR	=	$(BUILDROOT_DIR)/toolchain_build_$(ARCH)/uClibc-0.9.29/utils
TIMESTAMP	=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
IMAGE		=	$(PROJECT)-linux-$(RELEASE)
SVNURL		=	svn://192.168.1.220/diminuto/trunk/Diminuto

HOSTPROGRAMS	=	dbdi dcscope dgdb diminuto dlgdb dlib
TARGETOBJECTS	=	diminuto_coreable.o diminuto_daemonize.o diminuto_delay.o diminuto_lock.o diminuto_log.o diminuto_map.o diminuto_time.o
TARGETSCRIPTS	=	S10provision
TARGETBINARIES	=	getubenv
TARGETUNITTESTS	=	unittest-time unittest-daemonize
TARGETARCHIVES	=	lib$(PROJECT).a
TARGETSHARED	=	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
TARGETLIBRARIES	=	$(TARGETARCHIVES) $(TARGETSHARED)
TARGETPROGRAMS	=	$(TARGETSCRIPTS) $(TARGETBINARIES) $(TARGETUNITTESTS)
ARTIFACTS	=	$(TARGETLIBRARIES)

DESPERADO_LIB	=	$(DESPERADO_DIR)/libdesperado.so.3.0.0
FICL_LIB	=	$(FICL_DIR)/libficl.so.4.0.31

SCRIPT		=	dummy

CROSS_COMPILE	=	$(ARCH)-$(PLATFORM)-
CC		=	$(CROSS_COMPILE)gcc
CXX		=	$(CROSS_COMPILE)g++
AR		=	$(CROSS_COMPILE)ar
RANLIB		=	$(CROSS_COMPILE)ranlib

ARFLAGS		=	rcv
CPPFLAGS	=
CXXFLAGS	=	-g
CFLAGS		=	-g
CPFLAGS		=	-i
MVFLAGS		=	-i
LDFLAGS		=	-L. -Bdynamic -ldiminuto

########## Main Entry Points

all:	$(HOSTPROGRAMS) $(TARGETLIBRARIES) $(TARGETPROGRAMS)

host-install:	$(HOSTPROGRAMS) $(LOCALBIN_DIR) $(LOCALLIB_DIR)/lib$(PROJECT).so
	cp $(CPFLAGS) $(HOSTPROGRAMS) $(LOCALBIN_DIR)

target-patch:	patches
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(RELEASE)-head.patch
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(RELEASE)-vmlinuxlds.patch
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(PRODUCT)-devicetable.patch

target-install:	$(TARGETSHARED) $(TARGETPROGRAMS) $(FAKEROOT_DIR)/usr/local/bin $(FAKEROOT_DIR)/usr/local/lib
	cp $(CPFLAGS) S10provision $(FAKEROOT_DIR)/etc/init.d
	cp $(CPFLAGS) getubenv $(FAKEROOT_DIR)/usr/local/bin
	cp $(CPFLAGS) $(UTILS_DIR)/ldconfig $(FAKEROOT_DIR)/sbin
	cp $(CPFLAGS) $(UTILS_DIR)/ldd $(FAKEROOT_DIR)/usr/bin
	echo "/usr/local/lib" > $(FAKEROOT_DIR)/etc/ld.so.conf
	cp $(CPFLAGS) lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(FAKEROOT_DIR)/usr/local/lib
	test -f $(DESPERADO_LIB) && cp $(CPFLAGS) $(DESPERADO_LIB) $(FAKEROOT_DIR)/usr/local/lib
	test -f $(FICL_LIB) && cp $(CPFLAGS) $(FICL_LIB) $(FAKEROOT_DIR)/usr/local/lib

config-install:	$(CONFIG_DIR)
	cp $(CPFLAGS) $(KERNEL_DIR)/.config $(CONFIG_DIR)/$(TARGET)-$(PLATFORM)-$(RELEASE).config

tftp-install:
	cp $(CPFLAGS) $(BINARIES_DIR)/$(PROJECT)/$(PLATFORM)-kernel-$(RELEASE)-$(ARCH) $(TFTP_DIR)/$(IMAGE)

toolchain-dist:
	( cd $(OPT_DIR); tar cvjf - $(PROJECT)/$(PRODUCT) ) > $(PROJECT)-toolchain.tar.bz2

dist:
	rm -rf $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	svn export $(SVNURL) $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	( cd $(TMP_DIR); tar cvjf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) > $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tar.bz2 )

build:
	( cd $(BUILDROOT_DIR); make 2>&1 | tee LOG )

########## Host Scripts

dbdi:	dbdi.sh diminuto
	make script SCRIPT=dbdi

dcscope:	dcscope.sh
	make script SCRIPT=dcscope

dgdb:	dgdb.sh diminuto
	make script SCRIPT=dgdb

diminuto:	diminuto.sh
	make script SCRIPT=diminuto

diminuto.sh:	Makefile
	echo "# GENERATED FILE! DO NOT EDIT!" > diminuto.sh
	echo ARCH=\"$(ARCH)\" >> diminuto.sh
	echo BDIADDRESS=\"$(BDI_IPADDR)\" >> diminuto.sh
	echo BDIPORT=\"$(BDI_PORT)\" >> diminuto.sh
	echo BINARIES=\"$(BINARIES_DIR)\" >> diminuto.sh
	echo BUILDROOT=\"$(BUILDROOT_DIR)\" >> diminuto.sh
	echo CONFIG=\"$(CONFIG_DIR)\" >> diminuto.sh
	echo CROSS_COMPILE=\"$(CROSS_COMPILE)\" >> diminuto.sh
	echo DATESTAMP=\"\`date +%Y%m%d\`\" >> diminuto.sh
	echo TOOLCHAIN=\"$(TOOLCHAIN_DIR)\" >> diminuto.sh
	echo DIMINUTO=\"$(DIMINUTO_DIR)\" >> diminuto.sh
	echo DESPERADO=\"$(DESPERADO_DIR)\" >> diminuto.sh
	echo FICL=\"$(FICL_DIR)\" >> diminuto.sh
	echo IMAGE=\"$(IMAGE)\" >> diminuto.sh
	echo KERNEL=\"$(KERNEL_DIR)\" >> diminuto.sh
	echo PLATFORM=\"$(PLATFORM)\" >> diminuto.sh
	echo PROJECT=\"$(PROJECT)\" >> diminuto.sh
	echo RELEASE=\"$(RELEASE)\" >> diminuto.sh
	echo TARGET=\"$(TARGET)\" >> diminuto.sh
	echo TFTP=\"$(TFTP_DIR)\" >> diminuto.sh
	echo TGTADDRESS=\"$(TGT_IPADDR)\" >> diminuto.sh
	echo TMPDIR=\"$(TMP_DIR)\" >> diminuto.sh
	echo 'echo $${PATH} | grep -q "$(TOOLBIN_DIR)" || export PATH=$(TOOLBIN_DIR):$${PATH}' >> diminuto.sh
	echo 'echo $${PATH} | grep -q "$(LOCALBIN_DIR)" || export PATH=$(LOCALBIN_DIR):$${PATH}' >> diminuto.sh

dlgdb:	dlgdb.sh diminuto
	make script SCRIPT=dlgdb

dlib:	dlib.sh
	make script SCRIPT=dlib

########## Target Scripts

S10provision:	S10provision.sh getubenv
	make script SCRIPT=S10provision

########## Install Libraries

$(LOCALLIB_DIR)/lib$(PROJECT).so:	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)
	rm -f $(LOCALLIB_DIR)/lib$(PROJECT).so
	ln -s $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)/lib$(PROJECT).so

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)
	rm -f $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)
	ln -s $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)
	ln -s $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)

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
	$(CC) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(TARGETOBJECTS)

lib$(PROJECT).a:	$(TARGETOBJECTS)
	$(AR) $(ARFLAGS) lib$(PROJECT).a $(TARGETOBJECTS)
	$(RANLIB) lib$(PROJECT).a

########## Target Binaries

getubenv:	getubenv.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-time:	unittest-time.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

unittest-daemonize:	unittest-daemonize.c lib$(PROJECT).so
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $<

########## Helpers

backup:	../$(PROJECT).bak.tar.bz2
	mv $(MVFLAGS) ../$(PROJECT).bak.tar.bz2 ../$(PROJECT).$(TIMESTAMP).tar.bz2

../$(PROJECT).bak.tar.bz2:
	tar cvjf - . > ../diminuto.bak.tar.bz2

acquire:	$(HOME)/$(PROJECT)
	cd $(HOME)/$(PROJECT)
	svn co svn://uclibc.org/trunk/buildroot

clean:
	rm -f $(HOSTPROGRAMS) $(TARGETPROGRAMS) $(ARTIFACTS) *.o

binaries-clean:
	 rm -f $(BINARIES_DIR)/$(PROJECT)/$(PLATFORM)-kernel-$(RELEASE)-$(ARCH) $(BINARIES_DIR)/$(PROJECT)/rootfs.*

########## Patches

$(PROJECT)-$(RELEASE)-head.patch:
	make patch OLD=project_build_${ARCH}/${PROJECT}/${PLATFORM}-${RELEASE}.orig/arch/arm/kernel/head.S NEW=project_build_${ARCH}/${PROJECT}/${PLATFORM}-${RELEASE}/arch/arm/kernel/head.S > $(PROJECT)-$(RELEASE)-head.patch

$(PROJECT)-$(RELEASE)-vmlinuxlds.patch:
	make patch OLD=project_build_${ARCH}/${PROJECT}/${PLATFORM}-${RELEASE}.orig/arch/arm/kernel/vmlinux.lds.S NEW=project_build_${ARCH}/${PROJECT}/${PLATFORM}-${RELEASE}/arch/arm/kernel/vmlinux.lds.S > $(PROJECT)-$(RELEASE)-vmlinuxlds.patch

$(PROJECT)-$(PRODUCT)-devicetable.patch:
	make patch OLD=target/device/Atmel/root/device_table.txt.orig NEW=target/device/Atmel/root/device_table.txt > $(PROJECT)-$(PRODUCT)-devicetable.patch

patches:	$(PROJECT)-$(RELEASE)-head.patch $(PROJECT)-$(RELEASE)-vmlinuxlds.patch $(PROJECT)-$(PRODUCT)-devicetable.patch

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

########## Rules

%.o:	%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.o:	%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<
