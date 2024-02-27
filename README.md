com-diag-diminuto
=================

A Linux/GNU systems programming library in C.

# Copyright

Copyright 2008-2024 by the Digital Aggregates Corporation, Colorado, USA.

Except where noted, this software is an original work of its author.

# License

Licensed under the terms in LICENSE.txt. 

# Trademarks

"Digital Aggregates Corporation" is a registered trademark of the Digital
Aggregates Corporation, Arvada, Colorado, USA.

"Chip Overclock" is a registered trademark of John Sloan, Arvada, Colorado,
USA.

# Contact

Chip Overclock  
Digital Aggregates Corporation  
3440 Youngfield Street, Suite 209  
Wheat Ridge CO 80033 USA  
<http://www.diag.com>    
<mailto:coverclock@diag.com>    

# Abstract

Diminuto ("tiny") is an open source library of systems programming
functions and tools written in C to simplify the writing of device
drivers, daemons, and systems applications for the Linux operating
system with the GNU C library and compilers. It has found its way
into a number of commercial products of the Corporation's clients, so
don't be surprised if you find this code being used legitimately elsewhere
in your travels. Diminuto includes a suite of unit tests which are used
to validate features when they change, and a suite of functional tests
some of which require a purpose-built hardware test fixture used to validate
features that depend on hardware facilities like GPIO.

Here are just a few examples of the capabilities, or "features", that
Diminuto provides, in no particular order:

* smart logging that automatically determines whether to write to standard error or to the system log, and to control log levels by extracting the log mask from the environment or from a file (```diminuto_log```);
* simple unit test framework (```diminuto_unittest```) and programmatic enabling of core dumps (```diminuto_core```);
* POSIX mutual exclusion semaphores (```diminuto_mutex```), condition variables (```diminuto_condition```), and threads (```diminuto_thread```);
* POSIX timers (```diminuto_timer```);
* socket interprocess communication (IPC) using IPv4 (```diminuto_ipc4```), IPv6 (```diminuto_ipc6```), or UNIX domain (local) paths (```diminuto_ipcl```), using streams, datagrams, or (for local sockets) packets (messages).
* doubly-linked lists (```diminuto_list```), red-black trees (```diminuto_tree```), and keyword-value stores using red-black trees (```diminuto_store```);
* traffic shaping (```diminuto_shaper```) using the Generic Cell Rate Algorithm (GCRA) (```diminuto_throttle```);
* monitoring applications that run on a headless host (```diminuto_observation```);
* programmatic management of lock files (```diminuto_lock```);
* expanding and collapsing C-style escape sequences in strings (```diminuto_escape```) and generating expanded output in real-time (```diminuto_phex```);
* displaying hexadecimal dumps of data structures to the console (```diminuto_dump```);
* display stack traces to the console (```diminuto_stacktrace```);
* daemonizing applications (```diminuto_daemon```);
* helper functions for the dynamic run-time linker for user-space loadable modules (```diminuto_modules```);
* using General Purpose Input/Output (GPIO) pins via the standard /sys interface (```diminuto_pins```) and debouncing digital inputs (```diminuto_cue```);
* Time keeping (```diminuto_time```) and delays (```diminuto_delay```) with a consistent unit of tick;
* A fair first-come-first-served readers-writers synchronization solution (```diminuto_readerwriter```);
* Proportional, Integral, Derivative (PID) controller (```diminuto_controller```) in user space;
* Pulse Width Modulation (PWM) (```diminuto_modulator```) in user space.
* programmatic ping for IPv4 (```diminuto_ping4```) and IPv6 (```diminuto_ping6```) when run as root;
* vector I/O and IPC using scatter/gather (```diminuto_scattergather```);
* serial stream framing using HDLC-like byte stuffing (```diminuto_framer```);
* macros to create thread-safe critical sections (```diminuto_criticalsection```) or sections uninterruptible by selected signals (```diminuto_uninterruptiblesection```);
* macros to compute array dimensions (```diminuto_countof```), address of an object from a pointer to a field (```diminuto_containerof```), and minimum and maximum of any integral type (```diminuto_minmaxof```);
* macros to support typed object constructors and destructors and to implement the operations (```diminuto_new``` and ```diminuto_delete```) that call them;
* support for integrating C into a C++ code base (```diminuto_cxxcapi``` pronounced "sexy API");
* an extensive collection of unit tests and functional tests that provide working examples of using the library features;
* an architecture and design that simplifies using Diminuto in an existing code base, and for using multiple Digital Aggregates repositories in the same project.

Diminuto also provides many command-line utilities using the features
of the library that are especially useful in shell scripts.

See the Features section below for a more complete list.

# Pro Tip

When you link against the library or use any of the binaries or scripts
that are artifacts of the build process, the linker and the shell have
to know where to find those artifacts. Furthermore, some of the binaries
or scripts may depend upon values in your environment to work correctly.

You need to set the ```LANG``` (Language) environmental variable to
set your locale to use the U.S. version of UTF-8. This allows applications
to correctly display Unicode symbols like the degree symbol and the
plus/minus symbol. If you use the ```bash``` shell (as I do), you can put
the following line in your ```.profile``` in your home directory so that
it is set everytime you log in (as I do). Or you can just set it when you
need to.

    export LANG=en_US.UTF-8

