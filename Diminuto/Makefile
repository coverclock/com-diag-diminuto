# Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Diminuto.html

########## Customizations

COMPILEFOR			=	host
#COMPILEFOR			=	diminuto
#COMPILEFOR			=	arroyo
#COMPILEFOR			=	cascada
#COMPILEFOR			=	contraption

MAJOR				=	20# API changes requiring that applications be modified.
MINOR				=	0# Only functionality or features added with no API changes.
BUILD				=	0# Only bugs fixed with no API changes or new functionality.

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
VINTAGE				:=	$(shell date -u +%Y-%m-%dT%H:%M:%S.%N%z)# UTC in ISO8601 format: yyyy-mm-ddThh:mm:ss.nnnnnnnnn-zzzz

# This stuff all gets embedded in the vintage application.
TITLE				=	Diminuto
COPYRIGHT			=	2013 Digital Aggregates Corporation
LICENSE				=	GNU Lesser General Public License 2.1
CONTACT				=	coverclock@diag.com
HOMEPAGE			=	http://www.diag.com/navigation/downloads/Diminuto.html

# You can change the VINFO make variable into whatever tool you use to extract
# version information from your source code control system. For example, I
# use Subversion here, and hence VINFO is the "svn info" command. But elsewhere,
# I use Git, so maybe I'd use "git describe" in those circumstances. Or maybe
# I'd have my own shell script. Or I'd just set this to echo some constant
# string. If you don't have a source code control system, don't sweat it. If
# the VINFO command fails or does not exist, all that happens is no such version
# information is included in the Diminuto vintage application.
VINFO				=	svn info
#VINFO				=	git describe

# This is where I store collateral associated with projects that I have
# downloaded off the web and use without alteration. Examples: Linux kernel
# sources, toolchains, etc.
HOME_DIR			=	$(HOME)/projects

########## Configurations

PROJECT				=	diminuto
PRODUCT				=	buildroot

ifeq ($(COMPILEFOR),diminuto)
# Build for the AT91RM9200-EK board with the BuildRoot kernel.
ARCH				=	arm
PLATFORM			=	linux
TARGET				=	diminuto
CPPARCH				=
CARCH				=	-march=armv4t
LDARCH				=	-Bdynamic
CROSS_COMPILE		=	$(ARCH)-$(PLATFORM)-
KERNEL_REV			=	2.6.25.10
KERNEL_DIR			=	$(HOME_DIR)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/$(PROJECT)/$(PRODUCT)/project_build_arm/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),arroyo)
# Build for the AT91RM9200-EK board with the Arroyo kernel.
ARCH				=	arm
PLATFORM			=	linux
TARGET				=	arroyo
CPPARCH				=
CARCH				=	-march=armv4t
LDARCH				=	-Bdynamic
CROSS_COMPILE		=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV			=	2.6.26.3
KERNEL_DIR			=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),cascada)
# Build for the BeagleBoard C4 with the Angstrom kernel.
ARCH				=	arm
PLATFORM			=	linux
TARGET				=	cascada
CPPARCH				=
CARCH				=	-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC
LDARCH				=	-Bdynamic
CROSS_COMPILE		=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV			=	2.6.32.7
KERNEL_DIR			=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),contraption)
# Build for the BeagleBoard C4 with the FroYo Android 2.2 kernel.
ARCH				=	arm
PLATFORM			=	linux
TARGET				=	contraption
CPPARCH				=
CARCH				=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp -fPIC
#LDARCH				=	-static
LDARCH				=	-Bdynamic
CROSS_COMPILE		=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV			=	2.6.32
KERNEL_DIR			=	$(HOME_DIR)/contraption/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
INCLUDE_DIR			=	$(HOME_DIR)/contraption/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),cobbler)
# Build for the Raspberry Pi version B.
# Toolchain: https://github.com/raspberrypi/tools
# Kernel: https://github.com/raspberrypi/linux
# Bootloader: https://github.com/raspberrypi/firmware
ARCH				=	arm
PLATFORM			=	linux
TARGET				=	cobbler
CPPARCH				=
CARCH				=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp -fPIC
#LDARCH				=	-static
LDARCH				=	-Bdynamic
CROSS_COMPILE		=	$(ARCH)-none-$(PLATFORM)-gnueabi-
KERNEL_REV			=	2.6.32
KERNEL_DIR			=	$(HOME_DIR)/cobbler/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
INCLUDE_DIR			=	$(HOME_DIR)/cobbler/include-$(KERNEL_REV)/include
endif

