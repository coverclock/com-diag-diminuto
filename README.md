com-diag-diminuto
=================

A Linux/GNU systems programming library in C.

# Copyright

Copyright 2008-2020 by the Digital Aggregates Corporation, Colorado, USA.

This software is an original work of its author.

# License

Licensed under the terms in LICENSE.txt. 

# Trademarks

"Digital Aggregates Corporation" is a registered trademark of the Digital
Aggregates Corporation, Arvada, Colorado USA.

"Chip Overclock" is a registered trademark of John Sloan, Arvada, Colorado,
USA.

# Contact

Chip Overclock  
Digital Aggregates Corporation  
3440 Youngfield Street, Suite 209  
Wheat Ridge CO 80033 USA  
<http://www.diag.com>    
<mailto:coverclock@diag.com>    

# Thanks

[![Say Thanks!](https://img.shields.io/badge/Say%20Thanks-!-1EAEDB.svg)](https://saythanks.io/to/coverclock)

# Abstract

Diminuto ("tiny") is an open source library of systems programming
functions and tools written in C to simplify the writing of device
drivers, daemons, and systems applications for the Linux operating
system with the GNU C library and compilers. It has found its way
into a number of commercial products of the Corporation's clients, so
don't be surprised if you find this code being used legitimately elsewhere
in your travels. Diminuto includes a suite of unit tests that are used
to validate features when they change, and a suite of functional tests
that require a purpose-built hardware test fixture used to validate
features that depend on hardware (like GPIO).

Here are just a few examples of the capabilities, or "features", that
Diminuto provides:

* smart logging that automatically determines whether to write to standard error or to the system log (```diminuto_log```);
* simple unit test framework (```diminuto_unittest```) and programmatic enabling of core dumps (```diminuto_core```).
* POSIX mutual exclusion semaphores (```diminuto_mutex```), condition variables (```diminuto_condition```), and threads (```diminuto_thread```);
* POSIX timers (```diminuto_timer```);
* socket interprocess communication (IPC) using IPv4 (```diminuto_ipc4```), IPv6 (```diminuto_ipc6```), or UNIX domain (local) paths (```diminuto_ipcl```), using streams, datagrams, or (for local sockets) packets (messages).
* doubly-linked lists (```diminuto_list```), red-black trees (```diminuto_tree```), and keyword-value stores using trees (```diminuto_store```);
* traffic shaping (```diminuto_shaper```) using the Generic Cell Rate Algorithm (GCRA) (```diminuto_throttle```);
* monitoring applications that run on a headless host (```diminuto_observation```);
* programmatic management of lock files (```diminuto_lock```);
* expanding and collapsing C-style escape sequences in strings (```diminuto_escape```) and generating expanded output in real-time (```diminuto_phex```);
* displaying hexadecimal dumps of data structures to the console (```diminuto_dump```);
* display stack traces to the console (```diminuto_stacktrace```);
* daemonizing applications (```diminuto_daemon```);
* helper functions for the dynamic run-time linker for user-space loadable modules (```diminuto_modules```);
* using General Purpose Input/Output (GPIO) pins via the standard /sys interface (```diminuto_pins```) and debouncing digital inputs (```diminuto_cue```);
* consistent implementation of time keeping (```diminuto_time```) and delays (```diminuto_delay```);
* Proportional, Integral, Derivative (PID) controller (```diminuto_controller```);
* Pulse Width Modulation (PWM) (```diminuto_modulator```);
* programmatic ping for IPv4 (```diminuto_ping4```) and IPv6 (```diminuto_ping6```);
* an extensive collection of unit tests and functional tests that provide working examples of using the library features;
* an architecture and design that simplifies using Diminuto in an existing code base, include support for integrating C into a C++ code base (```diminuto_cxxcapi```).

Diminuto also provides many command-line utilities using the features
of the library that are especially useful in shell scripts.

See the Features section below for a more complete list.

# Details

Diminuto isn't intended to be portable. It is specifically designed to
support the kind of systems programming efforts in the Linux/GNU environment
that I am routinely called upon to do. It depends on specific features of
the Linux kernel, the GNU libraries, and even the GNU compiler.

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
in-flight entertainment systems, and special purpose Android platforms.

# Platforms

This code is typically regression tested against one or more of the
following hardware targets and software platforms. (It has been built
and used for specific projects on a much wider variety of targets
and platforms.)

Intel NUC7i7BNH    
Arch="x86_64"    
Cc="gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"    
Distro="Ubuntu 20.04.1 LTS (focal)"    
Kernel="5.4.0-48-generic"    
Libc="ldd (Ubuntu GLIBC 2.31-0ubuntu9.1) 2.31"    
Make="GNU Make 4.2.1"    

Intel NUC5i7RYH    
Arch="x86_64"    
Cc="gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"    
Distro="Ubuntu 18.04.5 LTS bionic"    
Kernel="4.15.0-122-generic"    
Libc="ldd (Ubuntu GLIBC 2.27-3ubuntu1.2) 2.27"    
Make="GNU Make 4.1"    

Rasbberry Pi 3B+    
Arch="arm"    
Cc="gcc (Raspbian 6.3.0-18+rpi1+deb9u1) 6.3.0 20170516"    
Distro="Raspbian GNU/Linux 9.8 (stretch)"    
Kernel="4.14.98-v7+"    
Libc="ldd (Debian GLIBC 2.24-11+deb9u4) 2.24"    
Make="GNU Make 4.1"    

Raspberry Pi 4B    
Arch="arm"    
Cc="gcc (Raspbian 8.3.0-6+rpi1) 8.3.0"    
Distro="Raspbian GNU/Linux 10 (buster)"    
Kernel="4.19.97-v7l+"    
Libc="ldd (Debian GLIBC 2.28-10+rpi1) 2.28"    
Make="GNU Make 4.2.1"    

Raspberry Pi 3B    
Arch="arm"    
Cc="gcc (Raspbian 4.9.2-10+deb8u2) 4.9.2"    
Distro="Raspbian GNU/Linux 8.0 (jessie)"    
Kernel="4.9.35-v7+"    
Libc="ldd (Debian GLIBC 2.19-18+deb8u10) 2.19"    
Make="GNU Make 4.0"    

# Features

* alarm - receive and check for a SIGALRM signal.
* assert - similar to assert(3) but with more output.
* barrier - acquire and release memory barriers.
* buffer - used with heap feature to replace malloc/free with buffer pool.
* buffer_pool - buffer pool used with buffer feature.
* comparator - prototype of comparator function for search feartures.
* containerof - macro just like in the Linux kernel.
* controller - proportional/integral/derivative (PID) controller.
* core - enable core dumps.
* coherentsection - macros to implement scoped acquire/release memory barriers.
* condition - error checking wrapper around pthread condition.
* countof - macro to compute array positions using sizeof operator.
* criticalsection - macros to implement scoped pthread mutex serialization.
* cue - logic level debouncer and edge detector.
* cxxcapi - helpers to allow C code to call C++ functions with type safety.
* debug - simple debug tools to help with Work In Progress (WIP).
* daemon - process deamonizer.
* datum - memory mapped register operators for device drivers.
* delay - time delay and execution yield.
* dump - a variety of formatted memory dump functions.
* emit - example of how to use the log feature to make your own log output.
* endianess - determine processor endianess.
* endpoint - parse a string containing host:service into an IP address and port, or path/file into a UNIX domain path.
* escape - collapse and expand escape sequences in strings.
* fd - general handling of file descriptors (including sockets).
* frequency - determine common library time frequency.
* fs - file system walking framework.
* hangup - receive and check for a SIGHUP signal.
* heap - dependency injection mechanism for malloc/free alternatives.
* i2c - Simplified thread-safe API around Linux I2C API.
* ipc4 - IPv4 socket interface.
* ipc6 - IPv6 socket interface (works for IPv4 endpoints too).
* ipcl - UNIX domain ("local") socket interface.
* list - doubly linked list implementation.
* lock - lock, unlock, and check for lock files.
* log - generic logging mechanism for applications, daemons, and kernel modules.
* macros - insane macros to implement conditional and recursive code generation.
* map - memory mapping in user or kernel state.
* memory - query system for memory architecture.
* mmdriver - kernel module implementing memory mapped I/O.
* modulator - software pulse width modulator (PWM) generator.
* module - support for dynamically linked user space modules.
* mutex - error checking wrapper around pthread mutex.
* mux - file descriptor multiplexer using select(2).
* number - alternative interfaces to strtol(3) and strtoul(3).
* observation - atomically instantiate data files from temporary files.
* offsetof - macro for determining field offset in structures.
* path - find a file from a list in a PATH-like environmental variable.
* phex - emit printable and unprintable characters in a printable form.
* pin - get and set GPIO pins using the sysfs interface.
* ping4 - programmatic ICMP ECHO REQUEST.
* ping6 - programmatic ICMP6 ECHO REQUEST.
* platform - try to determine what kind of system or target is being used.
* poll - file descriptor multiplexer using poll(2).
* pool - manage a pool of objects from which they can be allocated and freed.
* proxy - macros to implement a proxy feature like heap.
* serial - get and set parameters for a serial port.
* serializedsection - macros to implement scoped spinlocked code blocks.
* shaper - use the throttle feature to implement a bursty traffic shaper.
* stacktrace - print a stack trace.
* store - uses the tree feature to implement an in-memory key/value store.
* string - some useful string manipulation stuff.
* thread - error checking wrapper around pthread thread.
* throttle - implement a traffic shaper for a traffic contract.
* time - comprehensive time of day functions plus timestamp formatting.
* timer - periodic and one-shot timers.
* token - macros for preprocessor token handling.
* tree - red/black balanced binary tree implementation.
* types - common library types.
* uninterruptiblesection - macros to implement scoped blocked signals.
* unittest - simple unit test framework.
* well - like the pool feature but maintains requested alignment of objects.
* widthof - macro to return width in bits of a type.
 
# Utilities
 
* bakepi - monitors Raspberry Pi core temperature which throttles at 82C.
* bucketbrigade - read from a serial port and forward to another serial port.
* coreable - enable core dumps.
* datesink - consume verbose timestamp stream.
* datesource - produce verbose timestamp stream.
* dec - display an argument number in decimal.
* dhhmmss - calculate D/HH:MM:SS from seconds.
* dump - display a file in a formatted hexidecimal dump.
* elapsedsleep - sleep for monotonic duration to specified resolution.
* elapsedtime - display monotonic elapsed time to specified resolution.
* epochtime - display POSIX epoch to higher resolution than just seconds.
* endpoint - convert an endpoint name into an IP address and port number.
* frequency - display Diminuto base tick frequency in hertz.
* headsup - sends a SIGHUP (hangup signal) to a running observe script.
* hex - display an argument number in hexidecimal.
* internettool - verify internet connectectivity.
* ipcalc - calculate IPv4 addresses and masks.
* iso8601 - converts seconds since the UNIX epoch into an ISO8601 timestamp.
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
* pps - uses Diminuto pintool to multiplex on a 1PPS GPIO pin.
* renametool - atomically rename or swap files in the same file system.
* serialtool - manipulate serial ports.
* shaper - shape traffic in a pipeline.
* sizeof - display sizeof of different data types.
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
* lbktest - Loopback on serial port (Serial).
* luxcontroller - LED and lux sensor PID loop (Pin, Modulator, I2C, Controller).
* luxrheostat - LED and lux sensor rheostat (Pin, Modulator, I2C).
* pinchange - Multiplexing of GPIO pins (Pin).
* pincleanup - Clean up pins exported to hardware test fixture (Pin).
* pintest - Setting and getting GPIO pins (Pin).
* timestuff - Tries out the underlying glibc time functions.
* walker - Walk the file system tree starting at a specified root (FS).

(Some of these functional tests depend upon hardware test fixtures I
fabricated specifically for this purpose. Depending on the platform,
some or all of these functional tests  may need to be run as root,
or as a user - e.g. pi - that is in the gpio group.)

# Unit Test Make Targets

* extended - these unit tests take a lunch time to run.
* geologic - these unit test take over night or more to run.
* gnuish - these unit tests are for features that require GNU.
* logging - these unit tests exercise the logging feature.
* mostest - these unit tests require "make most; make mostest" to run.
* nohup - these unit tests require "nohup make nohup" to run.
* privileged - these unit tests require "sudo make privileged" to run.
* sanity - these unit tests take a coffee break to run.
* signals - these unit tests exercise the kill signal handlers.
* sockets - these unit tests test the ipc4, ipc6, ipcl, mux, and poll features.
* threads - these unit tests exercise the POSIX thread-based features.
* timers - these unit tests exercise the timer feature.

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

# Dependencies

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
The command "modprobe ipv6" also worked albeit non-persistently.

    sudo modprobe ipv6
    sudo su
    echo "ipv6" >> /etc/modules

## Building

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

For my own workflow, I installed the following tools. (I found valgrind
not to work on some ARM Raspbian platforms, so most of my valgrind testing
has been on x86_64 Ubuntu platforms.)

    sudo apt-get install cscope
    sudo apt-get install git
    sudo apt-get install net-tools
    sudo apt-get install openssh-server
    sudo apt-get install screen
    sudo apt-get install valgrind
    sudo apt-get install vim

## inotifywait

The observe script requires the inotify tools. Not all distros install
these by default (e.g. Raspbian).

    sudo apt-get install inotify-tools

# Building

Clone, build, and sanity test Diminuto. The use of the src directory
is just my own convention.

    cd ~
    mkdir -p src
    cd src
    git clone https://github.com/coverclock/com-diag-diminuto
    cd com-diag-diminuto/Diminuto
    make pristine
    make depend
    make all
    . out/host/bin/setup
    make sanity

Run more extended tests that may take a lunch break to complete.

    make extended

Run long term tests that may take overnight to complete.

    make geologic

Optionally install Diminuto headers, library, and utilities in /usr/local
subdirectories. You can override the make variable INSTALL_DIR to change
/usr/local to something else (like /opt or whatnot).

    sudo make install

I typically never bother with this for development and instead let the
out/host/bin/setup bash source script set my environmental variables
like PATH and LD_LIBRARY_PATH. Your mileage may vary.

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

The unit tests (./tst)  and functional tests (./fun) also have a wealth
of examples of how to use the public APIs.

# Remarks

Diminuto started out many years ago as a project to build a minimal
Linux-based embedded system for an ARMv4 processor that seems laughbly
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

If you'd like to see a non-trivial application that makes use
of many Diminuto features in exactly the way I envisioned them being
used, check out the following:

* the Diminuto IPC Ancillary unit test uses a lot of Diminuto features
and is a good example of a non-trivial application that is multi-process
and multi-threaded;
* the Diminuto IPC Scatter/Gather unit test similarly uses a lot of
Diminuto features and is also multi-process;
* the gpstool application in the Hazer project (repo URL below) is the
Swiss Army knife of Hazer, a library that parses output from GPS devices,
and relies on Diminuto for most of its infrastructure;
* the survey and census functional tests in the Placer project (ditto),
which uses the SQLite relational database system, use the Diminuto
FS file system walker and are a good example of how a need in another
project leads to a virtuous cycle of organic growth in Diminuto.

# Issues

## Warnings

I try hard to resolve all warnings. However, I may generate one myself.
On some platforms (e.g. Raspbian), the renameat2(2) system call exists
in Linux, but there is no glibc support for it in GNU. In this case,
Diminuto generates its own API to renameat2(2), but warns you that this
is the case at compile time.

    src/diminuto_renameat2.c:40:5: warning: #warning renameat2(2) not available on this platform so using SYS_renameat2 instead! [-Wcpp]

## Failures

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
round-trip latency between the connected hosts when the socket is
closed, and it is easy for the unit test to get ahead of that. I
address this by putting some delay between the receiving a request
and sending a response in the server thread, simulating some workload.
(Diminuto does support the REUSE PORT socket option, but this
isn't quite it's intended purpose.)

## Memory

On a recent Raspberry Pi I have run out of memory compiling the unit
tests. I admit I was completely taken by surprise by this. This was on
a Raspberry Pi 4, running Raspbian 10 "buster" (Linux 5.4.51, GNU 8.3.0),
compiling "tst/unitttest-ipc-ancillary.c".

    cc1: out of memory allocating 65536 bytes after a total of 1487814656 bytes

I believe the regression arose when I added code to make the Unit Test
framework thread safe so that I could test multi-threaded features. The
cpp (C PreProcessor) stage works just fine, but the C compiler chokes
on compiling the preprocessed translation unit.

Turning optimizations off with "-O0" didn't help. Nor did increasing
the swap space. This release is still a 32-bit kernel, which limits a
process' address space to a maximum of 2GB of virutal memory. (The script
etc/rpiswap.sh constitutes my notes on how to increase the swap space
on the RPi.)

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

<https://raspberrypi.stackexchange.com/questions/50240/missing-build-file-when-building-for-rtl8812au-driver>

<https://www.raspberrypi.org/forums/viewtopic.php?t=46472>

# Soundtrack

<https://www.youtube.com/playlist?list=PLd7Yo1333iA--8T8OyYiDnLAMWGIirosP>

# Articles

Chip Overclock, "In Praise of do while (false)", 2009-12,
<https://coverclock.blogspot.com/2009/12/in-praise-of-do-while-false.html>

Chip Overclock, "Can't We All Jut Get Along, C++ and C", 2011-02,
<https://coverclock.blogspot.com/2011/02/cant-we-all-just-get-along-c-and-c.html>

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

# Acknowledgements

I owe a debt of gratitude to my mentors, particularly at Bell Labs and its
various spin-offs, who were so generous with their time and expertise over
the years, and to my various clients, who both inspired and sometimes even
shipped this software.
