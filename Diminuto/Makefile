# vi: set ts=4 shiftwidth=4:
# Copyright 2008-2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# https://github.com/coverclock/com-diag-diminuto
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.
#
#	make all				# Build libraries, unit tests, executable programs.
#	make clean				# Remove OUT directory for current TARGET.
#	make pristine			# Remove all OUT directories.
#	sudo make install		# Install libraries, binaries, headers under /usr/local.
#	. out/host/bin/setup	# Sets up PATH and LD_LIBRARY_PATH after build.
#

.PHONY:	default

default:	all

########## Metadata

COPYRIGHT			:=	2008-2021 Digital Aggregates Corporation
LICENSE				:=	GNU Lesser General Public License 2.1
CONTACT				:=	coverclock@diag.com
HOMEPAGE			:=	https://github.com/coverclock/com-diag-diminuto

########## Customizations

TITLE				:=	Diminuto

MAJOR				:=	73# API changes that may require that applications be modified.
MINOR				:=	1# Only functionality or features added with no legacy API changes.
BUILD				:=	0# Only bugs fixed with no API changes or new functionality.

# Disclaimer: the only target that I routinely build and test for is "host",
# using the native GCC compilers on my X86_64 Ubuntu or ARMv7 Raspbian systems.
# While Diminuto has been built, tested, and used on all of these other targets
# at one time or another, # the current version has only been tested on whatever
# project I'm currently working on. The hardware for some of these targets
# isn't available to me anymore.

TARGET				:=	host# Build for the current host.
#TARGET				:=	arroyo# Build for the AT91RM9200-EK running roll-your-own.
#TARGET				:=	betty# Build for the Raspberry Pi 2 running Raspbian.
#TARGET				:=	cascada# Build for the BeagleBoard C4 running Angstrom.
#TARGET				:=	cobbler# Build for the Raspberry Pi version B running Raspbian.
#TARGET				:=	contraption# Build for the BeagleBoard C4 running Android 2.2 but with glibc.
#TARGET				:=	corset# Build for CyanogenMod 10.2 on a Nexus-7 running Android/bionic.
#TARGET				:=	diminuto# Build for the AT91RM9200-EK running BuildRoot.
#TARGET				:=	dumpling# Build for the Raspberry Pi Version B running buildroot/uClibc.
#TARGET				:=	host1204# Build for the x86 running Ubuntu 12.04 LTS.
#TARGET				:=	notgnu# Build for a host that is not GNU-based (testing).
#TARGET				:=	orchard# Mac OS X 10.9.5 running on a Mac Mini "late 2012".
#TARGET				:=	petticoat# Build inside cros_sdk chroot for the x86-generic running Chromium OS.
#TARGET				:=	schoolmarm# Windows 8.1 Pro running Cygwin 2.850.
#TARGET				:=	stampede# Build for the Nvidia Jetson TK1 running Ubuntu 14.04.

# This is where I store collateral associated with projects that I have
# downloaded off the web and use without alteration. Examples: Linux kernel
# sources, toolchains, etc.
HOME_DIR			:=	$(HOME)/Projects

########## Directories

ARC_DIR				:=	arc# Archive files for static linking
BIN_DIR				:=	bin# Utilities or stripped binaries
CFG_DIR				:=	cfg# Build configuration files
DEP_DIR				:=	dep# Generated dependencies and other make files
DOC_DIR				:=	doc# Documentation
DRV_DIR				:=	drv# Loadable kernel modules
ETC_DIR				:=	etc# Miscellaneous files
FUN_DIR				:=	fun# Functional tests
GEN_DIR				:=	gen# Generated files
INC_DIR				:=	inc# Header files
LIB_DIR				:=	lib# Shared objects for dynamic linking
MOD_DIR				:=	mod# Loadable user modules
OBC_DIR				:=	obc# C object modules
OBX_DIR				:=	obx# C++ object modules
OUT_DIR				:=	out# Build artifacts
SRC_DIR				:=	src# Library components
SYM_DIR				:=	sym# Unstripped executable binaries
SYS_DIR				:=	sys# Kernel module build directory
TGZ_DIR				:=	tgz# Compressed tarballs
TST_DIR				:=	tst# Unit tests

########## Configuration

PROJECT				:=	$(shell echo $(TITLE) | tr '[A-Z]' '[a-z]')
SYMBOL				:=	$(shell echo $(TITLE) | tr '[a-z]' '[A-Z]')

HERE				:=	$(shell pwd)

OUT					:=	$(OUT_DIR)/$(TARGET)

TEMP_DIR			:=	/tmp
ROOT_DIR			:=	$(HOME_DIR)/$(TARGET)

GITURL				:=	https://github.com/coverclock/com-diag-$(PROJECT).git

GENERATED			:=	vintage generate setup $(PROJECT)
SYNTHESIZED			:=	$(PROJECT)_release.h $(PROJECT)_vintage.h $(PROJECT)_revision.h

ALIASES				:=	hex oct ntohs htons ntohl htonl

NEW					:=	dummy
OLD					:=	dummy

PACKAGE				:=	$(OUT)/$(TGZ_DIR)/$(PROJECT)-$(TARGET)

MANIFEST			:=	$(ARC_DIR) $(BIN_DIR) $(DRV_DIR) $(INC_DIR) $(LIB_DIR) $(MOD_DIR) $(TST_DIR) $(FUN_DIR)

DISTRIBUTION		:=	$(OUT)/$(TGZ_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)

TARBALL				:=	$(OUT)/$(TGZ_DIR)/$(PROJECT)

SO					:=	so

A2SOARCH			:=	-shared -Wl,-soname,lib$(PROJECT).$(SO).$(MAJOR).$(MINOR)
A2SOXXARCH			:=	-shared -Wl,-soname,lib$(PROJECT)xx.$(SO).$(MAJOR).$(MINOR)

########## Configuration Makefile

TARGETMAKEFILE		:= $(CFG_DIR)/$(TARGET).mk

include $(TARGETMAKEFILE)

########## Commands and Option Flags

PROJECT_A			:=	lib$(PROJECT).a
PROJECTXX_A			:=	lib$(PROJECT)xx.a
PROJECT_SO			:=	lib$(PROJECT).$(SO)
PROJECTXX_SO		:=	lib$(PROJECT)xx.$(SO)

CROSS_COMPILE		:=	$(TOOLCHAIN)

CC					:=	$(CROSS_COMPILE)gcc
CXX					:=	$(CROSS_COMPILE)g++
LD					:=	$(CROSS_COMPILE)ld
AR					:=	$(CROSS_COMPILE)ar
RANLIB				:=	$(CROSS_COMPILE)ranlib
STRIP				:=	$(CROSS_COMPILE)strip
LDD					:=	$(CROSS_COMPILE)ldd