ifeq ($(COMPILEFOR),host)
# Build for my Dell build server with the Ubuntu kernel.
ARCH				=	i386
PLATFORM			=	linux
TARGET				=	i686
CPPARCH				=
CARCH				=
LDARCH				=	
CROSS_COMPILE		=
KERNEL_REV			=	3.2.0-51
KERNEL_DIR			=	/usr/src/linux-headers-$(KERNEL_REV)-generic-pae
INCLUDE_DIR			=	/usr/include
endif

########## Directory Tree

BIN_DIR				=	bin
DOC_DIR				=	doc
DRV_DIR				=	drv
INC_DIR				=	inc
LIB_DIR				=	lib
MOD_DIR				=	mod
OUT_DIR				=	out
SRC_DIR				=	src
TST_DIR				=	tst

########## Variables

HERE				:=	$(shell pwd)

OUT					=	$(OUT_DIR)/$(TARGET)

TMP_DIR				=	/var/tmp
ROOT_DIR			=	$(HOME_DIR)/$(PROJECT)

TIMESTAMP			=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
DATESTAMP			=	$(shell date +%Y%m%d)
IMAGE				=	$(PROJECT)-linux-$(KERNEL_REV)
SVNURL				=	svn://silver/diminuto/trunk/Diminuto

PROJECT_A			=	lib$(PROJECT).a
PROJECTXX_A			=	lib$(PROJECT)xx.a
PROJECT_SO			=	lib$(PROJECT).so
PROJECTXX_SO		=	lib$(PROJECT)xx.so

PROJECT_LIB			=	$(PROJECT_SO).$(MAJOR).$(MINOR).$(BUILD)
PROJECTXX_LIB		=	$(PROJECTXX_SO).$(MAJOR).$(MINOR).$(BUILD)

HOSTPROGRAMS		=	dbdi dcscope dgdb diminuto dlib
TARGETSOFTLINKS		=	hex oct ntohs htons ntohl htonl

