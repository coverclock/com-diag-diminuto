com-diag-diminuto
=================

Linux systems programming library in C.

# Thanks

[![Say Thanks!](https://img.shields.io/badge/Say%20Thanks-!-1EAEDB.svg)](https://saythanks.io/to/coverclock)

# Copyright

Copyright 2008-2019 by the Digital Aggregates Corporation, Colorado, USA.

# License

Licensed under the terms in LICENSE.txt. 

# Contact

Chip Overclock  
Digital Aggregates Corporation  
3440 Youngfield Street, Suite 209  
Wheat Ridge CO 80033 USA  
<http://www.diag.com>    
<mailto:coverclock@diag.com>    

# Abstract

This file is part of the Digital Aggregates Corporation Diminuto package.
Diminuto ("tiny") is an open source library of systems programming
functions and tools written in C to simplify the writing of device
drivers, daemons, and systems applications for the Linux operating
system. It has found its way into a number of commercial products of the
Corporation's clients, so don't be surprised if you find this code being
used legitimately elsewhere in your travels. Diminuto includes a suite
of unit tests that are used to validate features when they change, and
a suite of functional tests that require a purpose-built hardware test
fixture used to validate features that depend on hardware (like GPIO).

This software is an original work of its author.

# Targets

This code is typically regression tested against one or more of the following
hardware targets and software platforms depending on the feature.

"Mercury"  
Dell OptiPlex 7040  
Intel x86_64 64-bit    
Intel Core i7-6700T Skylake @ 2.80GHz x 4 x 2  
Ubuntu 16.04.2 "xenial"  
Linux 4.4.0  
gcc 5.4.0    

"Nickel"    
Intel NUC5i7RYH    
Intel x86_64 64-bit    
Intel Core i7-5557U @ 3.10GHz x 2 x 2   
Ubuntu 16.04.4 "xenial"    
Linux 4.13.0     
gcc 5.4.0    

"Nickel" (updated)    
Intel NUC5i7RYH    
Intel x86_64 64-bit    
Intel Core i7-5557U @ 3.10GHz x 2 x 2    
Ubuntu 18.04 "bionic"    
Linux 4.15.0    
gcc 7.3.0    

"Bronze"  
Raspberry Pi 2B    
ARMv7 32-bit    
Broadcom BCM2836 Cortex-A7 @ 900MHz x 4  
Raspbian 8.0 "jessie"  
Linux 4.4.34  
gcc 4.9.2    

"Lead"  
Raspberry Pi 3B  
ARMv7 64-bit    
Broadcom BCM2837 Cortex-A53 @ 1.2GHz x 4  
Raspbian 8.0 "jessie"  
Linux 4.4.34  
gcc 4.9.2    

"Zinc"    
Raspberry Pi 3B with special HW test fixture    
ARMv7 64-bit    
Broadcom BCM2837 Cortex-A53 @ 1.2GHz x 4    
Raspbian 8.0 "jessie"    
Linux 4.4.34    
gcc 4.9.2    

"Gold"    
Raspberry Pi 3B+    
ARMv7 64-bit    
Broadcom BCM2837B0 Cortex-A53 @ 1.4GHz x 4      
Raspbian 9.4 "stretch"    
Linux 4.14.34    
gcc 6.3.0    

"Rhodium"    
Raspberry Pi 4B    
ARMv8 64-bit    
Broadcom BCM2711 Cortex-A72 @ 1.5GHz x 4    
Raspbian 10 "Buster"    
Linux 4.19.58    
gcc 8.3.0    

"Silicon10"    
VM running under Windows 10    
Intel x86_64 64-bit    
Intel Core i7-3520M @ 2.90GHz x 2    
Ubuntu 19.10 "Eoan"    
Linux 5.3.0    
gcc 9.2.1    

# Features

* alarm - receive and check for a SIGALRM signal.
* barrier - acquire and release memory barriers.
* buffer - used with heap feature to replace malloc/free with buffer pool.
* buffer_pool - buffer pool used with buffer feature.
* comparator - prototype of comparator function for search feartures.
* containerof - macro just like in the Linux kernel.
* controller - proportional/integral/derivative (PID) controller.
* core - enable core dumps.
* coherentsection - macros to implement scoped acquire/release memory barriers.
* countof - macro to compute array positions using sizeof operator.
* criticalsection - macros to implement scoped pthread mutex serialization.
* cue - logic level debouncer and edge detector.
* daemon - process deamonizer.
* datum - memory mapped register operators for device drivers.
* delay - time delay and execution yield.
* dump - a variety of formatted memory dump functions.
* emit - example of how to use the log feature to make your own log output.
* endianess - determine processor endianess.
* endpoint - parse a string containing host:service into an IP address and port.
* escape - collapse and expand escape sequences in strings.
* fd - general handling of file descriptors (including sockets).
* frequency - determine common library time frequency.
* fs - file system walking framework.
* hangup - receive and check for a SIGHUP signal.
* heap - dependency injection mechanism for malloc/free alternatives.
* i2c - Simplified thread-safe API around Linux I2C API.
* ipc4 - IPv4 socket interface.
* ipc6 - IPv6 socket interface (works for IPv4 endpoints too).
* list - doubly linked list implementation.
* lock - lock, unlock, and check for lock files.
* log - generic logging mechanism for applications, daemons, and kernel modules.
* map - memory mapping in user or kernel state.
* memory - query system for memory architecture.
* mmdriver - kernel module implementing memory mapped I/O.
* modulator - software pulse width modulator (PWM) generator.
* module - support for dynamically linked user space modules.
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
* headless - display contents of an observation file as it changes.
* headup - sends a SIGHUP (hangup signal) to a running headless script.
* hex - display an argument number in hexidecimal.
* internettool - verify internet connectectivity.
* ipcalc - calculate IPv4 addresses and masks.
* juliet - display the local time in ISO 8601 format.
* log - log from command line and/or stdin using Diminuto log functions.
* loopback - provide a configurable serial port loopback.
* memtool - manipulate memory mapped registers directly (requires root).
* mmdrivertool - manipulate memory mapped registers using a device driver.
* oct - display an argument number in octal.
* phex - display standard input in a printable form.
* pinchange - execute a command when a GPIO pin changes state.
* pintool - manipulate GPIO pins.
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
* pintest - Setting and getting GPIO pins (Pin).
* walker - Walk the file system tree starting at a specified root (FS).

(Some of these functional tests use hardware test fixtures I fabricated
specifically for this purpose.)

# Unit Test Suites

* make extended - these unit tests take a lunch time to run.
* make geologic - these unit test take over night or more to run.
* make halting - these unit tests hang if they don't work.
* make mostest - these unit tests require "make most" to be run first.
* sudo make privileged - these unit tests must be run as root.
* make sanity - these unit tests take a coffee break to run.
* nohup make nohup - these unit tests require "nohup" to work.

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
* src - feature implementation and private header source files.
* tst - unit test source files.
* txt - notes and other useful stuff.

# Soundtrack

<https://www.youtube.com/playlist?list=PLd7Yo1333iA--8T8OyYiDnLAMWGIirosP>

# Remarks

Diminuto started out many years ago as a project to build a minimal
Linux-based embedded system for an ARMv4 processor that seems laughbly
small now. It consisted of a stripped down 2.4 (later, 2.6) Linux
kernel with just the stuff I needed, Busybox, and an application. The
application was built around a small C-based library I developed that
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
don't, that's great too. Portions of it will still be shipping in a number
of commercial products, ranging from satellite communications systems
to cellular base station routers to in-flight entertainment systems.

There are several other repositories of C or C++ code that I wrote that are
built on top of Diminuto. Diminuto doesn't depend on any of them, but they
depend on it. In addition, there are several of my projects in which I just
cut and pasted a small amount of code, a function here and there, rather
than create a dependency for the entire library.

If you'd like to see a non-trivial application that makes heavy use
of many Diminuto features in exactly the way I envisioned them being
used, check out the gpstool utility in the Hazer project (repo URL
below). gpstool is the Swiss Army knife of Hazer, a library that parses
output from GPS devices. The fact that gpstool can do so much in a single
file of not much more than a thousand lines of C code is a testimony to
how useful Diminuto can be (and how much time it can save you).

N.B. Diminuto requires IPv6 support. IPv6 is not enabled by default on
all platforms (in particular the Raspberry Pi). Under Raspbian 10,
I added a line "ipv6" to /etc/modules and rebooted. The command
"modprobe ipv6" also worked albeit non-persistently.

# Repositories

<https://github.com/coverclock/com-diag-diminuto>

<https://github.com/coverclock/com-diag-assay>

<https://github.com/coverclock/com-diag-codex>

<https://github.com/coverclock/com-diag-hazer>

<https://github.com/coverclock/com-diag-obelisk>

<https://github.com/coverclock/com-diag-placer>

# Images

<https://www.flickr.com/photos/johnlsloan/albums/72157680352449986>

# References

<https://raspberrypi.stackexchange.com/questions/50240/missing-build-file-when-building-for-rtl8812au-driver>

# Articles

Chip Overclock, "Some Stuff That Has Worked For Me In C", 2017-04,
<https://coverclock.blogspot.com/2017/04/some-stuff-that-has-worked-for-me-in-c.html>

Chip Overclock, "When The Silicon Meets The Road", 2018-07,
<https://coverclock.blogspot.com/2018/07/when-silicon-meets-road.html>

Chip Overclock, "When Learning By Doing Goes To Eleven", 2020-03,
<https://coverclock.blogspot.com/2020/03/when-learning-by-doing-goes-to-eleven.html>

# Build

Clone, build, and play with Diminuto (some unit and functional tests
may require you be root).


    cd ~
    mkdir -p src
    cd src
    git clone https://github.com/coverclock/com-diag-diminuto
    cd com-diag-diminuto/Diminuto
    make pristine
    make depend
    make all
    . out/host/bin/setup

Optionally install Diminuto headers, library, and utilities in /usr/local.

    sudo make install

# Acknowledgements

I owe a debt of gratitude to my mentors, particularly at Bell Labs and its
various spin-offs, who were so generous with their time and expertise over
the years, and to my various clients, who both inspired and sometimes even
shipped this software.
