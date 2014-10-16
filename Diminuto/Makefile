# Copyright 2008-2014 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html

PROJECT				:=	diminuto
TITLE				:=	Diminuto
SYMBOL				:=	DIMINUTO

########## Customizations

#TARGET				:=	host
#TARGET				:=	diminuto
#TARGET				:=	arroyo
#TARGET				:=	cascada
#TARGET				:=	contraption
#TARGET				:=	cobbler
#TARGET				:=	uclibc
#TARGET				:=	stampede
TARGET				:=	bionic

MAJOR				:=	25# API changes that may require that applications be modified.
MINOR				:=	2# Only functionality or features added with no legacy API changes.
BUILD				:=	1# Only bugs fixed with no API changes or new functionality.

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
COPYRIGHT			:=	2008-2014 Digital Aggregates Corporation, Colorado, USA.
LICENSE				:=	GNU Lesser General Public License 2.1
CONTACT				:=	coverclock@diag.com
HOMEPAGE			:=	http://www.diag.com/navigation/downloads/$(TITLE).html

# This is where I store collateral associated with projects that I have
# downloaded off the web and use without alteration. Examples: Linux kernel
# sources, toolchains, etc.
HOME_DIR			:=	$(HOME)/Projects

########## Directory Tree

ARC_DIR				:=	arc# Archive files
BIN_DIR				:=	bin# Stripped executable binaries
DOC_DIR				:=	doc# Documentation
DRV_DIR				:=	drv# Loadable kernel modules
ETC_DIR				:=	etc# Miscellaneous files
GEN_DIR				:=	gen# Generated files
INC_DIR				:=	inc# Header files
LIB_DIR				:=	lib# Shared objects
MOD_DIR				:=	mod# Loadable user modules
OUT_DIR				:=	out# Build artifacts
SRC_DIR				:=	src# Library source files
SYM_DIR				:=	sym# Unstripped executable binaries
SYS_DIR				:=	sys# Kernel module build directory
TMP_DIR				:=	tmp# Temporary files
TST_DIR				:=	tst# Unit tests

########## Configurations

ifeq ($(TARGET),diminuto)# Build for the AT91RM9200-EK with the BuildRoot system.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	-march=armv4t
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-$(PLATFORM)
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	2.6.25.10
KERNEL_DIR			:=	$(HOME_DIR)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)
HEADERS_DIR			:=	$(HOME_DIR)/$(PROJECT)/builtroot/project_build_arm/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),arroyo)# Build for the AT91RM9200-EK with the Arroyo custom system.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	-march=armv4t
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-none-$(PLATFORM)-gnueabi
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	2.6.26.3
KERNEL_DIR			:=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
HEADERS_DIR			:=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),cascada)# Build for the BeagleBoard C4 with the Angstrom system.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-none-$(PLATFORM)-gnueabi
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	2.6.32.7
KERNEL_DIR			:=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
HEADERS_DIR			:=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),contraption)# Build for the BeagleBoard C4 with the FroYo Android 2.2 system.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp
#LDARCH				:=	-static
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-none-$(PLATFORM)-gnueabi
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	2.6.32
KERNEL_DIR			:=	$(HOME_DIR)/contraption/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
HEADERS_DIR			:=	$(HOME_DIR)/contraption/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),cobbler)# Build for the Raspberry Pi version B with the Raspbian system.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	
#LDARCH				:=	-static
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-$(PLATFORM)-gnueabihf
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	rpi-3.6.y
KERNEL_DIR			:=	$(HOME_DIR)/cobbler/linux-$(KERNEL_REV)
HEADERS_DIR			:=	$(HOME_DIR)/cobbler/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),uclibc)# Build for the Raspberry Pi Version B with uClibc.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	
#LDARCH				:=	-static -L$(OUT)/$(ARC_DIR)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-buildroot-$(PLATFORM)-uclibcgnueabihf
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	rpi-3.6.y
KERNEL_DIR			:=	$(HOME_DIR)/cobbler/linux-$(KERNEL_REV)
HEADERS_DIR			:=	$(HOME_DIR)/cobbler/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),bionic)# Build for CM-10.2 on the Nexus-7 a.k.a. "flo" with bionic.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-$(PLATFORM)-androideabi
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	3.4.0-cyanogenmod-g7182995
KERNEL_DIR			:=	$(HOME)/src/cm-10.2/kernel/google/msm
HEADERS_DIR			:=	$(HOME)/src/cm-10.2/external/kernel-headers/original
# export PATH=${HOME}/src/cm-10.2/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin:${PATH}
endif