If you don't install libraries, binaries, and scripts in one of the usual
system locations like ```/usr/local/lib``` and ```/usr/local/bin```
(I typically don't), you can temporarily modify your environment
so that the linker and your shell can find them. This bash sourcing
script is an artifact of the build process and sets the ```PATH``` and
```LD_LIBRARY_PATH``` environmental variables and exports them.

    . ~/src/com-diag-diminuto/Diminuto/out/host/bin/setup

The libraries, binaries, and scripts make use of the Diminuto logging
system. The importance of log messages is classified into eight severity
levels, ranging from DEBUG (log mask 0x01, which may emit a firehose of
information) to EMERGENCY (log mask 0x80, in which case your system is
probably in deep trouble). You can control which level of messages are
emitted, either to standard error (if your application has a controlling
terminal), or to the system log (if your application, like a daemon,
does not). One way to control this is to set the log mask in your environment.

    export COM_DIAG_DIMINUTO_LOG_MASK=0xfe

The log mask value is an eight-bit number in decimal, hexadecimal, or
even octal. In addition, the string ```~0``` can be used to enable all
log levels, equivalent to ```255```, ```0xff```, or ```0377```. (Generally
I find ```0xfe``` to be a good starting point.)

# Disclaimer

Diminuto isn't intended to be portable. It is specifically designed
to support the kind of systems programming efforts in the Linux/GNU
environment that I am routinely called upon to do. It depends on
specific features of the Linux kernel, the GNU libraries, and even
the GNU compiler. In addition, it assumes a ```char``` is one octet so
that ```sizeof()``` returns a ```size_t``` in octets, and that integer
operations are performed using two's complement arithmetic; none of
these is a given in C. It also assumes that character encoding is
ASCII (applications may use other encodings, such as Unicode, as long
as they keep it to themselves).

The more complex Diminuto features, like those involving POSIX threads,
implement a specific model or approach to using the underlying C library.
The model expresses a design I have found useful in my own applications.
It is not intended to be a one size fits all implementation.

For some projects over the years, I have ported Diminuto (as it existed
at that time) to uClibc (a stripped down C library used by Buildroot),
Bionic (a C library and run-time used by Google's Android), Cygwin (a
UNIX-like run-time environment that runs on Windows), and Darwin (the
run-time of Apple's MacOS). You may find artifacts of that work in
existing code, but I have made no effort to maintain it.

There are several other repositories of C or C++ code that I wrote that are
built on top of Diminuto. Diminuto doesn't depend on any of them, but they
depend on it. In addition, there are several of my projects in which I just
cut and pasted a small amount of code, a function here and there, rather
than create a dependency for the entire library. Finally, over the years,
bits and pieces of Diminuto have found themselves incorporated into a
number of commercial products to which I have contributed, such as:
satellite communications systems, cellular base station routers,
in-flight entertainment systems for business aircraft, and special purpose
Android platforms.

# Platforms

This code was tested against one or more of the following hardware
targets and software platforms. These were generated using the Diminuto
platform script.  (Diminuto has been built and used in the dim past and
for specific projects on a much wider variety of targets and platforms.)

Intel(R) Core(TM) i7-7567U CPU @ 3.50GHz     
x86_64 x4     
Ubuntu 20.04.4 LTS (Focal Fossa)     
Linux 5.15.0-41-generic     
gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0     
ldd (Ubuntu GLIBC 2.31-0ubuntu9.9) 2.31     
GNU ld (GNU Binutils for Ubuntu) 2.34     
GNU Make 4.2.1     
x86_64-linux-gnu-gcc-9     
x86_64-linux-gnu     
little-endian    

Raspberry Pi 4 Model B Rev 1.4 BCM2835 d03114     
aarch64 x4     
Ubuntu 22.04 LTS (Jammy Jellyfish)     
Linux 5.15.0-1011-raspi     
gcc (Ubuntu 11.2.0-19ubuntu1) 11.2.0     
ldd (Ubuntu GLIBC 2.35-0ubuntu3) 2.35     
GNU ld (GNU Binutils for Ubuntu) 2.38     
GNU Make 4.3     
aarch64-linux-gnu-gcc-11     
aarch64-linux-gnu     
little-endian    

Raspberry Pi 4 Model B Rev 1.1 BCM2835 c03111     
aarch64 x4     
Debian GNU/Linux 11 (bullseye)     
Linux 5.15.32-v8+     
gcc (Debian 10.2.1-6) 10.2.1 20210110     
ldd (Debian GLIBC 2.31-13+rpt2+rpi1+deb11u2) 2.31     
GNU ld (GNU Binutils for Debian) 2.35.2     
GNU Make 4.3     
aarch64-linux-gnu-gcc-10     
aarch64-linux-gnu     
little-endian    

sifive,u74-mc rv64imafdc sv39     
riscv64 x2     
Fedora 33 (Rawhide)     
Linux 5.15.10+     
gcc (GCC) 10.3.1 20210422 (Red Hat 10.3.1-1)     
ldd (GNU libc) 2.32     
GNU ld version 2.35-18.fc33     
GNU Make 4.3     
ccache     
riscv64-redhat-linux     
little-endian    

sifive,u74-mc rv64imafdc sv39    
riscv64 x2    
Ubuntu 22.04.1 LTS (Jammy Jellyfish)    
Linux 5.17.0-1005-starfive    
gcc (Ubuntu 11.2.0-19ubuntu1) 11.2.0    
ldd (Ubuntu GLIBC 2.35-0ubuntu3.1) 2.35    
GNU ld (GNU Binutils for Ubuntu) 2.38    
GNU Make 4.3    
riscv64-linux-gnu-gcc-11    
riscv64-linux-gnu    
little-endian    

sifive,u74-mc rv64imafdc sv39    
riscv64 x4    
Debian GNU/Linux    
Linux 5.15.0-starfive    
gcc (Debian 11.3.0-3) 11.3.0    
ldd (Debian GLIBC 2.33-7) 2.33    
GNU ld (GNU Binutils for Debian) 2.38.50.20220615    
GNU Make 4.3    
riscv64-linux-gnu-gcc-11    
riscv64-linux-gnu    
little-endian    

OrangePi 5    
aarch64 x8    
Ubuntu 22.04.2 LTS (Jammy Jellyfish)    
Linux 5.10.110-rockchip-rk3588    
gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0    
ldd (Ubuntu GLIBC 2.35-0ubuntu3.1) 2.35    
GNU ld (GNU Binutils for Ubuntu) 2.38    
GNU Make 4.3    
aarch64-linux-gnu-gcc-11    
aarch64-linux-gnu    
little-endian    

Raspberry Pi Zero W Rev 1.1 ARMv6-compatible processor rev 7 (v6l) BCM2835 9000c1    
armv6l x1    
Raspbian GNU/Linux 11 (bullseye)    
Linux 6.1.21+    
gcc (Raspbian 10.2.1-6+rpi1) 10.2.1 20210110    
ldd (Debian GLIBC 2.31-13+rpt2+rpi1+deb11u5) 2.31    
GNU ld (GNU Binutils for Raspbian) 2.35.2    
GNU Make 4.3    
arm-linux-gnueabihf-gcc-10    
arm-linux-gnueabihf    
little-endian    

Raspberry Pi 5 Model B Rev 1.0 d04170    
aarch64 x4    
Debian GNU/Linux 12 (bookworm)    
Linux 6.1.0-rpi8-rpi-2712    
gcc (Debian 12.2.0-14) 12.2.0    
ldd (Debian GLIBC 2.36-9+rpt2+deb12u4) 2.36    
GNU ld (GNU Binutils for Debian) 2.40    
GNU Make 4.3    
aarch64-linux-gnu-gcc-12    
aarch64-linux-gnu    
little-endian    

# Features

Given a feature I call Foo (or foo),
* its public API will be defined in
```inc/com/diag/diminuto/diminuto_foo.h```;
* its private (internal) API, if one exists, will be defined in
```src/diminuto_foo.h```;
* its implemention (if it's not purely in the header file) will be in
```src/diminuto_foo.c``` (very occasionally there will be
multiple implementation files, with minor variations on this name);
* and its unit test will be in
```tst/unittest-foo.c``` (there may be more than one, with
minor variations on this name).

The long qualified path name for the header file directory facilitates
combining multiple Digital Aggregates projects into one application,
e.g.  ```inc/com/diag/diminuto```, ```inc/com/diag/codex```, and
```inc/com/diag/hazer```, with no danger of header file ambiguity even
if the file name doesn't have a prefix unique to the project. (My C++
projects, like Grandote, don't follow this prefix convention, and
instead depend upon C++ namespaces to avoid symbol conflicts.)
C/C++ include statements will look like this

    #include "com/diag/diminuto/diminuto_foo.h"
    #include "com/diag/codex/codex_foo.h"
    #include "com/diag/placer/placer_foo.h"
    #include "com/diag/assay/assay_foo.h"
    #include "com/diag/hazer/hazer_foo.h"
    #include "com/diag/grandote/Foo.h"
    #include "com/diag/lariat/Foo.h"

in the event that different projects each have a feature named Foo.

The prefix ```diminuto_``` on the name of each header file,
as in ```diminuto_foo.h```, along with the qualified path name,
```com/diag/diminuto/```, may seem redundant. But it facilitates the
integration of the library into existing code bases in those cases where
the developers choose instead to use the full Diminuto include path at
the command-line level, ```-Iinc/com/diag/diminuto``` (for their
own code), in addition to ```-Iinc``` (for Diminuto's own header files),
in their own build system.

Preprocessor symbols that are not strictly speaking part of the public API being
defined may have qualified names like ```COM_DIAG_DIMINUTO_TIMER_FREQUENCY```
to prevent symbol conflicts.

Similarly, environmental variables have qualified names like
```COM_DIAG_DIMINUTO_LOG_MASK``` to prevent name conflicts in the environment.

## Basics

* assert - similar to assert(3), plus expect, contract, and panic.
* containerof - macro to compute address of structure from address of a field.
* cxxcapi - helpers to allow C code to call C++ functions with type safety.
* macros - insane macros to implement conditional and recursive code generation.
* minmaxof - macros to compute minimum and maximum of integral types.
* offsetof - macro for determining field offset in structures.
* token - macros for preprocessor token handling.
* types - common library types.
* typeof - macro to infer the type of a variable in a typedef manner.
* unittest - simple unit test framework.
* version - provides printable strings for Diminuto version, revision, vintage.
* widthof - macro to return width in bits of a type.

## Data Structures

* bits - get, set, and clear bits in a bit field array.
* list - circular doubly linked list implementation.
* ring - generic support for ring (circular) buffers.
* store - uses the tree feature to implement an in-memory key/value store.
* tree - red/black balanced binary tree implementation.

## Signals

* alarm - receive and check for a SIGALRM signal.
* hangup - receive and check for a SIGHUP signal.
* interrupter - receive and check for a SIGINT signal.
* pipe - receive and check for a SIGPIPE signal.
* reaper - receive and check for a SIGCHLD signal and reap child processes.
* terminator - receive and check for a SIGTERM signal.
* uninterruptiblesection - macros to implement scoped blocked signals.

## Memory

* barrier - acquire and release memory barriers.
* buffer - used with heap feature to replace malloc/free with buffer pool.
* buffer_pool - buffer pool used with buffer feature.
* coherentsection - macros to implement scoped acquire/release memory barriers.
* heap - dependency injection mechanism for malloc/free alternatives.
* new - new, delete, ctor, dtor macros for memory allocation and freeing.
* pool - manage a pool of objects from which they can be allocated and freed.
* well - like the pool feature but maintains requested alignment of objects.

## Utilities

* command - reconstruct the command line from argc and argv.
* comparator - prototype of comparator function for search feartures.
* debug - simple debug tools to help with Work In Progress (WIP).
* dump - a variety of formatted memory dump functions.
* endianess - determine processor endianess.
* escape - collapse and expand escape sequences in strings.
* fletcher - implement the Fletcher-16 checksum.
* hamming - implement the Hamming(8,4) code.
* kermit - implement the Kermit-16 a.k.a. CRC-CCITT cyclic redundancy check.
* log - generic logging mechanism for applications, daemons, and kernel modules.
* lowpassfilter - implements a trivial low pass filter macro.
* number - alternative interfaces to strtol(3) and strtoul(3).
* phex - emit printable and unprintable characters in a printable form.
* proxy - macros to implement a proxy feature like heap.
* string - some useful string manipulation stuff.
* testify - common functions for interpreting yes/no questions.

## Sockets

* endpoint - parse a string into an IP address and port or a UNIX domain path.
* fd - general handling of file descriptors (including sockets).
* ipc4 - IPv4 socket interface.
* ipc6 - IPv6 socket interface (works for IPv4 endpoints too).
* ipcl - UNIX domain ("local") socket interface.
* mux - file descriptor multiplexer using select(2).
* ping4 - programmatic ICMP ECHO REQUEST.
* ping6 - programmatic ICMP6 ECHO REQUEST.
* poll - file descriptor multiplexer using poll(2).
* scattergather - support for vector I/O and IPC.

## Time

* delay - time delay and execution yield.
* frequency - determine common library time frequency.
* time - comprehensive time of day functions plus timestamp formatting.
* timer - periodic and one-shot timers.

## Processes

* core - enable core dumps.
* daemon - process deamonizer.
* lock - lock, unlock, and check for lock files.

## Threads

* condition - error checking wrapper around pthread condition.
* criticalsection - macros to implement scoped pthread mutex serialization.
* environment - global mutex to serialize access to the shell environment.
* mutex - error checking wrapper around pthread mutex.
* readerwriter - a FIFO lock that starves neither readers nor writers.
* thread - error checking wrapper around pthread thread.
* serializedsection - macros to implement scoped spinlocked code blocks.

## File System

* fs - file system walking framework.
* observation - atomically instantiate data files from temporary files.
* path - find a file from a list in a PATH-like environmental variable.
* renameat2 - atomically rename a file or swap files in the same file system.

## Real-Time

* meter - measures a stream of events for peak and sustainable rates.
* shaper - use the throttle feature to implement a bursty traffic shaper.
* throttle - implement a traffic shaper for a traffic contract.

## Hardware

* barrier - macros and inlines to implement memory barriers using GNU built-ins.
* coherentsection - macros to implement scoped code blocks with acquire/release memory barriers.
* controller - proportional/integral/derivative (PID) controller.
* cue - logic level debouncer and edge detector.
* framer - HDLC-like framing of binary serial streams using byte stuffing.
* i2c - simplified thread-safe API around Linux I2C API.
* pin - get and set GPIO pins using the sysfs interface.
* modulator - software pulse width modulation (PWM) generator.
* serial - get and set parameters for a serial port.

## System

* datum - memory mapped register operators for device drivers.
* map - memory mapping in user or kernel state.
* memory - query system for memory architecture.
* mmdriver - kernel module implementing memory mapped I/O.
* module - support for dynamically linked user space modules.
* platform - try to determine what kind of system or target is being used.
* stacktrace - print a stack trace.

# Bash Sourcing Files

* diminuto - defines and exports shell variables like Arch, Release, Revision, and Vintage into the environment.
* setup - defines and exports shell variables like PATH and LD_LIBRARY_PATH into the environment.

# Command Line Utilities
 
* alerts - emits zero or more BEL characters with a delay in between.
* bakepi - monitors Raspberry Pi core temperature which throttles at 82C.
* bucketbrigade - read from a serial port and forward to another serial port.
* clocks - logs the current values of UTC time, atomic time, and elapsed time.
* collapse - collapse escapes in arguments and write results to standard output.
* coreable - enable core dumps.
* datesink - consume verbose timestamp stream.
* datesource - produce verbose timestamp stream.
* dec - display an argument number in decimal.
* dhhmmss - calculate D/HH:MM:SS from elapsed seconds.
* dump - display a file in a formatted hexidecimal dump.
* elapsedsleep - sleep for monotonic duration to specified resolution.
* elapsedtime - display monotonic elapsed time to specified resolution.
* endianess - display the little or big endianess of the target.
* epochseconds - display seconds since POSIX epoch for provided data and time.
* epochtime - display POSIX epoch to higher resolution than just seconds.
* endpoint - convert an endpoint name into an IP address and port number.
* framertool - test the Framer feature across, for example, a serial connection.
* frequency - display Diminuto base tick frequency in hertz.
* gpstime - converts a timestamp into GPS WNO, GPS TOW, and UTC offset.
* headsup - sends a SIGHUP (hangup signal) to a running observe script.
* hex - display an argument number in hexadecimal.
* internettool - test internet connectivity using the Diminuto IPC features.
* ipcalc - calculate IPv4 addresses and masks.
* iso8601 - converts seconds since the UNIX epoch into an ISO8601 timestamp.
* juliantime - converts a timestamp into a year, julian day, and seconds.
* juliet - display the local time in ISO 8601 format.
* log - log from command line and/or stdin using Diminuto log functions.
* logging - display the configuration and state of the log feature.
* loopback - provide a configurable serial port loopback.
* memtool - manipulate memory mapped registers directly (requires root).
* mmdrivertool - manipulate memory mapped registers using a device driver.
* observe - watch for an observation file and indicate when it arrives.
* oct - display an argument number in octal.
* phex - display standard input in a printable form.
* pinchange - execute a command when a GPIO pin changes state.
* pintool - manipulate GPIO pins.
* platform - generate the platform description used in the README.
* posixtime - converts a timestamp into seconds since the POSIX epoch.
* pps - uses Diminuto pintool to multiplex on a 1PPS GPIO pin.
* renametool - atomically rename or swap files in the same file system.
* serialtool - manipulate serial ports.
* shaper - shape traffic in a pipeline.
* sizeof - display sizeof of different data types.
* spartntime - converts a timestamp into a SPARTNKEY WNO and TOW.
* timesink - consume concise timestamp stream.
* timesource - produce concise timestamp stream.
* usecsleep - sleep for a specified number of microseconds.
* usectime - display the elapsed monotonic time in microseconds.
* wipe - aggressively wipe a storage device.
* zulu - display the UTC time in ISO 8601 format.

# Functional Tests

* adccontroller - PWM and ADC PID loop (Pin, Modulator, I2C, Controller).    
* adcrheostat - PWM and ADC rheostat (Pin, Modulator, I2C).
* dcdtest - DCD support on serial port (Serial).
* internettooltests - use internettool in many configurations (IPC4, IPC6).
* lbktest - loopback on serial port (Serial).
* luxcontroller - LED and lux sensor PID loop (Pin, Modulator, I2C, Controller).
* luxrheostat - LED and lux sensor rheostat (Pin, Modulator, I2C).
* pinchange - multiplexing of GPIO pins (Pin).
* pincleanup - clean up pins exported to hardware test fixture (Pin).
* pintest - setting and getting GPIO pins (Pin).
* timestuff - tries out the underlying glibc time functions.
* walker - walk the file system tree starting at a specified root (FS).

(Some of these functional tests depend upon hardware test fixtures I
fabricated specifically for this purpose. Depending on the platform,
some or all of these functional tests  may need to be run as root,
or as a user - e.g. pi - that is in the gpio group.)

# Unit Test Make Targets

There are many make targets to run different combinations of unit
tests. Here are the ones I use the most. Because some individual unit
tests appear in multiple make targets, as part of different unit test
suites, some unit tests may be run more than once. This is a feature, not
a bug.

* extended-test - these unit tests take a lunch break or more to run.
* geologic-test - these unit test take the better part of a day or more to run.
* gnuish-test - these unit tests are for features that require GNU extensions.
* logging-test - these unit tests exercise the logging feature.
* longer-test - these unit tests take a coffee break to run.
* privileged-test - these unit tests require "sudo make privileged" to run.
* reassuring-test - a long test suite to reassure me that things mostly work.
* sanity-test - these unit tests take just a few minutes to run.
* signals-test - these unit tests exercise the signal handlers.
* sockets-test - these unit tests test the ipc4, ipc6, ipcl, mux, and poll features.
* tagging-test - a very long test suite done before tagging a new release.
* threads-test - these unit tests exercise the POSIX thread-based features.
* timers-test - these unit tests exercise the timer features.

Important safety tip: on some (most?) platforms, if you run a unit test
in the background, perhaps using the nohup command, having redirected
its stdout and stderr to files for examination later, and then logoff,
the Diminuto log feature will detect this in real-time and redirect
the log output from that point forward to the system log. This will
look mysterious later when you log back in to check the progress of
the unit test and find your output file(s) strangely truncated. This
is a feature, not a bug. Also note, on systemd platforms like Fedora,
the system log is saved in a database format, not in a flat text file;
see ```man journalctl``` on how to access it.

# Directories
 
* bin - utility source files.
* cfg - configuration makefiles.
* drv - kernel space loadable module source files.
* etc - miscellaneous occasionally useful files.
* fun - functional test source files (may require special hardware).
* inc - public header files.
* log - captures of some unit and functional test utility output.
* mod - user space dynamically linkable module source files.
* out - build artifacts.
* sav - saved stuff that is not part of the build.
* src - feature implementation and private header source files.
* tst - unit test source files.
* txt - notes and other useful stuff.

# Artifacts

* out/$(TARGET)/app - application binary executables.
* out/$(TARGET)/arc - object file archives for static linking.
* out/$(TARGET)/bin - utility stripped binary executables and scripts.
* out/$(TARGET)/dep - make dependencies.
* out/$(TARGET)/ext - executables of extra files if you choose to compile them.
* out/$(TARGET)/fun - functional test binary executables and scripts.
* out/$(TARGET)/gen - generated source files.
* out/$(TARGET)/inc - include (header) files.
* out/$(TARGET)/lib - shared objects for dynamic linking.
* out/$(TARGET)/log - log files produced at run-time.
* out/$(TARGET)/mod - dynamically loadable application modules.
* out/$(TARGET)/obc - object files.
* out/$(TARGET)/sym - utility unstripped binary executables.
* out/$(TARGET)/tmp - temporary files supporting headless operation.
* out/$(TARGET)/tst - unit test binary executables and scripts.

# Dependencies

Most of my Intel, ARM, and RISC-V development systems are Debian-based, so the
command line examples I show below use the ```apt```  package manager. A
few, like one of my RISC-V development systems, are Fedora-based, so on those
I use the ```dnf``` package manager.

## Libraries

The Diminuto Makefile references the following libraries. They may
not be installed on all platforms. (Although they are part of the
versions of Ubuntu and Raspbian that I've been using).

    -lpthread   # POSIX Threads
    -lrt        # POSIX Real-Time
    -ldl        # Dynamic Linker
    -lm         # Math

## IPv6

Diminuto's ipc6 feature requires IPv6 support. IPv6 is not enabled by
default on all platforms (in particular the Raspberry Pi). Under
Raspbian 10, I added a line "ipv6" to /etc/modules and rebooted.
The command ```modprobe ipv6``` also worked albeit non-persistently.

    sudo modprobe ipv6
    sudo su
    echo "ipv6" >> /etc/modules

On the StarFive VisionFive 2 SBC, IPv6 isn't even built as a module;
I had to build a new kernel.

## SCTP

Stream Control Transmission Protocol (SCTP) is a bit of an outlier in the
pantheon of internet protocols build on top of IP. While I would expect
all internet routers to support TCP and UDP, I wouldn't expect
any to support SCTP, a protocol built for Signaling System 7 (SS7), a
protocol stack used in the Public Switched Telephone Network (PSTN).
SCTP combines the fixed boundry size of UDP with the guaranteed delivery
of TCP, making it ideal for many messaging applications.

While SCTP can't be relied upon in the public internet, I find it useful
when used with Unix domain (Local) sockets, and its use is supported by
Diminuto. The protocol is enabled in the Linux kernel by default on the
Raspberry Pi. But when I rebuilt the Vision 2 kernel to enable IPv6,
I also had to enable SCTP, which, like IPv6, was not enabled by default.

If it was built as a module, these commands might work.

    sudo modprobe sctp
    sudo su
    echo "sctp" >> /etc/modules

## Tool Chain

On some distros I had to install gcc, g++, and make.

    sudo apt-get install gcc
    sudo apt-get install g++
    sudo apt-get install make

## Documentation

If you want to make documentation, doxygen and related tools will
need to be installed.

    sudo apt-get install doxygen
    sudo apt-get install texlive-base
    sudo apt-get install texlive-fonts-recommended
    sudo apt-get install texlive-fonts-extra
    sudo apt-get install texlive-science
    sudo apt-get install grip

## Workflow

For my own workflow, I installed the following tools.

    sudo apt-get install cscope
    sudo apt-get install dnsutils
    sudo apt-get install git
    sudo apt-get install net-tools
    sudo apt-get install openssh-server
    sudo apt-get install screen
    sudo apt-get install socat
    sudo apt-get install vim

## valgrind

    sudo apt-get install valgrind

I found valgrind not to work on my ARM Raspbian (a.k.a. Raspberry Pi OS)
platforms, so my valgrind testing has been on Ubuntu on either x86_64
platforms, or the ARM64 version that runs on the Raspberry Pi.
On those platforms, I found the following command especially useful. There
are some special unit test targets in the Makefile that uses this form of
the command.

    valgrind --leak-check=full --show-leak-kinds=all <COMMAND...>

## cscope

If I have to build cscope from source, as sometimes happens, I also need the
ncurses library.

    sudo apt-get install libncurses5-dev libncursesw5-dev

Warning: I have found that cscope 15.9 on Ubuntu core dumps with a double
free exception under some circumstances on both Intel and Arm platforms.

## inotifywait

The observe script requires the inotify tools. Not all distros install
these by default (e.g. Raspbian).

    sudo apt-get install inotify-tools

## Scripts

I have a lot of bash scripts that I find useful, but which are not
Diminuto-specific. These can be found in the separate repository
<https://github.com/coverclock/com-diag-bin>. These scripts do not
require that you build, install, and use the Diminuto library or
its command line utilities.

# Branching

Diminuto is big and complex enough that I sometimes move to a "master"
(or "main") and "develop" dual branch model of development. I make and
test major changes in the "develop" branch, and when I think I have a
stable release, I merge "develop" into the "master" ("main") branch. I
still make what I consider to be minor changes in the master branch.

# Versioning

After a lot of testing (typically having run the tagging-test unit test
suite), I tag major releases with a three-number tuple that is defined
in the build Makefile for each project. Release numbers, e.g. 22.2.1,
consist of a major number (22), a minor number (2), and a build number
(1).

The major number changes when I've made a change significant enough that
I think applications using the library will likely need to be changed,
even though they may compile.

The minor number changes when I've added new features or functionality,
but existing features and functionality haven't changed.

The build number changes when I've fixed a bug so that existing
features or functionality works as described (even though this may break
workarounds in applications).

The major and minor numbers are incorporated into the name of the shared
object (dynamic link library) produced by the build.

The vintage is the date and time of the most recent build in UTC and expressed
in ISO8601 format.

The revision is the Git commit number.

The release, revision, vintage, and a bunch of other stuff, are embedded as
modules in the Diminuto library. Those modules can be linked into
an application. The project has a binary executable named vintage that
displays this information.

# Building

Clone, build, and sanity test Diminuto. The use of the src directory
is just my own convention.

    cd ~
    mkdir -p src
    cd src
    git clone https://github.com/coverclock/com-diag-diminuto
    cd com-diag-diminuto/Diminuto
    make scratch
    . out/host/bin/setup

The ```scratch``` target is a combination of ```pristine``` which recursively
removes the output directory,```depend``` which generates the header file
dependencies file used by the build, and ```all``` which builds everything
that needs to be built. These targets can be use separately as well.

## Build Parameters

The build process builds some shell scripts. On Debian-based Linux distos
like Ubuntu and Raspbian, escaping special characters in the Makefile
requires two successive backslashes. But typically (and mysteriously)
on Fedora-based distros, only one successive backslash is necessary,
and two will cause an error. Since I mostly develop under Debian-based
distros, two backslashes is the default. But one backslash can be selected
by specifying the ```BACKSLASHES``` parameter on the command line.

    make scratch BACKSLASHES=1

For more complex parameterization needs, the parameter ```TARGET``` can be set
to select a specific Make configuration file in the directory ```cfg```. This
file will be included early in the Makefile during the build process. The default
value for the parameter is ```host```, which includes the default configuration file
```cfg/host.mk```. This technique can be used, for example, when cross-compiling
on a build host architecture different from that of the target architecture.

    make scratch TARGET=mytarget

## Special Targets

There are some make targets that are useful when you have dig into the
code in a lot more detail.

If you want to look at the preprocessor output without further compiling it,
you can use a ```.i``` target to the output directory (replacing ```host```
if necessary). This invokes the C compiler with the ```-E``` option.

    make out/host/obc/tst/unittest-barrier.i

If you want to look at the assembler output without further assembling it,
you can use a ```.s``` target to the output directory (replacing ```host```
if necessary). This invokes the C compiler with the ```-S``` option.

    make out/host/obc/tst/unittest-barrier.s

The ```.l``` target causes the assembler to produce a listing that includes
useful stuff like the machine code and offsets.

    make out/host/obc/tst/unittest-barrier.l

## Unit Testing

Run the basic sanity tests that complete in just a few minutes.

    make sanity-test

Run more extended tests that may take a lunch break to complete.

    make extended-test

Run long term tests that may take overnight to complete.

    make geologic-test

## Installing

Optionally install Diminuto headers, library, and utilities in ```/usr/local```
subdirectories. You can override the make variable ```INSTALL_DIR``` to change
```/usr/local``` to something else (like ```/opt``` or whatnot).

    sudo make install

I typically never bother with this for development and instead let the
```out/host/bin/setup``` bash source script set my environmental variables
like ```PATH``` and ```LD_LIBRARY_PATH```. Your mileage may vary. This script is
generated by the build process.

    . out/host/bin/setup

You can change ```host``` above to some other name if you are
cross-compiling for a different target. 

# Logging

The Diminuto features, unit tests, and functional tests make heavy use of
the Diminuto Log feature. Eight different log levels (see below) can be
selectively enabled or disabled using a process global log mask. Updates
to the log mask are protected by a thread mutex.

Most Diminuto utilities and tests using the Log feature
extract the log mask from the environment using the function
```diminuto_log_setmask()```. The default name of the environmental
variable is ```COM_DIAG_DIMINUTO_LOG_MASK```. The log mask can also be
imported from a text file (which can be done dynamically at run-time)
using the function ```diminuto_log_importmask()```. The default path
of the file is ```${HOME}/.com_diag_diminuto_log_mask```. For either
mechanism, the mask value is coded as an octal, decimal, or hexadecimal
numeric string in C-style format, e.g.  ```0377```, ```255```, or ```0xff```. As
a special case, the value ```~0``` generates a mask that enables all
log levels. A good example of setting and importing a log mask can be
found in the ```stagecoach``` application found in the Codex repository.

The log mask bits, as defined in the feature's header file, are these,
patterned after the syslog(3) log levels.

    DIMINUTO_LOG_MASK_EMERGENCY     = (1 << (7 - 0))
    DIMINUTO_LOG_MASK_ALERT         = (1 << (7 - 1))
    DIMINUTO_LOG_MASK_CRITICAL      = (1 << (7 - 2))
    DIMINUTO_LOG_MASK_ERROR         = (1 << (7 - 3))
    DIMINUTO_LOG_MASK_WARNING       = (1 << (7 - 4))
    DIMINUTO_LOG_MASK_NOTICE        = (1 << (7 - 5))
    DIMINUTO_LOG_MASK_INFORMATION   = (1 << (7 - 6))
    DIMINUTO_LOG_MASK_DEBUG         = (1 << (7 - 7))

The log levels enabled by default are NOTICE, WARNING, ERROR, CRITICAL,
ALERT, and EMERGENCY, equivalent to the log mask environmental variable
below.

    export COM_DIAG_DIMINUTO_LOG_MASK=0xfc

A good starting point (and what I use myself for development) is to
enable all log levels other than DEBUG.

    export COM_DIAG_DIMINUTO_LOG_MASK=0xfe

To see more detail, also enable DEBUG, although this can produce a fire
hose of output.

    export COM_DIAG_DIMINUTO_LOG_MASK=0xff

For every log output line, the Diminuto Log features determines whether
the issuing process is being run interactively or not, or is a daemon,
and directs the output appropriately to either standard error or to the
system log. The system log on various verisons of Linux may be stored
in the file ```/var/log/syslog``` (earlier versions of Debian) or it may
be accessed using the ```journalctl -f``` system command (Fedora or later
versions of Debian that use ```systemd```).

# Documentation

All of the Diminuto code has embedded within it Doxygen comments. You can
build hundreds of pages of man pages, HTML-based documentation, and PDF
reference manuals for Diminuto by installing the doxygen and LaTeX tools
(see under Dependencies) and running the following make targets.

    make documentation
    make readme
    make manuals

The documentation will be in the directories

    out/${TARGET}/doc/html
    out/${TARGET}/doc/man
    out/${TARGET}/doc/pdf

whose exact path depends upon for what target you are building. The
value of the TARGET variable will typically be "host" unless you are
cross-compiling for an embedded target (an ability I confess I haven't
tested in years).

You can view the HTML, and probably the PDF, using your web browser.

The easiest approach to using the documentation is to find the feature
or utility you think you might want to know about in the list above,
then look it up in the generated HTML, man, or PDF documentation.
(The public APIs for the Diminuto library are documented in their
header or .h files. Their source or .c files may have some documentation
regarding any private API they might have.)

These PDFs of the
[manual pages](https://www.dropbox.com/s/sjkg31fti54lirj/manpages.pdf?dl=0)
and associated
[reference manual](https://www.dropbox.com/s/ss8s2fz7ki52ht7/refman.pdf?dl=0)
were built from Diminuto's embedded Doxygen comments on
2023-05-24
using tag
```100.1.0```
. They will not reflect changes made since then.

The unit tests (```Diminuto/tst```),  functional tests
(```Diminuto/fun```), and command line utilities (```Diminuto/bin```)
have a wealth of non-trivial examples of how to use the public APIs.
The private APIs mostly exist to expose internal details so that they can
be unit tested.

# Remarks

Diminuto started out many years ago as a project to build a minimal
Linux-based embedded system for an ARMv4 processor that seems laughably
small now. It consisted of a stripped down 2.4 (later, 2.6) Linux
kernel with just the stuff I needed, uClibc, Busybox, and an application.
The application was built around a small C-based library I developed that
contained functions to support the kinds of systems work that I am
typically called upon to do. In the fullness of time the library became
more important than the project as some or all of it found its way into
several products I was helping my clients develop and ship.

Today there are several reasons Diminuto continues to be a useful resource
that I expand and maintain.

I got tired of writing the same code over and over again, even though I
got paid by the hour to do so. Sometimes even for the same client. Some
of the work I do is under a subcontract, and my clients' clients often
explicitly didn't want code whose development they pay for to be shared
with other clients; but they were completely okay using open source code.
I wanted to write that useful but generic code once, test it thoroughly,
make it open source and easily available.

I needed a way I could get my arms and head around the evolving low level
POSIX and Linux APIs where I spent a lot of time. I'm a very hands-on
person, and can only really learn by doing.

I wanted to develop a simpler, inter-operable, reasonably consistent
API of my own on top of those low level APIs. This was partly because
I was interested in exploring API design; because I wanted to reduce
the development time needed to work in C and C++ (although I routinely
work in Python, Bash, and Java, and have been known to hack JavaScript,
when duty calls); and because I wanted perhaps to return to teaching this
stuff in the future but didn't want to start at the low level to do so.

If you happen upon this repo and find it useful, that's great. If you
don't, that's great too.

# Examples

If you'd like to see a non-trivial application that makes use
of many Diminuto features in exactly the way I envisioned them being
used, check out the following.

* The Diminuto IPC Ancillary unit test uses a lot of Diminuto features
and is a good example of a non-trivial application that is multi-process
and multi-threaded. The source code also has a useful implementation of Thread
Pools. It also illustrates how to pass open file descriptors between processes.

<https://coverclock.blogspot.com/2020/12/old-dog-new-tricks.html>    
<https://github.com/coverclock/com-diag-diminuto/blob/master/Diminuto/tst/unittest-ipc-ancillary.c>    

* The Diminuto IPC Scatter/Gather unit test similarly uses a lot of
Diminuto features and is also multi-process. It has a useful
implementation of Diminuto List pools, and support for Records, Segments, and
I/O vectors, used to implement scatter/gather I/O. (It was eventually
mainstreamed into the Diminuto library as the Scatter/Gather feature.)

<https://coverclock.blogspot.com/2020/12/scattergather.html>    
<https://github.com/coverclock/com-diag-diminuto/blob/master/Diminuto/tst/unittest-ipc-scattergather.c>    

* The Diminuto Reader/Writer feature and its unit test is a reasonably complex
example of using Diminuto features with POSIX thread capabilities for mutual
exclusion and synchronization.

<https://coverclock.blogspot.com/2020/12/first-come-first-served-readers-and.html>    
<https://coverclock.blogspot.com/2022/02/revisiting-first-come-first-served.html>    
<https://github.com/coverclock/com-diag-diminuto/blob/master/Diminuto/src/diminuto_readerwriter.c>    
<https://github.com/coverclock/com-diag-diminuto/blob/master/Diminuto/tst/unittest-readerwriter.c>    

* The Diminuto internettool command line utility and its functional test script
internettooltests uses the Diminuto IPC feature in just about every way
possible: IPv4 or IPv6, and TCP or UDP or even ICMP. For best results, enable
DEBUG logging in the environment before running. internettool must be run as
root to use the ICMP capability. The ICMP options of internettool require the
tool be run with root privileges; the internettooltests functional test script
will test the ICMP options automatically when the script is run as root.

<https://github.com/coverclock/com-diag-diminuto/blob/master/Diminuto/bin/internettool.c>    
<https://github.com/coverclock/com-diag-diminuto/blob/master/Diminuto/fun/internettooltests.sh>    

* The gpstool application in the Hazer project (repo URL below) is the
Swiss Army knife of Hazer, a library that parses output from GPS devices,
and relies on Diminuto for much of its infrastructure.

<https://coverclock.blogspot.com/2017/02/better-never-than-late.html>    
<https://github.com/coverclock/com-diag-hazer/tree/master/Hazer/app/gpstool>    

* The stagecoach application in the Codex project (repo URL below),
a library that uses Open Secure Socket Layer (OpenSSL), also relies on
Diminuto for much of its infrastructure.

<https://github.com/coverclock/com-diag-codex/tree/master/Codex/app/stagecoach>    

* The survey and census functional tests in the Placer project,
which uses the SQLite relational database system, use the Diminuto
FS file system walker and are a good example of how a need in another
project leads to a virtuous cycle of organic growth in Diminuto.

<https://coverclock.blogspot.com/2020/03/placer-x-macros-sqlite-and-schema.html>    
<https://github.com/coverclock/com-diag-placer/blob/master/Placer/fun/survey.c>    
<https://github.com/coverclock/com-diag-placer/blob/master/Placer/fun/census.c>    

# Issues

## Feature Deprecations

The Pin feature, which makes it easy to manipulate General Purpose Input/Output
(GPIO) pins, uses the deprecated sysfs ABI. I'm looking at the new ABIs. While
the new ABIs offer a higher level interface, my initial impression is that
they don't easily support the kinds of weird GPIO doings with special devices
that I am often called upon to do.

## Build Warnings

I try hard to resolve all warnings, even in the Doxygen comments.
However, Diminuto may intentionally generate some itself.

On some platforms (e.g. Raspbian), the renameat2(2) system call exists
in Linux, but there is no glibc support for it in GNU. In this case,
Diminuto generates its own API to renameat2(2), but warns you that this
is the case at compile time.

    src/diminuto_renameat2.c:40:5: warning: #warning renameat2(2) not available on this platform so using SYS_renameat2 instead! [-Wcpp]

If you define the C preprocessor symbol ```COM_DIAG_DIMINUTO_DEPRECATED```,
deprecated code segments (that is, code that you can expect to be removed
in the near future) will be defined and built. Compiling these segments
will generate a warning.

    inc/com/diag/diminuto/diminuto_error.h:56:5: warning: #warning This code is deprecated! [-Wcpp]

## Unit Test Failures

### Ephemeral Ports

Some of the socket unit tests make use of ephemeral ports that are
allocated and discarded quickly. Some careful collection and editing
of the log messages from the tst/unittest-ipc-ancillary.c unit test
shows that the entire range of ephemeral ports [32768..60999] is
used, more than once, by the test; this indicates that not only is
the entire range available, but the ports are being recycled as the
sockets are closed.

Sometimes (rarely, on my x86_64 development machine) the test fails
with the error message

    diminuto_ipc4_source: bind: "Address already in use" (98)

which in this case really means that there are no emphemeral ports
left to assign. The speed of recycling ports is limited by the
round-trip latency between the connected hosts (even when it is the
same host on both ends) when the socket is closed, and it is easy
for the unit test to get ahead of that. I address this by putting
some delay between the receiving a request and sending a response
in the server thread, simulating some workload. (Diminuto does
support the REUSE PORT socket option, but this isn't quite it's
intended purpose.)

### Endpoints

I have had unittest-ipc-endpoint fail because I had misconfigured the
DNS resolver on an Ubuntu-based test system to search "diag.com", and
my web server configured to respond go "http.diag.com" in addition to
"www.diag.com". This caused ```diminuto_ipc_endpoint()``` to resolve "http"
(without the colon) to the IP address of my web server instead of the
HTTP port number (80), because it searched for "http.diag.com" instead
of "http".

It is true that the use of a service name without the leading colon
can be ambiguous, but this is intentional on my part in the sense
that in this case the feature is "working as designed".

When I tried to fix the configuration of the resolver on the test
system using resolvectl, I discovered a bug in resolvectl in
that the command

     resolvectl domain eth0 ""

did not work as documented on the man page: it did nothing, instead
of removing "diag.com" as a search domain. Furthermore, using the
command

     resolvectl domain eth0 "invalid"

appeared to override the old configuration (at the expense of
configuring a non-working search domain), but this configuration
change did not persist across reboots.

I initially fixed this by removing the soft link to
```/etc/resolv.conf``` and hand coding ```/etc/resolv.conf```,
removing the line "search diag.com". This was not the preferred fix.

Eventually (almost by accident) I re-discovered the configuration
file I had added, a YAML file in ```/etc/netplan```, and removed the
"search" line from it. After rebooting, this permanently (and
correctly) fixed the problem. I also restored the original soft
link from ```/etc/resolv.conf``` to ```/run/systemd/resolve/stub-resolv.conf```.

## Build Running Out Of Memory

On a recent Raspberry Pi I have run out of memory compiling the unit
tests. I admit I was completely taken by surprise by this. This was on
a Raspberry Pi 4, running Raspbian 10 "buster" (Linux 5.4.51, GNU 8.3.0),
compiling ```tst/unitttest-ipc-ancillary.c```.

    cc1: out of memory allocating 65536 bytes after a total of 1487814656 bytes

I believe the regression arose when I added code to make the Unit Test
framework thread safe so that I could test multi-threaded features. The
cpp (C PreProcessor) stage works just fine, but the C compiler chokes
on compiling the preprocessed translation unit.

Turning optimizations off with "-O0" didn't help. Nor did increasing
the swap space. Rasbian (at this time) is still a 32-bit kernel, which
limits a process' address space to a maximum of 2GB of virutal memory.
(The script etc/rpiswap.sh constitutes my notes on how to increase the
swap space on the RPi.)

To get around this I refactored the Unit Test framework to move the
critical sections out of the .h file and into functions in the .c file,
with some loss of useful info that had been logged by the framework.
This had the effect of making all unit tests syntactically simpler, with
fewer nested macro invocations (although just running the C preprocessor
cpp on the original code base in question worked just fine). So far
this seems to have worked, but I have to believe it's going to show up
again in the future.

I have added the make target

    make notests

to build everything except the unit tests, and

    make onlytests

to just build the unit tests, just in case this happens again in the future,
to make this easier to troubleshoot.

## IPv6

There are a variety of unit tests that exercise features specific to
IPv6. Typically these have "6" (instead of "4") in their names. Some
platforms (e.g. Raspbian a.k.a. Raspberry Pi OS) do not come with IPv6
enabled by default. I mention this above in the "Dependencies" section.

# Repository

<https://github.com/coverclock/com-diag-diminuto>

# Related

<https://github.com/coverclock/com-diag-assay>

<https://github.com/coverclock/com-diag-codex>

<https://github.com/coverclock/com-diag-hazer>

<https://github.com/coverclock/com-diag-obelisk>

<https://github.com/coverclock/com-diag-placer>

# Images

<https://www.flickr.com/photos/johnlsloan/albums/72157680352449986>

# References

P. Albertos, I. Mareels, "Feedback and Control for Everyone", Springer, 2010

A. Arcangeli et al., Linux kernel, include/linux/rbtree.h and lib/rbtree.c

A. Arcangeli et al., U-Boot, include/linux/rbtree.h and lib/rbtree.c

K. Astrom, T. Hagglund, "PID Controllers: Theory, Design, and Tuning", 2nd ed., Instrumentation, Systems and Automation Society, 1995-01-01

ATM Forum, Traffic Management Specification Version 4.1, af-tm-0121.000, 1999-03

B. Beauregard, "Improving the Beginner's PID", Project Blog, 2011-04

T. Berners-Lee, R. Fielding, L. Masinter, "Uniform Resource Identifier (URI) Syntax", RFC 3986, 2005-01

P. Bieringer, LINUX IPv6 HOWTO, 2015-10-16

R. Braden, D. Borman, C. Partridge, "Computing the Internet Checksum", RFC 1071, 1988-09

M. Cline et al., "C++ FAQs", 2nd edition, Addision-Wessley, 1999, pp. 538, "FAQ 36.05 How can an object of a C++ class be passed to or from a C function?"

M. Cline, "C++ FAQ Lite", 2001-08-15, 29.8

T. Cormen et al., Introduction to Algorithms, MIT Press, 1992, pp. 263-280

P. Courtois, F. Heymans, D. Parnas, "Concurrent Control with ''Readers'' and ''Writers''", CACM, 14.10, 1971-10

E. Davies, S. Krishnan, P. Sovola, "IPv6 Transition/Coexistence Security Considerations", RFC 4942, 2007-09 

J. Fletcher, "An Arithmetic Checksum for Serial Transmissions", IEEE Transactions on Communication, COM-30, No. 1, pp. 247-252, January 1982

Free Software Foundation et al., Standard Template Library, include/bits/stl_tree.h

J. Ganssle, "A Guide to Debouncing - Part 2, or How To Debounce a Contact in Two Easy Pages", 2008-06, <http://www.ganssle.com/debouncing-pt2.htm>

J. Ganssle, "A Guide to Debouncing, or How To Debounce a Contact in Two Easy Pages", 2008-06, <http://www.ganssle.com/debouncing.htm>

J. Ganssle, "My Favorite Software Debouncers", 2004-06-16, <http://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers>

R. Gilligan, S. Thomson, J. Bound, J. McCann, W. Stevens, "Basic Socket Interface Extentions for IPv6", RFC 3493, 2003-02

GNU, "Built-in functions for atomic memory access", 5.44, <https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html>

R. Graziani, IPV6 FUNDAMENTALS, Cisco Press, 2013

R. Hamming, "Error Detecting and Error Correcting Codes", BELL SYSTEM TECHNICAL JOURNAL, XXIX.2, 1950-04

J. Heathcote, "C Pre-Processor Magic", 2020, <http://jhnet.co.uk/articles/cpp_magic>

C. Hoare, "Monitors: An Operating System Structuring Concept", CACM, 17.10, 1974-10

International Standards Organization, "Data elements and interchange formats - Information interchange -
Representation of dates and times", First edition, ISO8601:1988(E), 1988-06-15

International Standards Organization, "HIGH LEVEL DATA LINK CONTROL PROCEDURES - PART 1: FRAME STRUCTURE",
ISO3309-1984, 1984

V. Jacobson et al., “TCP Extensions for High Performance”, RFC1323, 1992

A. Kalvans, "Using select(2)", 2017-04-06, <http://aivarsk.github.io/2017/04/06/select/>

D. Kegel, "The C10K Problem", 2014-02-05, <http://www.kegel.com/c10k.html>

The Kernel Development Community, "Linux Timers Documentation",
<http://blog.foool.net/wp-content/uploads/linuxdocs/timers.pdf>

G. Klyne, C. Newman, "Date and Time on the Internet: Timestamps", RFC3339, IETF, 2002-07

R. Kroll, "getaddrinfo() on glibc calls getenv(), oh boy",
<https://rachelbythebay.com/w/2023/10/16/env/>, 2023-10-16

K. Kumar, "Linux TCP SO_REUSEPORT - Usage and implementation", 2019-08-19, <https://tech.flipkart.com/linux-tcp-so-reuseport-usage-and-implementation-6bfbf642885a>

L. Lamport, "Time, Clocks, and the Ordering of Events in a Distributed System", CACM, 21.7, 1978-07, pp. 558-565

B. Lampson, D. Redell, "Experience with Processes and Monitors in Mesa", CACM, 23.2, 1980-02

D. Lemire, "In C, how do you know if the dynamic allocation succeeded?",
2021-10-27(?), <https://lemire.me/blog/2021/10/27/in-c-how-do-you-know-if-the-dynamic-allocation-succeeded/>

Linux 4.4.34, Documentation/i2c/muxes/dev-interface

linux/uio.h 5.4.0: UIO_MAXIOV=1024 UIO_FASTIOV=8

J. Mahdavi, “Enabling High Performance Data Transfers on Hosts”, Pittsburgh Supercomputing Center, 1996

Dr. Marty, "The Best Switch Debounce Routine Ever", 2009-05-20, <http://drmarty.blogspot.com/2009/05/best-switch-debounce-routine-ever.html>

U. Naseer et al., "Zero Downtime Release: Disruption-free Load Balancing of a Multi-Billion User Website", ACM SIGCOMM '20,

NXP, "I2C-bus specification and user manual", UM10204, Rev.6, NXP Semiconductor N.V., 2014-04-04

Open Group, pthread_cond_broadcast, pthead_cond_signal, Open Group Base Specification Issue 7, 2018 edition, IEEE Std. 1003.1-2017, 2018

Open Group, pthread_cond_timedwait, pthead_cond_wait, Open Group Base Specification Issue 7, 2018 edition, IEEE Std. 1003.1-2017, 2018

V. Popov, O. Mazonka, "Faster Fair Solution for the Reader-Writer Problem", 2013

POSIX 1003.1-2001 (readv, writev)

POSIX 1003.1g 5.4.1.1: "UIO_MAXIOV shall be at least 16"

J. Postel, "Internet Control Message Protocol", RFC 792, September 1981

R. Quattlebaum, "Framing Protocol", 2018-09-26,
<https://chromium.googlesource.com/external/github.com/openthread/openthread/+/refs/tags/upstream/thread-reference-20180926/doc/spinel-protocol-src/spinel-framing.md>

W. Sewell, "Ephemeral port exhaustion and how to avoid it", 2018-05-22, <https://making.pusher.com/ephemeral-port-exhaustion-and-how-to-avoid-it/>

W. Simpson, ed., "PPP in HDLC-Like Framing', RFC 1662, 1994-07

J. Sloan, "Parcels with TAGS", NCAR TECHNICAL NOTE, NCAR/TN-377+IA, National Center for Atmospheric Research, 1992-10, section 4, "Storage Management", p. 10, <http://www.diag.com/ftp/NCAR_Sloan_Parcels.pdf>

C. Sridharan, "File Descriptor Transfer over Unix Domain Sockets", CopyConstruct, 2020-08

W. Stevens, "TCP/IP Illustrated Volume 1: The Protocols", Addison-Wesley, 1994

W. Stevens, M. Thomas, E. Nordmark, T. Jinmei, "Advanced Sockets Applications Program Interface (API) for IPv6", RFC 3542, 2003-05

O. Li, T. Jimmel, K. Shima, IPV6 CORE PROTOCOLS IMPLEMENTATION, Morgan Kaufmann, 2007

S. Tardieu, "The third readers-writers problem", rfc1149.net, 2011-11-07

Single UNIX Specification version 4 (POSIX:2008), Issue 7 (sendmsg, recvmsg)

M. Vaner, "40 millisecond bug", 2020-11-06, <https://vorner.github.io/2020/11/06/40-ms-bug.html>

J. Walker, "Red Black Trees", <http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx>

V. Welch, “A User’s Guide to TCP Windows”, NCSA, 1996

T. Wescott, "PID without a PhD", Embedded Systems Programming, 2000-10-01

Wind River Systems, "VxWorks Reference Manual", 5.4

G. Wright et al., "TCP/IP Illustrated Volume 2: The Implementation", Addison-Wesley, 1995

J. Zweig, C. Partridge, "TCP Alternate Checksum Options", RFC 1146, IETF, February 1990, <https://tools.ietf.org/html/rfc1146>

man page, cmsg(3)

man page, pipe(7)

man page, readv(2)

man page, recvmsg(2)

man page, sendmsg(2)

man page, socket(7)

man page, unix(7)

man page, writev(2)

Wikibooks, "C Programming/Preprocessor directives and macros", <https://en.wikibooks.org/wiki/C_Programming/Preprocessor_directives_and_macros#X-Macros>

Wikipedia, "Circular buffer", 2020-12-10

Wikipedia, "Fibonacci Number", <http://en.wikipedia.org/wiki/Fibonacci_number>

Wikipedia, "Fletcher's checksum", 2016-12-21, <https://en.wikipedia.org/wiki/Fletcher's_checksum>

Wikipedia, "High-Level Data Link Control", 2022-09-22, <https://en.wikipedia.org/wiki/High-Level_Data_Link_Control>

Wikipedia, "PID controller", 2018-06-27

Wikipedia, "Readers-writer lock", 2020-11-16

Wikipedia, "Readers-writers problem", 2020-11-23

Wikipedia, "Red-black tree", <http://en.wikipedia.org/wiki/Red�black_tree>

Wikipedia, "X-Macro", <https://en.wikipedia.org/wiki/X_Macro>

# Resources

<https://raspberrypi.stackexchange.com/questions/50240/missing-build-file-when-building-for-rtl8812au-driver>

<https://www.raspberrypi.org/forums/viewtopic.php?t=46472>

<https://xkcd.com/1179/>

<https://RVspace.org>

<https://github.com/starfive-tech>

# Articles

Chip Overclock, "Traffic Management", 2006-12,
<http://coverclock.blogspot.com/2006/12/traffic-management.html>

Chip Overclock, "Traffic Contracts", 2007-01,
<http://coverclock.blogspot.com/2007/01/traffic-contracts.html>

Chip Overclock, "Rate Control Using Throttles", 2007-01,
<http://coverclock.blogspot.com/2007/01/rate-control-and-throttles.html>

Chip Overclock, "In Praise of do while (false)", 2009-12,
<https://coverclock.blogspot.com/2009/12/in-praise-of-do-while-false.html>

Chip Overclock, "Can't We All Just Get Along, C++ and C", 2011-02,
<https://coverclock.blogspot.com/2011/02/cant-we-all-just-get-along-c-and-c.html>

Chip Overclock, "Fibonacci Scaling", 2011-12,
<http://coverclock.blogspot.com/2011/12/fibonacci-scaling_09.html>

Chip Overclock, "Being Evidence-Based Using the sizeof Operator", 2015-03,
<https://coverclock.blogspot.com/2015/03/being-evidence-based-using-sizeof.html>

Chip Overclock, "Buried Treasure", 2017-01,
<https://coverclock.blogspot.com/2017/01/buried-treasure.html>

Chip Overclock, "Some Stuff That Has Worked For Me In C", 2017-04,
<https://coverclock.blogspot.com/2017/04/some-stuff-that-has-worked-for-me-in-c.html>

Chip Overclock, "renameat2(2)", 2018-04,
<https://coverclock.blogspot.com/2018/04/renameat22.html>

Chip Overclock, "When The Silicon Meets The Road", 2018-07,
<https://coverclock.blogspot.com/2018/07/when-silicon-meets-road.html>

Chip Overclock, "When Learning By Doing Goes To Eleven", 2020-03,
<https://coverclock.blogspot.com/2020/03/when-learning-by-doing-goes-to-eleven.html>

Chip Overclock, "Headless", 2020-06,
<https://coverclock.blogspot.com/2020/06/headless.html>

Chip Overclock, "Clock Time", 2020-10,
<https://coverclock.blogspot.com/2020/10/clock-time.html>

Chip Overclock, "Timer Threads", 2020-10,
<https://coverclock.blogspot.com/2020/10/timer-threads.html>

Chip Overclock, "Layers", 2020-10,
<https://coverclock.blogspot.com/2020/10/layers.html>

Chip Overclock, "Is the Virtuous Cycle really that virtuous?", 2020-11,
<https://coverclock.blogspot.com/2020/11/is-virtuous-cycle-really-that-virtuous.html>

Chip Overclock, "Scatter/Gather", 2020-12,
<https://coverclock.blogspot.com/2020/12/scattergather.html>

Chip Overclock, "Old Dog, New Tricks", 2020-12,
<https://coverclock.blogspot.com/2020/12/old-dog-new-tricks.html>

Chip Overclock, "First-Come First-Served Readers and Writers in C using
Condition Variables", 2020-12,
<https://coverclock.blogspot.com/2020/12/first-come-first-served-readers-and.html>

Chip Overclock, "Where the RF Meets the Road", 2021-03,
<https://coverclock.blogspot.com/2021/03/where-rf-meets-road.html>

Chip Overclock, "Revisiting the First-Come First-Served Reader-Writer Solution",
2022-02,
<https://coverclock.blogspot.com/2022/02/revisiting-first-come-first-served.html>

Chip Overclock, "Layers II", 2022-11,
<https://coverclock.blogspot.com/2022/02/layers-ii.html>

# Soundtrack

<https://www.youtube.com/playlist?list=PLd7Yo1333iA--8T8OyYiDnLAMWGIirosP>

# Acknowledgements

I owe a debt of gratitude to my mentors, particularly at Wright State
University (Dayton Ohio), at the National Center for Atmospheric Research
(Boulder Colorado), and at Bell Labs and its various spin-offs, who were
so generous with their time and expertise over the years, and to my various
clients, who both inspired and sometimes even shipped this software.
