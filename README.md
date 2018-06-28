# com-diag-diminuto

Linux systems programming library in C.

## Copyright

Copyright 2008-2018 by the Digital Aggregates Corporation, Colorado, USA.

## License

Licensed under the terms in LICENSE.txt. 

## Abstract

This file is part of the Digital Aggregates Corporation Diminuto package.
Diminuto (Spanish for "tiny") is an open source library of systems
programming functions and tools written in C to simplify the writing of
device drivers, daemons, and systems applications for the Linux operating
system. It has found its way into a number of commercial products of the
Corporation's clients, so don't be surprised if you find this code being
used legitimately elsewhere in your travels. Diminuto includes a suite
of unit tests that are used to validate the software when it changes.

This software is an original work of its author.

## Targets

This code is typically regression tested against one or more of the following
hardware targets and software platforms depending on the feature.

"Mmercury"  
Dell OptiPlex 7040  
Intel x86_64 64-bit    
Intel Core i7-6700T Skylake @ 2.80GHz x 8  
Ubuntu 16.04.2 "xenial"  
Linux 4.4.0  
gcc 5.4.0    

"Nickel"    
Intel NUC5i7RYH    
Intel x86_64 64-bit    
Intel Core i7-5557U @ 3.10GHz x 8    
Ubuntu 16.04.4 "xenial"    
Linux 4.13.0     
gcc 5.4.0    

"Lead"  
Raspberry Pi 3 Model B  
ARMv7 64-bit    
Broadcom BCM2837 Cortex-A53 @ 1.2GHz x 4  
Raspbian 8.0 "jessie"  
Linux 4.4.34  
gcc 4.9.2    

"Bronze"  
Raspberry Pi 2 Model B    
ARMv7 32-bit    
Broadcom BCM2836 Cortex-A7 @ 900MHz x 4  
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

## Web Page

<http://www.diag.com/navigation/downloads/Diminuto.html>

## Contact

Chip Overclock  
<mailto:coverclock@diag.com>  
Digital Aggregates Corporation  
<http://www.diag.com>  
3440 Youngfield Street, Suite 209  
Wheat Ridge CO 80033 USA  

## Features

* alarm - receive and check for a SIGALRM signal.
* barrier - acquire and release memory barriers (EXPERIMENTAL).
* buffer - used with heap feature to replace malloc/free with buffer pool.
* buffer_pool - buffer pool used with buffer feature.
* comparator - prototype of comparator function for search feartures.
* containerof - macro just like in the Linux kernel.
* controller - proportional/integral/derivative (PID) controller (EXPERIMENTAL).
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
* hangup - receive and check for a SIGHUP signal.
* heap - dependency injection mechanism for malloc/free alternatives.
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
 
## Utilities
 
* coreable - enable core dumps.
* datesink - consume verbose timestamp stream.
* datesource - produce verbose timestamp stream.
* dec - display an argument number in decimal.
* dump - display a file in a formatted hexidecimal dump.
* elapsedsleep - sleep for monotonic duration to specified resolution.
* elapsedtime - display monotonic elapsed time to specified resolution.
* epochtime - display POSIX epoch to higher resolution than just seconds.
* endpoint - convert an endpoint name into an IP address and port number.
* frequency - display Diminuto base tick frequency in hertz.
* hex - display an argument number in hexidecimal.
* internettool - test internet connectectivity.
* ipcalc - calculate IPv4 addresses and masks.
* juliet - display the local time in ISO 8601 format.
* log - log from a script using Diminuto log functions.
* loopback - provide a configurable serial port loopback.
* memtool - manipulate memory mapped registers directly (requires root).
* mmdrivertool - manipulate memory mapped registers using a device driver.
* oct - display an argument number in octal.
* phex - display standard input in a printable form.
* pinchange - execute a command when a GPIO pin changes state.
* pintool - manipulate GPIO pins.
* renametool - atomically rename or swap files in the same file system.
* serialtool - test serial ports.
* shaper - shape traffic in a pipeline.
* sizeof - display sizeof of different data types.
* timesink - consume concise timestamp stream.
* timesource - produce concise timestamp stream.
* usecsleep - sleep for a specified number of microseconds.
* usectime - display the elapsed monotonic time in microseconds.
* wipe - aggressively wipe a storage device.
* zulu - display the UTC time in ISO 8601 format.
 
## Directories
 
* bin - utility source files
* cfg - configuration makefiles
* drv - kernel space loadable module source files
* etc - miscellaneous occasionally useful files
* inc - public header files
* mod - user space dynamically linkable module source files
* out - build artifacts
* src - feature source files
* tst - unit test source files
* txt - notes and other useful stuff

## Remarks

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

## Repositories

<https://github.com/coverclock/com-diag-diminuto>

<https://github.com/coverclock/com-diag-assay>

<https://github.com/coverclock/com-diag-concha>

<https://github.com/coverclock/com-diag-drover>

<https://github.com/coverclock/com-diag-grandote>

<https://github.com/coverclock/com-diag-hazer>

<https://github.com/coverclock/com-diag-obelisk>

## Build
Clone, build, and install Diminuto in /usr/local.

    cd ~
    mkdir -p src
    cd src
    git clone https://github.com/coverclock/com-diag-diminuto
    cd com-diag-diminuto/Diminuto
    make pristine depend all
    sudo make install
