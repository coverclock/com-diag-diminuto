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
TOOLCHAIN_DIR	=	/opt/$(PROJECT)/$(PRODUCT)
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
TIMESTAMP	=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
IMAGE		=	$(PROJECT)-linux-$(RELEASE)

HOSTPROGRAMS	=	dbdi dcscope dgdb diminuto
TARGETOBJECTS	=	diminuto_coreable.o diminuto_delay.o diminuto_map.o
TARGETSCRIPTS	=	S10provision
TARGETBINARIES	=	getubenv
TARGETLIBRARIES	=	lib$(PROJECT).a lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
TARGETPROGRAMS	=	$(TARGETSCRIPTS) $(TARGETBINARIES)

ARTIFACTS	=	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) lib$(PROJECT).a

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
LDFLAGS		=	-L$(LOCALLIB_DIR) -Bdynamic -ldiminuto

########## Main Entry Points

all:	$(HOSTPROGRAMS) $(TARGETLIBRARIES) $(TARGETPROGRAMS)

host-install:	$(HOSTPROGRAMS) $(LOCALBIN_DIR)
	cp $(CPFLAGS) $(HOSTPROGRAMS) $(LOCALBIN_DIR)

target-patch:	patches
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(RELEASE)-head.patch
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(RELEASE)-vmlinuxlds.patch
	( cd $(BUILDROOT_DIR); patch -p0 ) < $(PROJECT)-$(PRODUCT)-devicetable.patch

target-install:	$(TARGETLIBRARIES) $(TARGETPROGRAMS) $(FAKEROOT_DIR)/usr/local/bin $(FAKEROOT_DIR)/usr/local/lib
	cp $(CPFLAGS) S10provision $(FAKEROOT_DIR)/etc/init.d
	cp $(CPFLAGS) getubenv $(FAKEROOT_DIR)/usr/local/bin
	cp $(CPFLAGS) lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(FAKEROOT_DIR)/usr/local/lib

config-install:	$(CONFIG_DIR)
	cp $(CPFLAGS) $(KERNEL_DIR)/.config $(CONFIG_DIR)/$(TARGET)-$(PLATFORM)-$(RELEASE).config

tftp-install:
	cp $(CPFLAGS) $(BINARIES_DIR)/$(PROJECT)/$(PLATFORM)-kernel-$(RELEASE)-$(ARCH) $(TFTP_DIR)/$(IMAGE)

toolchain-dist:
	( cd $(TOOLCHAIN_DIR); tar cvjf - . ) > $(PROJECT)-toolchain.tar.bz2

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
	echo DIMINUTO=\"$(DIMINUTO_DIR)\" >> diminuto.sh
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

########## Target Scripts

S10provision:	S10provision.sh getubenv
	make script SCRIPT=S10provision

########## Target Libraries

libraries:	$(LOCALLIB_DIR)/lib$(PROJECT).so $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR) $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)

$(LOCALLIB_DIR)/lib$(PROJECT).so:	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f $(LOCALLIB_DIR)/lib$(PROJECT).so
	ln -s $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)/lib$(PROJECT).so

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)
	ln -s $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR)

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	rm -f $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)
	ln -s $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR)

$(LOCALLIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)
	cp $(CPFLAGS) lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(LOCALLIB_DIR)

lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(TARGETOBJECTS)
	$(CC) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) $(TARGETOBJECTS)

lib$(PROJECT).a:	$(TARGETOBJECTS)
	$(AR) $(ARFLAGS) lib$(PROJECT).a $(TARGETOBJECTS)
	$(RANLIB) lib$(PROJECT).a

########## Target Binaries

getubenv:	getubenv.c libraries
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
	rm -f $(HOSTPROGRAMS) $(TARGETOBJECTS) $(TARGETPROGRAMS) $(ARTIFACTS)

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