ifeq ($(TARGET),stampede)# Build for the Nvidia Jetson TK1 with Ubuntu 14.04.
ARCH				:=	arm
PLATFORM			:=	linux
CPPARCH				:=
#CARCH				:=
CARCH				:=	-rdynamic -fno-omit-frame-pointer -funwind-tables
#LDARCH				:=	-static -L$(OUT)/$(ARC_DIR)
LDARCH				:=	-Bdynamic -L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=	$(ARCH)-$(PLATFORM)-gnueabihf
CROSS_COMPILE		:=	$(TOOLCHAIN)-
KERNEL_REV			:=	3.10.24-jetson
KERNEL_DIR			:=	$(HOME_DIR)/stampede/linux-$(KERNEL_REV)
HEADERS_DIR			:=	$(HOME_DIR)/stampede/linux-headers-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),host1)# Build for an Intel build server with Ubuntu 12.04.
ARCH				:=	i386
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=
CROSS_COMPILE		:=
KERNEL_REV			:=	3.2.0-51
KERNEL_DIR			:=	/usr/src/linux-headers-$(KERNEL_REV)-generic-pae
HEADERS_DIR			:=	/usr/include
endif

ifeq ($(TARGET),host)# Build for an Intel build server with Ubuntu 14.04.
ARCH				:=	i386
PLATFORM			:=	linux
CPPARCH				:=
CARCH				:=	-rdynamic
LDARCH				:=	-L$(OUT)/$(LIB_DIR)
TOOLCHAIN			:=
CROSS_COMPILE		:=
KERNEL_REV			:=	3.13.0-24
KERNEL_DIR			:=	/usr/src/linux-headers-$(KERNEL_REV)
HEADERS_DIR			:=	/usr/include
endif

########## Variables

HERE				:=	$(shell pwd)

OUT					:=	$(OUT_DIR)/$(TARGET)

TEMP_DIR			:=	/var/tmp
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

UTILITIES			:=	
GENERATED			:=	vintage setup
ALIASES				:=	hex oct ntohs htons ntohl htonl