TARGETOBJECTS		=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.c))))
TARGETOBJECTSXX		=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.cpp))))
TARGETDRIVERS		=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(DRV_DIR)/*.c))))
TARGETMODULES		=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(MOD_DIR)/*.c))))
TARGETSCRIPTS		=	
TARGETBINARIES		=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.c)))
TARGETALIASES		=	$(addprefix $(OUT)/$(BIN_DIR)/,$(TARGETSOFTLINKS))
TARGETUNSTRIPPED	=	$(addsuffix _unstripped,$(TARGETBINARIES))
TARGETUNITTESTS		=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.c)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.cpp)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.sh)))

TARGETARCHIVE		=	$(OUT)/$(LIB_DIR)/$(PROJECT_A)
TARGETARCHIVEXX		=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_A)
TARGETSHARED		=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR).$(BUILD)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO)
TARGETSHAREDXX		=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR).$(BUILD)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO)

TARGETLIBRARIES		=	$(TARGETARCHIVE) $(TARGETSHARED)
TARGETLIBRARIESXX	=	$(TARGETARCHIVEXX) $(TARGETSHAREDXX)
TARGETPROGRAMS		=	$(TARGETSCRIPTS) $(TARGETUNSTRIPPED) $(TARGETBINARIES) $(TARGETALIASES) $(TARGETUNITTESTS)
TARGETDEFAULT		=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) $(TARGETPROGRAMS)
TARGETPACKAGE		=	$(TARGETDEFAULT) $(TARGETDRIVERS) $(TARGETMODULES)
ARTIFACTS			=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) doxygen-local.cf

SCRIPT				=	dummy

CC					=	$(CROSS_COMPILE)gcc
CXX					=	$(CROSS_COMPILE)g++
AR					=	$(CROSS_COMPILE)ar
RANLIB				=	$(CROSS_COMPILE)ranlib
STRIP				=	$(CROSS_COMPILE)strip

CDEFINES			=	-DCOM_DIAG_DIMINUTO_VINTAGE=\"$(VINTAGE)\"
ARFLAGS				=	rcv
CPPFLAGS			=	$(CPPARCH) -iquote $(INC_DIR) -isystem $(INCLUDE_DIR) $(CDEFINES)
CXXFLAGS			=	$(CARCH) -g
CFLAGS				=	$(CARCH) -g
#CXXFLAGS			=	$(CARCH) -O3
#CFLAGS				=	$(CARCH) -O3
CPFLAGS				=	-i
MVFLAGS				=	-i
LDFLAGS				=	$(LDARCH) -L$(OUT)/lib -ldiminuto -lpthread -lrt -ldl
LDXXFLAGS			=	$(LDARCH) -L$(OUT)/lib -ldiminutoxx -ldiminuto -lpthread -lrt -ldl

BROWSER				=	firefox

########## Main Entry Points

.PHONY:	default all dist

default:	$(TARGETDEFAULT)

all:	$(HOSTPROGRAMS) $(TARGETPACKAGE)

dist:	distribution

clean:
	rm -f $(HOSTPROGRAMS) $(TARGETPROGRAMS) $(TARGETUNITTESTS) $(ARTIFACTS) unittest-version.c unittest-version
	rm -rf $(DOC_DIR)

pristine:	clean
	rm -f $(TARGETLIBRARIES)

########## Distribution

.PHONY:	distribution

distribution:
	rm -rf $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	svn export $(SVNURL) $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	( cd $(TMP_DIR); tar cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) ) > $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
	( cd $(TMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD); make COMPILEFOR=host; ./out/i686/bin/vintage )

########## Host Scripts

dbdi:	dbdi.sh diminuto
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dbdi

dcscope:	dcscope.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dcscope

dgdb:	dgdb.sh diminuto
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dgdb

dlib:	dlib.sh
	$(MAKE) COMPILEFOR=$(COMPILEFOR) script SCRIPT=dlib

########## Target C Libraries

$(OUT)/$(LIB_DIR)/lib$(PROJECT).a:	$(TARGETOBJECTS)
	test -d $(OUT)/$(LIB_DIR) || mkdir -p $(OUT)/$(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	ln -s -f $< $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	ln -s -f $< $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	ln -s -f $< $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).a
	$(CC) $(CARCH) -shared -Wl,-soname,$@ -o $@ -Wl,--whole-archive $< -Wl,--no-whole-archive
	
########## Target C++ Libraries

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.a:	$(TARGETOBJECTSXX)
	test -d $(OUT)/$(LIB_DIR) || mkdir -p $(OUT)/$(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	ln -s -f $< $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	ln -s -f $< $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	ln -s -f $< $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.a
	$(CC) $(CARCH) -shared -Wl,-soname,$@ -o $@ -Wl,--whole-archive $< -Wl,--no-whole-archive

########## Target Binaries

$(OUT)/$(BIN_DIR)/%_unstripped:	$(BIN_DIR)/%.c $(TARGETLIBRARIES)
	test -d $(OUT)/$(BIN_DIR) || mkdir -p $(OUT)/$(BIN_DIR)
	$(CC) $(CPPFLAGS) -I $(KERNEL_DIR)/include $(CFLAGS) -o $@ $< $(LDFLAGS)

$(OUT)/$(BIN_DIR)/hex $(OUT)/$(BIN_DIR)/oct $(OUT)/$(BIN_DIR)/ntohs $(OUT)/$(BIN_DIR)/htons $(OUT)/$(BIN_DIR)/ntohl $(OUT)/$(BIN_DIR)/htonl:	$(OUT)/$(BIN_DIR)/dec
	ln -f $< $@
	
########## Unit Tests

$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.c $(TARGETLIBRARIES)
	test -d $(OUT)/$(TST_DIR) || mkdir -p $(OUT)/$(TST_DIR)
	$(CC) -rdynamic $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.cpp $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	test -d $(OUT)/$(TST_DIR) || mkdir -p $(OUT)/$(TST_DIR)
	$(CXX) -rdynamic $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Generated

.PHONY:	vintage.c diminuto_release.h diminuto_vintage.h

vintage_unstripped:	vintage.c $(TARGETLIBRARIES)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

# For embedding in a system where it can be executed from a shell.
vintage.c:	diminuto_release.h diminuto_vintage.h
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "diminuto_release.h"' >> $@
	echo '#include "diminuto_release.h"' >> $@
	echo '#include "diminuto_vintage.h"' >> $@
	echo '#include "diminuto_vintage.h"' >> $@
	echo '#include <stdio.h>' >> $@
	echo 'static const char VERSION[] =' >> $@
	echo '"VERSION_BEGIN\n"' >> $@
	echo "\"Title: $(TITLE)\\n\"" >> $@
	echo "\"Copyright: $(COPYRIGHT)\\n\"" >> $@
	echo "\"Contact: $(CONTACT)\\n\"" >> $@
	echo "\"License: $(LICENSE)\\n\"" >> $@
	echo "\"Homepage: $(HOMEPAGE)\\n\"" >> $@
	echo "\"Release: $(MAJOR).$(MINOR).$(BUILD)\\n\"" >> $@
	echo "\"Vintage: $(VINTAGE)\\n\"" >> $@
	echo "\"Host: $(shell hostname)\\n\"" >> $@
	echo "\"Directory: $(shell pwd)\\n\"" >> $@
	$(VINFO) | sed 's/"/\\"/g' | awk '/^$$/ { next; } { print "\""$$0"\\n\""; }' >> $@ || true
	echo '"VERSION_END\n";' >> $@
	echo 'int main(void) { fputs(VERSION, stdout); fputs("$(MAJOR).$(MINOR).$(BUILD)\n", stderr); return 0; }' >> $@

# For embedding in an application where it can be interrogated or displayed.
diminuto_release.h:
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_DIMINUTO_RELEASE_' >> $@
	echo '#define _H_COM_DIAG_DIMINUTO_RELEASE_' >> $@
	echo "static const char RELEASE[] = \"RELEASE=$(MAJOR).$(MINOR).$(BUILD)\";" >> $@
	echo '#endif' >> $@

# For embedding in an application where it can be interrogated or displayed.
diminuto_vintage.h:
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_DIMINUTO_VINTAGE_' >> $@
	echo '#define _H_COM_DIAG_DIMINUTO_VINTAGE_' >> $@
	echo "static const char VINTAGE[] = \"VINTAGE=$(VINTAGE)\";" >> $@
	echo '#endif' >> $@

########## Modules

LDWHOLEARCHIVES=# These archives will be linked into the shared object in their entirety.

loadables/unittest-module-example.so:	loadables/unittest-module-example.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -shared $< $(LDFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

########## Drivers

.PHONY:	drivers drivers-clean

modules/Makefile:	Makefile
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo "obj-m := diminuto_utmodule.o diminuto_mmdriver.o diminuto_kernel_datum.o diminuto_kernel_map.o" >> $@
	echo "EXTRA_CFLAGS := -I$(HERE) -I$(HERE)/include" >> $@
	#echo "EXTRA_CFLAGS := -I$(HERE) -I$(HERE)/include -DDEBUG" >> $@

$(TARGETDRIVERS):	modules/Makefile modules/diminuto_mmdriver.c modules/diminuto_utmodule.c modules/diminuto_kernel_datum.c modules/diminuto_kernel_map.c
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

########## Documentation

documentation:
	sed -e "s/\\\$$Name.*\\\$$/$(MAJOR).$(MINOR).$(BUILD)/" < doxygen.cf > doxygen-local.cf
	doxygen doxygen-local.cf
	test -d $(DOC_DIR)/pdf || mkdir -p $(DOC_DIR)/pdf
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

patch:	$(OLD) $(NEW)
	echo "diff -purN $(OLD) $(NEW)"
	diff -purN $(OLD) $(NEW)

########## Rules

$(OUT)/%.txt:	%.cpp
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CXX) -E $(CPPFLAGS) -c $< > $@

$(OUT)/%.o:	%.cpp
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OUT)/%.txt:	%.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) -E $(CPPFLAGS) -c $< > $@

$(OUT)/%.o:	%.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%:	%_unstripped
	$(STRIP) -o $@ $<

########## Dependencies

.PHONY:	depend
	
depend:
	cp /dev/null dependencies.mk
	for S in $(SRC_DIR) $(MOD_DIR) $(DRV_DIR) $(TST_DIR); do \
		for F in $$S/*.c; do \
			D=`dirname $$F`; \
			echo -n "$(OUT)/$$D/" >> dependencies.mk; \
			$(CC) $(CPPFLAGS) -MM -MG $$F >> dependencies.mk; \
		done; \
	done
	for S in $(SRC_DIR) $(TST_DIR); do \
		for F in $$S/*.cpp; do \
			D=`dirname $$F`; \
			echo -n "$(OUT)/$$D/" >> dependencies.mk; \
			$(CXX) $(CPPFLAGS) -MM -MG $$F >> dependencies.mk; \
		done; \
	done

-include dependencies.mk