CDEFINES			:=

ARFLAGS				:=	crsv
#CPPFLAGS			:=	$(CDEFINES) -I $(SRC_DIR) -I $(INC_DIR) -I $(OUT)/$(INC_DIR) $(CPPARCH)
CPPFLAGS			:=	$(CDEFINES) -iquote $(SRC_DIR) -iquote $(INC_DIR) -iquote $(OUT)/$(INC_DIR) $(CPPARCH)
CFLAGS				:=	$(CARCH) -g
#CFLAGS				:=	$(CARCH) -g -pedantic
#CFLAGS				:=	$(CARCH) -g -fprofile-arcs -ftest-coverage
#CFLAGS				:=	$(CARCH) -O3
CXXFLAGS			:=	$(CXXARCH) -g
#CXXFLAGS			:=	$(CXXARCH) -g -pedantic
#CXXFLAGS			:=	$(CXXARCH) -g -fprofile-arcs -ftest-coverage
#CXXFLAGS			:=	$(CXXARCH) -O3
CPFLAGS				:=	-i
MVFLAGS				:=	-i
LDFLAGS				:=	$(LDARCH) -l$(PROJECT) $(LDLIBRARIES)
LDXXFLAGS			:=	$(LDARCH) -l$(PROJECT)xx -l$(PROJECT) $(LDXXLIBRARIES)
MOFLAGS				:=	$(MOARCH) -l$(PROJECT) $(LDLIBRARIES)
MOXXFLAGS			:=	$(MOARCH) -l$(PROJECT)xx -l$(PROJECT) $(LDXXLIBRARIES)
SOFLAGS				:=	$(SOARCH) $(LDLIBRARIES)
SOXXFLAGS			:=	$(SOXXARCH) $(LDXXLIBRARIES)

########## Environment

HOST				:=	$(shell hostname -s)
BRANCH				:=	$(shell git rev-parse --abbrev-ref HEAD)
REVISION			:=	$(shell git rev-parse HEAD)
MODIFIED			:=	$(shell date -u -d @$(shell git log -1 --format="%at") +%Y-%m-%dT%H:%M:%S.%N%z)
ROOT				:=	$(shell git rev-parse --show-toplevel)
VINTAGE				:=	$(shell date -u +%Y-%m-%dT%H:%M:%S.%N%z)
MAKEVERSION			:=	$(shell make --version < /dev/null 2>&1 | head -1)
CCVERSION			:=	$(shell $(CC) --version < /dev/null 2>&1 | head -1)
LIBCVERSION			:=	$(shell $(LDD) --version < /dev/null 2>&1 | head -1)
DISTROVERSION		:=	$(shell lsb_release -d | sed 's/^.*:\t*//') $(shell lsb_release -c | sed 's/^.*:\t*//')

########## Build Artifacts