TARGETOBJECTS		:=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.c))))
TARGETOBJECTSXX		:=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.cpp))))
TARGETDRIVERS		:=	$(addprefix $(OUT)/,$(addsuffix .ko,$(basename $(wildcard $(DRV_DIR)/*.c))))
TARGETMODULES		:=	$(addprefix $(OUT)/,$(addsuffix .so,$(basename $(wildcard $(MOD_DIR)/*.c))))
TARGETSCRIPTS		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.sh)))
TARGETBINARIES		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.c)))
TARGETGENERATED		:=	$(addprefix $(OUT)/$(BIN_DIR)/,$(GENERATED))
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
TARGETDEFAULT		:=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) $(TARGETMODULES) $(TARGETPROGRAMS)
TARGETPACKAGE		:=	$(TARGETDEFAULT) $(TARGETDRIVERS)

PACKAGEMANIFEST		:=	./$(TITLE)/$(OUT)/$(BIN_DIR) ./$(TITLE)/$(OUT)/$(DRV_DIR) ./$(TITLE)/$(OUT)/$(LIB_DIR) ./$(TITLE)/$(OUT)/$(MOD_DIR) ./$(TITLE)/$(OUT)/$(TST_DIR)

BUILDARTIFACTS		:=	doxygen-local.cf dependencies.mk

COMMAND				:=	dummy
SCRIPT				:=	dummy

CC					:=	$(CROSS_COMPILE)gcc
CXX					:=	$(CROSS_COMPILE)g++
AR					:=	$(CROSS_COMPILE)ar
RANLIB				:=	$(CROSS_COMPILE)ranlib
STRIP				:=	$(CROSS_COMPILE)strip

CDEFINES			:=	-DCOM_DIAG_$(SYMBOL)_VINTAGE=\"$(VINTAGE)\"

ISYSTEMDIRECTORIES	:=	$(shell $(CC) -x c -E -v - < /dev/null 2>&1 | grep '^[ ]' | sed 's/^ //' | grep -v ' ')
ISYSTEMOPTIONS		:=	$(addprefix -isystem ,$(ISYSTEMDIRECTORIES))

isystem:
	echo $(ISYSTEMDIRECTORIES)
	echo $(ISYSTEMOPTIONS)

ARFLAGS				:=	rcv
CPPFLAGS			:=	$(CPPARCH) -iquote $(SRC_DIR) -iquote $(INC_DIR) $(CDEFINES)
#CPPFLAGS			:=	$(CPPARCH) -iquote $(SRC_DIR) -iquote $(INC_DIR) -isystem $(HEADERS_DIR) $(CDEFINES)
CXXFLAGS			:=	$(CARCH) -fPIC -g
CFLAGS				:=	$(CARCH) -fPIC -g
#CXXFLAGS			:=	$(CARCH) -fPIC -O3
#CFLAGS				:=	$(CARCH) -fPIC -O3
CPFLAGS				:=	-i
MVFLAGS				:=	-i
LDFLAGS				:=	$(LDARCH) -l$(PROJECT) -lpthread -lrt -ldl
LDXXFLAGS			:=	$(LDARCH) -l$(PROJECT)xx -l$(PROJECT) -lpthread -lrt -ldl

BROWSER				:=	firefox

########## Main Entry Points

.PHONY:	default all dist clean pristine

default:	$(TARGETDEFAULT)

all:	$(UTILITIES) $(TARGETPACKAGE)

dist:	distribution

clean:
	rm -rf $(OUT) $(UTILITIES) $(BUILDARTIFACTS)
	rm -rf $(DOC_DIR)

pristine:	clean
	rm -rf $(OUT_DIR)

########## Distribution

.PHONY:	distribution

distribution:
	rm -rf $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	svn export $(SVNURL) $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	( cd $(TEMP_DIR); tar cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) ) > $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
	( cd $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD); make TARGET=host; ./out/host/bin/vintage )

.PHONY:	package

package $(PROJECT)-$(TARGET).tgz:
	( cd ..; mkdir -p $(PACKAGEMANIFEST) )
	tar -C .. -cvzf - $(PACKAGEMANIFEST) > $(PROJECT)-$(TARGET).tgz

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
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(ARC_DIR)/lib$(PROJECT).a
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CARCH) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o $@ -Wl,--whole-archive $< -Wl,--no-whole-archive

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 
	
########## Target C++ Libraries

$(OUT)/$(ARC_DIR)/lib$(PROJECT)xx.a:	$(TARGETOBJECTSXX)
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(ARC_DIR)/lib$(PROJECT)xx.a
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CARCH) -shared -Wl,-soname,lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) -o $@ -Wl,--whole-archive $< -Wl,--no-whole-archive

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

########## Target Unstripped Binaries

$(OUT)/$(SYM_DIR)/%:	$(BIN_DIR)/%.c $(TARGETLIBRARIES)
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CPPFLAGS) -I $(KERNEL_DIR)/include $(CFLAGS) -o $@ $< $(LDFLAGS)
	
########## Target Aliases

$(OUT)/$(BIN_DIR)/hex $(OUT)/$(BIN_DIR)/oct $(OUT)/$(BIN_DIR)/ntohs $(OUT)/$(BIN_DIR)/htons $(OUT)/$(BIN_DIR)/ntohl $(OUT)/$(BIN_DIR)/htonl:	$(OUT)/$(BIN_DIR)/dec
	ln -f $< $@
	
########## Unit Tests

# Generally if the unit tests work they exit with an exit code of zero;
# otherwise they exit with something else (or core dump). But left to their
# own devices, then unit tests aren't very chatty. If you want to see more log
# output from them, you can tune the logging using the environmental variable
# DIMINUTO_LOG_MASK. For example, the bash command
#
#	export DIMINUTO_LOG_MASK=0xffff
#
# turns on everything.

$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.c $(TARGETLIBRARIES)
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.cpp $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Generated

.PHONY:	$(OUT)/$(GEN_DIR)/vintage.c $(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_release.h $(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_vintage.h

# For embedding in a system where it can be executed from a shell.
# The major.minor.build is emitted to standard output, a bunch more
# metadata to standard error. Hence, they can be redirected to separate
# files. The metadata file is intended to parsable as a standard properties
# file (although I have not tried it).
$(OUT)/$(GEN_DIR)/vintage.c:	$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_release.h $(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_vintage.h Makefile
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
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
	echo "    \"Platform = $(PLATFORM)\\\\n\"" >> $@
	echo "    \"Toolchain = $(TOOLCHAIN)\\\\n\"" >> $@
	echo ';' >> $@
	echo 'int main(void) {' >> $@
	echo '    fputs(METADATA, stderr);' >> $@
	echo '    fprintf(stderr, "RELEASE_KEYWORD = %s\\n", DIMINUTO_RELEASE_KEYWORD);' >> $@
	echo '    fprintf(stderr, "RELEASE_VALUE = %s\\n", DIMINUTO_RELEASE_VALUE);' >> $@
	echo '    fprintf(stderr, "VINTAGE_KEYWORD = %s\\n", DIMINUTO_VINTAGE_KEYWORD);' >> $@
	echo '    fprintf(stderr, "VINTAGE_VALUE = %s\\n", DIMINUTO_VINTAGE_VALUE);' >> $@
	echo '    fputs("$(MAJOR).$(MINOR).$(BUILD)\\n", stdout);' >> $@
	echo '    return 0;' >> $@
	echo '}' >> $@

# Why do we generate the release and vintage header files into a source code
# directory? Because a common thing to do is to build the library, then
# copy the include directory and the library or shared object to some public
# location. We want the generated header files to be in the public include
# directory with all of the other header files.

# For embedding in an application where it can be interrogated or displayed.
$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_release.h:
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_RELEASE_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_RELEASE_' >> $@
	echo "static const char DIMINUTO_RELEASE_KEYWORD[] = \"DIMINUTO_RELEASE=$(MAJOR).$(MINOR).$(BUILD)\";" >> $@
	echo "static const char * DIMINUTO_RELEASE_VALUE = &DIMINUTO_RELEASE_KEYWORD[sizeof(\"DIMINUTO_RELEASE=\") - 1];" >> $@
	echo '#endif' >> $@

# For embedding in an application where it can be interrogated or displayed.
$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_vintage.h:
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_VINTAGE_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_VINTAGE_' >> $@
	echo "static const char DIMINUTO_VINTAGE_KEYWORD[] = \"DIMINUTO_VINTAGE=$(VINTAGE)\";" >> $@
	echo "static const char * DIMINUTO_VINTAGE_VALUE = &DIMINUTO_VINTAGE_KEYWORD[sizeof(\"DIMINUTO_VINTAGE=\") - 1];" >> $@
	echo '#endif' >> $@

$(OUT)/$(SYM_DIR)/vintage:	$(OUT)/$(GEN_DIR)/vintage.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

# For sourcing into a bash shell (for example, ". setup").
$(OUT)/$(BIN_DIR)/setup:	Makefile
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	echo 'COM_DIAG_$(SYMBOL)_PATH=`dirname $${BASH_ARGV[0]}`; COM_DIAG_$(SYMBOL)_ROOT=`cd $$COM_DIAG_$(SYMBOL)_PATH; pwd`' > $@
	echo 'export PATH=$$PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../bin:$$COM_DIAG_$(SYMBOL)_ROOT/../tst' >> $@
	echo 'export LD_DRIVER_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../drv' >> $@
	echo 'export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../lib' >> $@
	echo 'export LD_MODULE_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../mod' >> $@

########## User-Space Loadable Modules

LDWHOLEARCHIVES=# These archives will be linked into the shared object in their entirety.

$(OUT)/$(MOD_DIR)/%.so:	$(MOD_DIR)/%.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -shared $< $(LDFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

########## Kernel-Space Loadable Modules

.PHONY:	drivers drivers-clean

OBJ_M =	$(addsuffix .o,$(basename $(shell cd $(DRV_DIR); ls *.c)))

$(OUT)/$(SYS_DIR)/Makefile:	Makefile
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo "obj-m := $(OBJ_M)" >> $@
	echo "EXTRA_CFLAGS := -iquote $(HERE)/$(INC_DIR) -iquote $(HERE)/$(TST_DIR)" >> $@

$(OUT)/$(SYS_DIR)/%.c:	$(DRV_DIR)/%.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	cp $< $@

$(OUT)/$(DRV_DIR)/%.ko:	$(OUT)/$(SYS_DIR)/%.ko
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	cp $< $@

$(OUT)/$(SYS_DIR)/diminuto_mmdriver.ko $(OUT)/$(SYS_DIR)/diminuto_utmodule.ko $(OUT)/$(SYS_DIR)/diminuto_kernel_datum.ko $(OUT)/$(SYS_DIR)/diminuto_kernel_map.ko:	$(OUT)/$(SYS_DIR)/Makefile $(OUT)/$(SYS_DIR)/diminuto_mmdriver.c $(OUT)/$(SYS_DIR)/diminuto_utmodule.c $(OUT)/$(SYS_DIR)/diminuto_kernel_datum.c $(OUT)/$(SYS_DIR)/diminuto_kernel_map.c
	make -C $(KERNEL_DIR) M=$(shell cd $(OUT)/$(SYS_DIR); pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

drivers:	$(OUT)/$(DRV_DIR)/diminuto_mmdriver.ko $(OUT)/$(DRV_DIR)/diminuto_utmodule.ko $(OUT)/$(DRV_DIR)/diminuto_kernel_datum.ko $(OUT)/$(DRV_DIR)/diminuto_kernel_map.ko

drivers-clean:
	make -C $(KERNEL_DIR) M=$(shell cd $(OUT)/$(SYS_DIR); pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) clean
	rm -f $(OUT)/$(SYS_DIR)/Makefile

########## Helpers

.PHONY:	backup implicit

backup:	../$(PROJECT).bak.tgz
	mv $(MVFLAGS) ../$(PROJECT).bak.tgz ../$(PROJECT).$(TIMESTAMP).tgz

../$(PROJECT).bak.tgz:
	tar cvzf - . > $@
	
implicit:
	$(CC) $(CPPFLAGS) $(CFLAGS) -dM -E - < /dev/null

########## Documentation

.PHONY:	documentation browse refman manpages

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

.PHONY:	patch

patch:	$(OLD) $(NEW)
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

$(OUT)/%:	%.sh
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	cp $< $@
	chmod 755 $@
	
.SECONDARY:

$(OUT)/$(BIN_DIR)/%:	$(OUT)/$(SYM_DIR)/%
	D=`dirname $@`; test -d $$D || mkdir -p $$D
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