TARGETOBJECTS		:=	$(addprefix $(OUT)/$(OBC_DIR)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.c))))
TARGETOBJECTSXX		:=	$(addprefix $(OUT)/$(OBX_DIR)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.cpp))))
TARGETDRIVERS		:=	$(addprefix $(OUT)/,$(addsuffix .ko,$(basename $(wildcard $(DRV_DIR)/*.c))))
TARGETMODULES		:=	$(addprefix $(OUT)/,$(addsuffix .$(SO),$(basename $(wildcard $(MOD_DIR)/*.c))))
TARGETSCRIPTS		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.sh)))
TARGETBINARIES		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.c)))
TARGETGENERATED		:=	$(addprefix $(OUT)/$(BIN_DIR)/,$(GENERATED)) $(addprefix $(OUT)/$(SYM_DIR)/,$(GENERATED))
TARGETSYNTHESIZED	:=	$(addprefix $(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/,$(SYNTHESIZED))
TARGETALIASES		:=	$(addprefix $(OUT)/$(BIN_DIR)/,$(ALIASES))
TARGETUNITTESTS		:=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.c)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.cpp)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.sh)))
TARGETFUNCTIONALS	:=	$(addprefix $(OUT)/,$(basename $(wildcard $(FUN_DIR)/*.c)))
TARGETFUNCTIONALS	+=	$(addprefix $(OUT)/,$(basename $(wildcard $(FUN_DIR)/*.sh)))

TARGETARCHIVE		:=	$(OUT)/$(ARC_DIR)/$(PROJECT_A)
TARGETARCHIVEXX		:=	$(OUT)/$(ARC_DIR)/$(PROJECTXX_A)
TARGETSHARED		:=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO)
TARGETSHAREDXX		:=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO)

TARGETLIBRARIES		:=	$(TARGETARCHIVE) $(TARGETSHARED)
TARGETLIBRARIESXX	:=	$(TARGETARCHIVEXX) $(TARGETSHAREDXX)
TARGETPROGRAMS		:=	$(TARGETBINARIES) $(TARGETALIASES) $(TARGETUNITTESTS) $(TARGETFUNCTIONALS) $(TARGETGENERATED) $(TARGETSCRIPTS)
TARGETALL			:=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) $(TARGETPROGRAMS)
TARGETMOST			:=	$(TARGETALL) $(TARGETMODULES)
TARGETPLUS			:=	$(TARGETMOST) $(TARGETDRIVERS)
TARGETNOTESTS		:=	$(TARGETBINARIES) $(TARGETALIASES) $(TARGETFUNCTIONALS) $(TARGETGENERATED) $(TARGETSCRIPTS)

########## Main Entry Points

.PHONY:	all most plus dist tidy clean pristine clobber scratch notests onlytests

all:	$(TARGETALL)

most:	$(TARGETMOST)

# On the Raspberry Pi, this is what worked for me:
# sudo apt-get install raspberrypi-kernel-headers
plus:	$(TARGETPLUS)

notests:	$(TARGETNOTESTS)

onlytests:	$(TARGETUNITTESTS)

dist:	distribution

tidy:
	rm -rf $(OUT)/arc $(OUT)/bin $(OUT)/dep $(OUT)/gen $(OUT)/inc $(OUT)/lib $(OUT)/obc $(OUT)/sym $(OUT)/tst

clean:
	rm -rf $(OUT)

pristine:	clean
	rm -rf $(OUT_DIR)

# This is not the same as simply listing the targets as dependencies.
scratch:
	make pristine
	make depend
	make all

clobber:	pristine
	rm -f .cscope.lst .cscope.out .cscope.out.in .cscope.out.po
	rm -rf *.gcda *.gcno *.gcov

########## Packaging and Distribution

# Useful for copying the executables over to another target for which they were
# cross compiled.

.PHONY:	package

package $(PACKAGE).tgz:
	D=`dirname $(PACKAGE)`; mkdir -p $$D; \
	for M in $(MANIFEST); do mkdir -p $(OUT)/$$M; done; \
	T=`mktemp -d "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
	B=`basename $(PACKAGE)`; mkdir -p $$T/$$B; \
	tar -C $(OUT) --exclude-vcs -cvf - $(MANIFEST) | tar -C $$T/$$B -xvf -; \
	tar --exclude-vcs -cvf - $(INC_DIR) | tar -C $$T/$$B -xvf -; \
	tar -C $$T --exclude-vcs -cvzf - $$B > $(PACKAGE).tgz; \
	rm -rf $$T

# Useful for given someone a tarball of the actual source distribution that
# is guaranteed to at least build.

.PHONY:	distribution distro

distribution distro $(DISTRIBUTION).tgz:
	D=`dirname $(DISTRIBUTION)`; mkdir -p $$D; \
	T=`mktemp -d "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
	B=`basename $(DISTRIBUTION)`; \
	( cd $$T; git clone $(GITURL) $$B ); \
	tar -C $$T --exclude-vcs -cvzf - ./$$B > $(DISTRIBUTION).tgz; \
	rm -rf $$T/$$B; \
	tar -C $$T -xvzf - < $(DISTRIBUTION).tgz; \
	( cd $$T/$$B/$(TITLE); make all TARGET=host OUT=out/host && $(OUT)/bin/generate > ./setup && . ./setup && vintage ); \
	rm -rf $$T

# Useful for copying a tarball of the current development source base to a
# target for which there is no cross compiler toolchain.

.PHONY:	tarball

tarball $(TARBALL).tgz:
	D=`dirname $(TARBALL)`; mkdir -p $$D; \
	T=`pwd`; \
	B=`basename $$T`; \
	( tar -C .. --exclude-vcs --exclude=$(OUT_DIR) --exclude=.??* -cvzf - $$B ) > $(TARBALL).tgz; \

# Useful for backing a complete backup before doing something that may turn
# out to be profoundly stupid, like running a transformative script across all
# the source files. Sure, your local and remote repos should save you, but what
# if they didn't?

.PHONY:	backup

backup:
	tar --exclude=$(OUT_DIR) -cvzf - . > $(HOME)/$(PROJECT)-$(shell date -u +%Y%m%d%H%M%S%N%Z).tgz

########## Target C Libraries

$(OUT)/$(ARC_DIR)/$(PROJECT_A):	$(TARGETOBJECTS)
	D=`dirname $@`; mkdir -p $$D
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).$(SO).$(MAJOR).$(MINOR):	$(TARGETOBJECTS)
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CFLAGS) $(A2SOARCH) -o $@ $(SOFLAGS) $(TARGETOBJECTS)

$(OUT)/$(LIB_DIR)/lib$(PROJECT).$(SO).$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).$(SO).$(MAJOR).$(MINOR)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).$(SO):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).$(SO).$(MAJOR)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

########## Target C++ Libraries

$(OUT)/$(ARC_DIR)/$(PROJECTXX_A):	$(TARGETOBJECTSXX)
	D=`dirname $@`; mkdir -p $$D
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.$(SO).$(MAJOR).$(MINOR):	$(TARGETOBJECTSXX) $(OUT)/$(LIB_DIR)/lib$(PROJECT).$(SO).$(MAJOR).$(MINOR)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CXXFLAGS) $(A2SOXXARCH) -o $@ $(SOXXFLAGS) $(TARGETOBJECTSXX)

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.$(SO).$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.$(SO).$(MAJOR).$(MINOR)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.$(SO):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.$(SO).$(MAJOR)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

########## Target Unstripped Binaries

$(OUT)/$(SYM_DIR)/%:	$(OUT)/$(OBC_DIR)/$(BIN_DIR)/%.o $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(OUT)/$(SYM_DIR)/%:	$(OUT)/$(OBX_DIR)/$(BIN_DIR)/%.o $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Target Aliases

$(OUT)/$(BIN_DIR)/hex $(OUT)/$(BIN_DIR)/oct $(OUT)/$(BIN_DIR)/ntohs $(OUT)/$(BIN_DIR)/htons $(OUT)/$(BIN_DIR)/ntohl $(OUT)/$(BIN_DIR)/htonl:	$(OUT)/$(BIN_DIR)/dec
	ln -f $< $@

########## Unit Tests

# Generally if the unit tests work they exit with an exit code of zero;
# otherwise they exit with something else (or core dump). But left to their
# own devices, the unit tests aren't very chatty. If you want to see more log
# output from them, you can tune the logging using the environmental variable
# COM_DIAG_DIMINUTO_LOG_MASK. For example, this bash command turns on
# everything.
#
#	export COM_DIAG_DIMINUTO_LOG_MASK=0xff
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

$(OUT)/$(TST_DIR)/%:	$(OUT)/$(OBC_DIR)/$(TST_DIR)/%.o $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
$(OUT)/$(TST_DIR)/%:	$(OUT)/$(OBX_DIR)/$(TST_DIR)/%.o $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Functional Tests

# Most (perhaps all) functional tests require a special hardware test
# fixture to run them. That's why they are functional tests instead of
# unit tests.

$(OUT)/$(FUN_DIR)/%:	$(OUT)/$(OBC_DIR)/$(FUN_DIR)/%.o $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
$(OUT)/$(FUN_DIR)/%:	$(OUT)/$(OBX_DIR)/$(FUN_DIR)/%.o $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Generated Source Files

# For embedding in a system where it can be executed from a shell.
#
# The major.minor.build is emitted to standard error, a bunch more
# metadata to standard output. Hence, they can be redirected to separate
# files.
#
# Some of the information in this binary executable may be sensitive, for
# example, the Directory or the User. That's why it's in a form that doesn't
# have to be distributed with the product, unlike the shared objects.
#
# This program also serves as a unit test, so be careful about removing stuff
# from it that looks redundant or unused.
#
# The stdout stream from vintage is designed so that you can source it
# into a variety of tools including bash. e.g. eval $(vintage 2> /dev/null) .
#
# NOTE: in the generated C code below, whether you should have \\n or \n in the
# echo statements seems to depend on what version of make you are running. I've
# tried to automate this - having this Makefile determine your make version - but
# have not yet discovered the fu necessary to make that work reliably.

# Some civilian, defense, or intelligence agencies (e.g. the U.S. Federal
# Aviation Administration or FAA) require that software builds for safety
# critical or national security applications generate exactly the same binary
# images bit for bit if the source code has not changed. (This is theoretically
# a more stringent requirement than requiring that checksums or cryptographic
# hashes are the same, although in practice it is the same thing.) This allows
# agency inspectors to verify the integrity of the binary software images. This
# makes embedding build timestamps inside compiled translation units problematic.
# Avoid using the built-in C macros __DATE__ and __TIME__, and consider using the
# faketime(1) utility to spoof the system time during builds.

SLASHES=2

ifeq ($(SLASHES), 1)

$(OUT)/$(GEN_DIR)/vintage.c:	Makefile
	@echo MAKE_VERSION=$(MAKE_VERSION) SLASHES=$(SLASHES)
	D=`dirname $@`; mkdir -p $$D
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_revision.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_revision.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_platform.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_platform.h"' >> $@
	echo '#include <stdio.h>' >> $@
	echo '#include <assert.h>' >> $@
	echo 'static const char METADATA[] =' >> $@
	echo '    "Arch=\"$(ARCH)\"\n"' >> $@
	echo '    "Branch=\"$(BRANCH)\"\n"' >> $@
	echo '    "Cc=\"$(CCVERSION)\"\n"' >> $@
	echo '    "Contact=\"$(CONTACT)\"\n"' >> $@
	echo '    "Copyright=\"$(COPYRIGHT)\"\n"' >> $@
	echo '    "Distro=\"$(DISTROVERSION)\"\n"' >> $@
	echo '    "Homepage=\"$(HOMEPAGE)\"\n"' >> $@
	echo '    "Host=\"$(HOST)\"\n"' >> $@
	echo '    "Kernel=\"$(KERNEL_REV)\"\n"' >> $@
	echo '    "Libc=\"$(LIBCVERSION)\"\n"' >> $@
	echo '    "License=\"$(LICENSE)\"\n"' >> $@
	echo '    "Make=\"$(MAKEVERSION)\"\n"' >> $@
	echo '    "Modified=\"$(MODIFIED)\"\n"' >> $@
	echo '    "Os=\"$(OS)\"\n"' >> $@
	echo '    "Platform=\"" COM_DIAG_$(SYMBOL)_PLATFORM "\"\n"' >> $@
	echo '    "Release=\"" COM_DIAG_$(SYMBOL)_RELEASE "\"\n"' >> $@
	echo '    "Repository=\"$(GITURL)\"\n"' >> $@
	echo '    "Revision=\"" COM_DIAG_$(SYMBOL)_REVISION "\"\n"' >> $@
	echo '    "Root=\"$(ROOT)\"\n"' >> $@
	echo '    "Target=\"$(TARGET)\"\n"' >> $@
	echo '    "Title=\"$(TITLE)\"\n"' >> $@
	echo '    "Toolchain=\"$(TOOLCHAIN)\"\n"' >> $@
	echo '    "User=\"$(USER)\"\n"' >> $@
	echo '    "Vintage=\"" COM_DIAG_$(SYMBOL)_VINTAGE "\"\n"' >> $@
	echo ';' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_RELEASE_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_RELEASE_VALUE;' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_VINTAGE_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_VINTAGE_VALUE;' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_REVISION_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_REVISION_VALUE;' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_PLATFORM_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_PLATFORM_VALUE;' >> $@
	echo 'int main(void) {' >> $@
	echo '    const char * release_keyword = (const char *)0;' >> $@
	echo '    const char * release_value = (const char *)0;' >> $@
	echo '    const char * vintage_keyword = (const char *)0;' >> $@
	echo '    const char * vintage_value = (const char *)0;' >> $@
	echo '    const char * revision_keyword = (const char *)0;' >> $@
	echo '    const char * revision_value = (const char *)0;' >> $@
	echo '    const char * platform_keyword = (const char *)0;' >> $@
	echo '    const char * platform_value = (const char *)0;' >> $@
	echo '    fputs(METADATA, stdout);' >> $@
	echo '    fputs("$(MAJOR).$(MINOR).$(BUILD)\n", stderr);' >> $@
	echo '    release_keyword = COM_DIAG_$(SYMBOL)_RELEASE_KEYWORD;' >> $@
	echo '    release_value = COM_DIAG_$(SYMBOL)_RELEASE_VALUE;' >> $@
	echo '    vintage_keyword = COM_DIAG_$(SYMBOL)_VINTAGE_KEYWORD;' >> $@
	echo '    vintage_value = COM_DIAG_$(SYMBOL)_VINTAGE_VALUE;' >> $@
	echo '    revision_keyword = COM_DIAG_$(SYMBOL)_REVISION_KEYWORD;' >> $@
	echo '    revision_value = COM_DIAG_$(SYMBOL)_REVISION_VALUE;' >> $@
	echo '    platform_keyword = COM_DIAG_$(SYMBOL)_PLATFORM_KEYWORD;' >> $@
	echo '    platform_value = COM_DIAG_$(SYMBOL)_PLATFORM_VALUE;' >> $@
	echo '    assert(release_keyword != (const char *)0);' >> $@
	echo '    assert(release_value != (const char *)0);' >> $@
	echo '    assert(vintage_keyword != (const char *)0);' >> $@
	echo '    assert(vintage_value != (const char *)0);' >> $@
	echo '    assert(release_keyword != (const char *)0);' >> $@
	echo '    assert(release_value != (const char *)0);' >> $@
	echo '    assert(platform_keyword != (const char *)0);' >> $@
	echo '    assert(platform_value != (const char *)0);' >> $@
	echo '    return 0;' >> $@
	echo '}' >> $@

endif

ifeq ($(SLASHES), 2)

$(OUT)/$(GEN_DIR)/vintage.c:	Makefile
	@echo MAKE_VERSION=$(MAKE_VERSION) SLASHES=$(SLASHES)
	D=`dirname $@`; mkdir -p $$D
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_release.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_vintage.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_revision.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_revision.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_platform.h"' >> $@
	echo '#include "com/diag/$(PROJECT)/$(PROJECT)_platform.h"' >> $@
	echo '#include <stdio.h>' >> $@
	echo '#include <assert.h>' >> $@
	echo 'static const char METADATA[] =' >> $@
	echo '    "Arch=\\"$(ARCH)\\"\\n"' >> $@
	echo '    "Branch=\\"$(BRANCH)\\"\\n"' >> $@
	echo '    "Cc=\\"$(CCVERSION)\\"\\n"' >> $@
	echo '    "Contact=\\"$(CONTACT)\\"\\n"' >> $@
	echo '    "Copyright=\\"$(COPYRIGHT)\\"\\n"' >> $@
	echo '    "Distro=\\"$(DISTROVERSION)\\"\\n"' >> $@
	echo '    "Homepage=\\"$(HOMEPAGE)\\"\\n"' >> $@
	echo '    "Host=\\"$(HOST)\\"\\n"' >> $@
	echo '    "Kernel=\\"$(KERNEL_REV)\\"\\n"' >> $@
	echo '    "Libc=\\"$(LIBCVERSION)\\"\\n"' >> $@
	echo '    "License=\\"$(LICENSE)\\"\\n"' >> $@
	echo '    "Make=\\"$(MAKEVERSION)\\"\\n"' >> $@
	echo '    "Modified=\\"$(MODIFIED)\\"\\n"' >> $@
	echo '    "Os=\\"$(OS)\\"\\n"' >> $@
	echo '    "Platform=\\"" COM_DIAG_$(SYMBOL)_PLATFORM "\\"\\n"' >> $@
	echo '    "Release=\\"" COM_DIAG_$(SYMBOL)_RELEASE "\\"\\n"' >> $@
	echo '    "Repository=\\"$(GITURL)\\"\\n"' >> $@
	echo '    "Revision=\\"" COM_DIAG_$(SYMBOL)_REVISION "\\"\\n"' >> $@
	echo '    "Root=\\"$(ROOT)\\"\\n"' >> $@
	echo '    "Target=\\"$(TARGET)\\"\\n"' >> $@
	echo '    "Title=\\"$(TITLE)\\"\\n"' >> $@
	echo '    "Toolchain=\\"$(TOOLCHAIN)\\"\\n"' >> $@
	echo '    "User=\\"$(USER)\\"\\n"' >> $@
	echo '    "Vintage=\\"" COM_DIAG_$(SYMBOL)_VINTAGE "\\"\\n"' >> $@
	echo ';' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_RELEASE_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_RELEASE_VALUE;' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_VINTAGE_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_VINTAGE_VALUE;' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_REVISION_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_REVISION_VALUE;' >> $@
	echo 'extern const char COM_DIAG_$(SYMBOL)_PLATFORM_KEYWORD[];' >> $@
	echo 'extern const char * COM_DIAG_$(SYMBOL)_PLATFORM_VALUE;' >> $@
	echo 'int main(void) {' >> $@
	echo '    const char * release_keyword = (const char *)0;' >> $@
	echo '    const char * release_value = (const char *)0;' >> $@
	echo '    const char * vintage_keyword = (const char *)0;' >> $@
	echo '    const char * vintage_value = (const char *)0;' >> $@
	echo '    const char * revision_keyword = (const char *)0;' >> $@
	echo '    const char * revision_value = (const char *)0;' >> $@
	echo '    const char * platform_keyword = (const char *)0;' >> $@
	echo '    const char * platform_value = (const char *)0;' >> $@
	echo '    fputs(METADATA, stdout);' >> $@
	echo '    fputs("$(MAJOR).$(MINOR).$(BUILD)\\n", stderr);' >> $@
	echo '    release_keyword = COM_DIAG_$(SYMBOL)_RELEASE_KEYWORD;' >> $@
	echo '    release_value = COM_DIAG_$(SYMBOL)_RELEASE_VALUE;' >> $@
	echo '    vintage_keyword = COM_DIAG_$(SYMBOL)_VINTAGE_KEYWORD;' >> $@
	echo '    vintage_value = COM_DIAG_$(SYMBOL)_VINTAGE_VALUE;' >> $@
	echo '    revision_keyword = COM_DIAG_$(SYMBOL)_REVISION_KEYWORD;' >> $@
	echo '    revision_value = COM_DIAG_$(SYMBOL)_REVISION_VALUE;' >> $@
	echo '    platform_keyword = COM_DIAG_$(SYMBOL)_PLATFORM_KEYWORD;' >> $@
	echo '    platform_value = COM_DIAG_$(SYMBOL)_PLATFORM_VALUE;' >> $@
	echo '    assert(release_keyword != (const char *)0);' >> $@
	echo '    assert(release_value != (const char *)0);' >> $@
	echo '    assert(vintage_keyword != (const char *)0);' >> $@
	echo '    assert(vintage_value != (const char *)0);' >> $@
	echo '    assert(revision_keyword != (const char *)0);' >> $@
	echo '    assert(revision_value != (const char *)0);' >> $@
	echo '    assert(platform_keyword != (const char *)0);' >> $@
	echo '    assert(platform_value != (const char *)0);' >> $@
	echo '    return 0;' >> $@
	echo '}' >> $@

endif

# For embedding in an application where it can be interrogated or displayed.
$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_release.h:	Makefile
	D=`dirname $@`; mkdir -p $$D
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_RELEASE_GENERATED_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_RELEASE_GENERATED_' >> $@
	echo '#define COM_DIAG_$(SYMBOL)_RELEASE_MAJOR $(MAJOR)' >> $@
	echo '#define COM_DIAG_$(SYMBOL)_RELEASE_MINOR $(MINOR)' >> $@
	echo '#define COM_DIAG_$(SYMBOL)_RELEASE_BUILD $(BUILD)' >> $@
	echo '#define COM_DIAG_$(SYMBOL)_RELEASE "$(MAJOR).$(MINOR).$(BUILD)"' >> $@
	echo '#endif' >> $@

# For embedding in the library archive and shared object.
$(OUT)/$(OBC_DIR)/$(SRC_DIR)/$(PROJECT)_release.o:	$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_release.h

# For embedding in an application where it can be interrogated or displayed.
$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_vintage.h:	Makefile
	D=`dirname $@`; mkdir -p $$D
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_VINTAGE_GENERATED_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_VINTAGE_GENERATED_' >> $@
	echo '#define COM_DIAG_$(SYMBOL)_VINTAGE "$(VINTAGE)"' >> $@
	echo '#endif' >> $@

# For embedding in the library archive and shared object.
$(OUT)/$(OBC_DIR)/$(SRC_DIR)/$(PROJECT)_vintage.o:	$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_vintage.h

# For embedding in an application where it can be interrogated or displayed.
$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_revision.h:	Makefile
	D=`dirname $@`; mkdir -p $$D
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_$(SYMBOL)_REVISION_GENERATED_' >> $@
	echo '#define _H_COM_DIAG_$(SYMBOL)_REVISION_GENERATED_' >> $@
	echo '#define COM_DIAG_$(SYMBOL)_REVISION "$(REVISION)"' >> $@
	echo '#endif' >> $@

# For embedding in the library archive and shared object.
$(OUT)/$(OBC_DIR)/$(SRC_DIR)/$(PROJECT)_revision.o:	$(OUT)/$(INC_DIR)/com/diag/$(PROJECT)/$(PROJECT)_revision.h

# For executing from the command line during testing.
$(OUT)/$(SYM_DIR)/vintage:	$(OUT)/$(GEN_DIR)/vintage.c
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

# For generating a setup script for a bash shell (for example, "bash generate > setup").
# (Because the Android bash doesn't seem to implement BASH_ARGV.)
$(OUT)/$(BIN_DIR)/generate:	Makefile
	D=`dirname $@`; mkdir -p $$D
	echo 'COM_DIAG_$(SYMBOL)_PATH=`dirname $$0`; COM_DIAG_$(SYMBOL)_ROOT=`cd $$COM_DIAG_$(SYMBOL)_PATH; pwd`' > $@
	echo 'echo export PATH=\\$$PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../$(BIN_DIR):$$COM_DIAG_$(SYMBOL)_ROOT/../$(TST_DIR):$$COM_DIAG_$(SYMBOL)_ROOT/../$(FUN_DIR)' >> $@
	echo 'echo export LD_DRIVER_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../$(DRV_DIR)' >> $@
	echo 'echo export LD_LIBRARY_PATH=\\$$LD_LIBRARY_PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../$(LIB_DIR)' >> $@
	echo 'echo export LD_MODULE_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../$(MOD_DIR)' >> $@
	chmod 755 $@

# For generating a setup script for a bash shell (for example, "bash generate > setup").
# (Because the Android bash doesn't seem to implement BASH_ARGV.)
$(OUT)/$(SYM_DIR)/generate:	Makefile
	D=`dirname $@`; mkdir -p $$D
	echo 'COM_DIAG_$(SYMBOL)_PATH=`dirname $$0`; COM_DIAG_$(SYMBOL)_ROOT=`cd $$COM_DIAG_$(SYMBOL)_PATH; pwd`' > $@
	echo 'echo export PATH=\\$$PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../$(SYM_DIR):$$COM_DIAG_$(SYMBOL)_ROOT/../$(BIN_DIR):$$COM_DIAG_$(SYMBOL)_ROOT/../$(TST_DIR):$$COM_DIAG_$(SYMBOL)_ROOT/../$(FUN_DIR)' >> $@
	echo 'echo export LD_DRIVER_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../$(DRV_DIR)' >> $@
	echo 'echo export LD_LIBRARY_PATH=\\$$LD_LIBRARY_PATH:$$COM_DIAG_$(SYMBOL)_ROOT/../$(LIB_DIR)' >> $@
	echo 'echo export LD_MODULE_PATH=$$COM_DIAG_$(SYMBOL)_ROOT/../$(MOD_DIR)' >> $@
	chmod 755 $@

# For sourcing into a bash shell (for example, ". setup").
$(OUT)/$(BIN_DIR)/setup:	$(OUT)/$(BIN_DIR)/generate
	$< > $@
	chmod 664 $@

# For sourcing into a bash shell (for example, ". setup").
$(OUT)/$(SYM_DIR)/setup:	$(OUT)/$(SYM_DIR)/generate
	$< > $@
	chmod 664 $@

# For sourcing into a bash shell (for example, ". diminuto").
$(OUT)/$(BIN_DIR)/$(PROJECT):	$(OUT)/$(BIN_DIR)/vintage $(OUT)/$(BIN_DIR)/setup
	. $(OUT)/$(BIN_DIR)/setup; \
	$< > $@; \
	chmod 664 $@

# For sourcing into a bash shell (for example, ". diminuto").
$(OUT)/$(SYM_DIR)/$(PROJECT):	$(OUT)/$(SYM_DIR)/vintage $(OUT)/$(SYM_DIR)/setup
	. $(OUT)/$(SYM_DIR)/setup; \
	$< > $@; \
	chmod 664 $@

########## User-Space Loadable Modules

LDWHOLEARCHIVES := # These archives will be linked into the shared object in their entirety.

$(OUT)/$(MOD_DIR)/%.$(SO):	$(MOD_DIR)/%.c
	D=`dirname $@`; mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -o $@ $< $(MOFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

$(OUT)/$(MOD_DIR)/%.$(SO):	$(MOD_DIR)/%.cpp
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -o $@ $< $(MOXXFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

########## Kernel-Space Loadable Modules

.PHONY:	drivers drivers-clean drivers-make

OBJ_M := $(addsuffix .o,$(basename $(shell cd $(DRV_DIR); ls *.c)))

$(OUT)/$(SYS_DIR)/Makefile:	Makefile
	D=`dirname $@`; mkdir -p $$D
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo "obj-m := $(OBJ_M)" >> $@
	echo "EXTRA_CFLAGS := -iquote $(HERE)/$(INC_DIR) -iquote $(HERE)/$(TST_DIR) -iquote $(HERE)/$(FUN_DIR)" >> $@

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

.PHONY:	makeversion ccversion distroversion libcversion implicit defines iquotes isystems

makeversion:
	echo "$(MAKEVERSION)"

ccversion:
	echo "$(CCVERSION)"

libcversion:
	echo "$(LIBCVERSION)"

distroversion:
	echo "$(DISTROVERSION)"

implicit:
	@$(CC) $(CFLAGS) -dM -E - < /dev/null
	
defines:
	@$(CC) $(CPPFLAGS) $(CFLAGS) -dM -E - < /dev/null

iquotes:
	@$(CC) -x c $(CPPFLAGS) $(CFLAGS) -E -v - < /dev/null 2>&1 | awk 'BEGIN { S=0; } ($$0=="#include \"...\" search starts here:") { S=1; next; } ($$0=="#include <...> search starts here:") { S=0; next; } (S!=0) { print $$1; } { next; }'

isystems:
	@$(CC) -x c $(CPPFLAGS) $(CFLAGS) -E -v - < /dev/null 2>&1 | awk 'BEGIN { S=0; } ($$0=="#include <...> search starts here:") { S=1; next; } ($$0=="End of search list.") { S=0; next; } (S!=0) { print $$1; } { next; }'

helptarget:
	@$(CC) -Q --help=target

compilationstages:
	@$(CC) -v

########## Documentation

# sudo apt-get install doxygen
# sudo apt-get install ghostscript
# sudo apt-get install latex
# sudo apt-get install texlive
# sudo apt-get install tabu
# sudo apt-get install texlive-latex-extra

.PHONY:	documentation manuals readme

DOCCOOKED := $(shell echo $(OUT)/$(DOC_DIR) | sed 's/\//\\\//g')

documentation $(OUT)/$(DOC_DIR)/latex $(OUT)/$(DOC_DIR)/man $(OUT)/$(DOC_DIR)/pdf:
	mkdir -p $(OUT)/$(DOC_DIR)/pdf
	cat doxygen.cf | \
		sed -e "s/\\\$$PROJECT_NUMBER\\\$$/$(MAJOR).$(MINOR).$(BUILD)/" | \
		sed -e "s/\\\$$OUTPUT_DIRECTORY\\\$$/$(DOCCOOKED)/" | \
		cat > $(OUT)/$(DOC_DIR)/doxygen-local.cf
	doxygen $(OUT)/$(DOC_DIR)/doxygen-local.cf

manuals:	$(OUT)/$(DOC_DIR)/latex $(OUT)/$(DOC_DIR)/man $(OUT)/$(DOC_DIR)/pdf
	$(MAKE) -C $(OUT)/$(DOC_DIR)/latex refman.pdf || exit 0
	cp $(OUT)/$(DOC_DIR)/latex/refman.pdf $(OUT)/$(DOC_DIR)/pdf
	cat $(OUT)/$(DOC_DIR)/man/man3/*.3 | groff -man -Tps - > $(OUT)/$(DOC_DIR)/pdf/manpages.ps
	ps2pdf $(OUT)/$(DOC_DIR)/pdf/manpages.ps $(OUT)/$(DOC_DIR)/pdf/manpages.pdf

$(OUT)/$(DOC_DIR)/html/README.html:	../README.md
	mkdir -p $(OUT)/$(DOC_DIR)/html
	grip ../README.md --export $(OUT)/$(DOC_DIR)/html/README.html

readme:	$(OUT)/$(DOC_DIR)/html/README.html

########## Diffs and Patches

.PHONY:	patch

patch:	$(OLD) $(NEW)
	diff -purN $(OLD) $(NEW)

########## Rules

$(OUT)/$(OBX_DIR)/%.txt:	%.cpp $(TARGETSYNTHESIZED)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) -E $(CPPFLAGS) -c $< > $@

$(OUT)/$(OBX_DIR)/%.o:	%.cpp $(TARGETSYNTHESIZED)
	D=`dirname $@`; mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OUT)/$(OBC_DIR)/%.txt:	%.c $(TARGETSYNTHESIZED)
	D=`dirname $@`; mkdir -p $$D
	$(CC) -E $(CPPFLAGS) -c $< > $@

$(OUT)/$(OBC_DIR)/%.o:	%.c $(TARGETSYNTHESIZED)
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

depend:	$(TARGETSYNTHESIZED)
	M=`dirname $(DEPENDENCIES)`; mkdir -p $$M
	cp /dev/null $(DEPENDENCIES)
	for S in $(BIN_DIR) $(MOD_DIR) $(SRC_DIR) $(TST_DIR) $(FUN_DIR); do \
		if [ -d $$S ]; then \
			for F in $$S/*.c; do \
				D=`dirname $$F`; \
				T=`mktemp "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
				echo -n "$(OUT)/$(OBC_DIR)/$$D/" > $$T; \
				$(CC) $(CPPFLAGS) -MM -MG $$F >> $$T && cat $$T >> $(DEPENDENCIES); \
				rm -f $$T; \
			done; \
		fi; \
	done
	for S in $(DRV_DIR); do \
		if [ -d $$S ]; then \
			for F in $$S/*.c; do \
				D=`dirname $$F`; \
				T=`mktemp "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
				echo -n "$(OUT)/$$D/" > $$T; \
				$(CC) $(CPPFLAGS) -DMODULE -MM -MG $$F >> $$T && cat $$T >> $(DEPENDENCIES); \
				rm -f $$T; \
			done; \
		fi; \
	done; \
	if ls $(SRC_DIR)/*.cpp $(TST_DIR)/*.cpp 2> /dev/null; then \
		for S in $(SRC_DIR) $(TST_DIR); do \
			if [ -d $$S ]; then \
				for F in $$S/*.cpp; do \
					D=`dirname $$F`; \
					T=`mktemp "$(TEMP_DIR)/$(PROJECT).XXXXXXXXXX"`; \
					echo -n "$(OUT)/$(OBX_DIR)/$$D/" > $$T; \
					$(CXX) $(CPPFLAGS) -MM -MG $$F >> $$T && cat $$T >> $(DEPENDENCIES); \
					rm -f $$T; \
				done; \
			fi; \
		done; \
	fi

-include $(DEPENDENCIES)

########## Installation

.PHONY:	install install-bin install-lib install-include

INSTALL_DIR := /usr/local
INSTALL_BIN := $(INSTALL_DIR)/bin
INSTALL_LIB := $(INSTALL_DIR)/lib
INSTALL_INC := $(INSTALL_DIR)/include

install:	install-bin install-lib install-include

install-bin:
	mkdir -p $(INSTALL_BIN)
	for B in $(OUT)/$(BIN_DIR)/*; do \
		install -v $$B $(INSTALL_BIN); \
	done

install-lib:
	mkdir -p $(INSTALL_LIB)
	for F in $(OUT)/$(LIB_DIR)/*.so; do \
		O=`basename $$F`; \
		cp $(OUT)/$(LIB_DIR)/$$O.$(MAJOR).$(MINOR) $(INSTALL_LIB); \
		( cd $(INSTALL_LIB); ln -s -f $$O.$(MAJOR).$(MINOR) $$O.$(MAJOR) ); \
		( cd $(INSTALL_LIB); ln -s -f $$O.$(MAJOR) $$O ); \
	done
	ldconfig -v $(INSTALL_LIB)

install-include:
	mkdir -p $(INSTALL_INC)
	tar -C $(OUT)/$(INC_DIR) -cvf - . | tar -C $(INSTALL_INC) -xvf -
	tar -C $(INC_DIR) -cvf - . | tar -C $(INSTALL_INC) -xvf -

########## Suites

.PHONY: criticalsection cxxcapi extended geologic gnuish logging mostest nohup privileged renameat2 sanity signals sockets sourcing threads timers unittest vintage

vintage:	$(OUT)/$(SYM_DIR)/vintage
	vintage

sourcing:	vintage
	unittest-sourcing

criticalsection:	sourcing
	unittest-criticalsection

unittest:	criticalsection
	unittest-unittest

sanity:	unittest
	unittest-absolute
	unittest-arraytype
	unittest-assert && false || true
	unittest-barrier
	unittest-bitfield
	unittest-bits
	unittest-buffer
	unittest-bufferpool
	unittest-coherentsection
	unittest-command
	unittest-containerof
	unittest-coreable && false || true
	unittest-countof
	unittest-cue
	make -C cxx
	unittest-daemon
	unittest-datum
	unittest-dump
	unittest-endianess
	unittest-escape
	unittest-fatal && false || true
	unittest-fd
	unittest-fibonacci
	unittest-fletcher
	unittest-frequency
	unittest-fs
	unittest-guards
	unittest-hamming
	unittest-heap
	unittest-interrupter
	unittest-ipc
	unittest-ipc4
	unittest-ipc6
	unittest-ipc-endpoint
	unittest-ipcl
	unittest-list
	unittest-lock
	unittest-log
	unittest-macros
	unittest-memory
	unittest-minmaxof
	unittest-mutex
	unittest-mux4 -q
	unittest-number
	unittest-observation && rm /tmp/diminuto-unittest-observation-*
	unittest-offsetof
	unittest-path
	unittest-phex
	unittest-pin
	unittest-pipe | sleep 5
	unittest-pool
	unittest-proxy
	unittest-reaper
	unittest-renameat2
	unittest-ring
	unittest-scattergather4
	unittest-scattergather6
	unittest-serializedsection
	unittest-service
	unittest-shaper
	unittest-sizeof
	unittest-stacktrace
	unittest-store
	unittest-string
	unittest-system
	unittest-terminator
	unittest-testify
	unittest-thread
	unittest-throttle
	unittest-timer
	unittest-time-zone
	unittest-tree
	unittest-types
	unittest-uninterruptible
	unittest-well
	unittest-widthof

mostest:	most
	unittest-module

privileged:
	unittest-ping4
	unittest-map

privileged-ipv6:
	unittest-ping6 # Will only work with IPv6 routing.

extended:
	unittest-alarm
	unittest-buffer-perf
	unittest-controller
	unittest-ipc-ancillary
	unittest-modulator
	unittest-mux4
	unittest-mux6
	unittest-muxl
	unittest-mux-eventloop
	unittest-oneshot1
	unittest-oneshot2
	unittest-oneshot3
	unittest-periodic1
	unittest-periodic2
	unittest-periodic3
	unittest-poll4
	unittest-poll6
	unittest-polll
	unittest-poll-eventloop
	unittest-readerwriter
	unittest-time
	unittest-time-cost

geologic:
	unittest-epoch

nohup:
	unittest-hangup-wan
	unittest-hangup-too
	unittest-hangup-tree
	unittest-hangup-fore

signals:
	unittest-alarm
	echo "nohup make nohup" 1>&2
	unittest-interrupter
	unittest-pipe | cat > /dev/null & sleep 1 & pkill -x -n cat
	unittest-reaper
	unittest-terminator

threads:
	unittest-mutex
	unittest-condition
	unittest-thread
	unittest-coherentsection
	unittest-criticalsection
	unittest-ipc-ancillary
	unittest-readerwriter
	unittest-serializedsection

gnuish:
	make nohup
	unittest-lock
	make most
	unittest-module
	unittest-mutex
	unittest-poll4
	unittest-poll6
	unittest-polll
	unittest-poll-eventloop
	unittest-renameat2
	unittest-thread

logging:
	logging
	unittest-log
	unittest-log daemon
	unittest-log syslog
	unittest-log stderr
	nohup unittest-log stderr < /dev/null > /dev/null 2> /dev/null
	log -N DBUG -I -d "Testing debug."
	log -N INFO -I -i "Testing information."
	log -N NOTE -I -n "Testing notice."
	log -N WARN -I -w "Testing warning."
	log -N EROR -I -e "Testing error."
	log -N CRIT -I -c "Testing critical."
	log -N ALRT -I -a "Testing alert."
	log -N EMER -I -E "Testing Emergency."
	log -N DBUG -I -S -d "Testing debug."
	log -N INFO -I -S -i "Testing information."
	log -N NOTE -I -S -n "Testing notice."
	log -N WARN -I -S -w "Testing warning."
	log -N EROR -I -S -e "Testing error."
	log -N CRIT -I -S -c "Testing critical."
	log -N ALRT -I -S -a "Testing alert."
	log -N EMER -I -S -E "Testing Emergency."
	nohup log -N DBUG -I -s -d "Testing debug."       < /dev/null > /dev/null 2> /dev/null
	nohup log -N INFO -I -s -i "Testing information." < /dev/null > /dev/null 2> /dev/null
	nohup log -N NOTE -I -s -n "Testing notice."      < /dev/null > /dev/null 2> /dev/null
	nohup log -N WARN -I -s -w "Testing warning."     < /dev/null > /dev/null 2> /dev/null
	nohup log -N EROR -I -s -e "Testing error."       < /dev/null > /dev/null 2> /dev/null
	nohup log -N CRIT -I -s -c "Testing critical."    < /dev/null > /dev/null 2> /dev/null
	nohup log -N ALRT -I -s -a "Testing alert."       < /dev/null > /dev/null 2> /dev/null
	nohup log -N EMER -I -s -E "Testing Emergency."   < /dev/null > /dev/null 2> /dev/null
	echo "Testing debug."       | log -N DBUG -d
	echo "Testing information." | log -N INFO -i
	echo "Testing notice."      | log -N NOTE -n
	echo "Testing warning."     | log -N WARN -w
	echo "Testing error."       | log -N EROR -e
	echo "Testing critical."    | log -N CRIT -c
	echo "Testing alert."       | log -N ALRT -a
	echo "Testing emergency."   | log -N EMER -E
	echo "Testing debug."       | log -N DBUG -d -B 256
	echo "Testing information." | log -N INFO -i -B 256
	echo "Testing notice."      | log -N NOTE -n -B 256
	echo "Testing warning."     | log -N WARN -w -B 256
	echo "Testing error."       | log -N EROR -e -B 256
	echo "Testing critical."    | log -N CRIT -c -B 256
	echo "Testing alert."       | log -N ALRT -a -B 256
	echo "Testing emergency."   | log -N EMER -E -B 256
	echo "Testing debug."       | log -N DBUG -d -U

timers:
	unittest-frequency
	unittest-timer
	unittest-oneshot1
	unittest-oneshot2
	unittest-oneshot3
	unittest-oneshot4
	unittest-periodic1
	unittest-periodic2
	unittest-periodic3
	unittest-periodic4
	unittest-alarm
	unittest-uninterruptible
	unittest-modulator

sockets:
	unittest-fd
	unittest-ipc
	unittest-ipc4
	unittest-ipc6
	unittest-ipc-ancillary
	unittest-ipcl
	unittest-ipc-scattergather
	unittest-mux4
	unittest-mux6
	unittest-mux-eventloop
	unittest-muxl
	unittest-poll4
	unittest-poll6
	unittest-poll-eventloop
	unittest-polll
	unittest-scattergather4
	unittest-scattergather6

renameat2:
	unittest-renameat2
	unittest-lock
	renametooltest && echo "SUCCESS." || echo "FAILED!"

cxxcapi:
	make -C cxx
