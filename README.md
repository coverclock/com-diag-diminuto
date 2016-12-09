com-diag-diminuto
=================

Copyright 2008-2016 by the Digital Aggregates Corporation, Colorado, USA.

LICENSE

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

As a special exception, if other files instantiate templates or
use macros or inline functions from this file, or you compile
this file and link it with other works to produce a work based on
this file, this file does not by itself cause the resulting work
to be covered by the GNU Lesser General Public License. However
the source code for this file must still be made available in
accordance with the GNU Lesser General Public License.

This exception does not invalidate any other reasons why a work
based on this file might be covered by the GNU Lesser General
Public License.

Alternative commercial licensing terms are available from the copyright
holder. Contact Digital Aggregates Corporation for more information.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, contact

    Free Software Foundation, Inc.
    59 Temple Place, Suite 330
    Boston MA 02111-1307 USA
    http://www.gnu.org/copyleft/lesser.txt

ABSTRACT

This file is part of the Digital Aggregates Corporation Diminuto package.
Diminuto is an open source library of systems programming functions and
tools written in C to simplify the writing of device drivers, daemons, and
systems applications for the Linux operating system. It has found its way
into a number of commercial products of the Corporation's clients, so don't
be surprised if you find this code being used legitimately elsewhere in
your travels. Diminuto includes a suite of unit tests that are used to
validate the software when it changes.

This software is an original work of its author(s).

Information regarding Diminuto can be found here:

    http://www.diag.com/navigation/downloads/Diminuto.html

CONTACT

    Chip Overclock
    Digital Aggregates Corporation
    3440 Youngfield Street, Suite 209
    Wheat Ridge CO 80033 USA
    http://www.diag.com
    mailto:coverclock@diag.com

FEATURES

* alarm - receive and check for a SIGALRM signal.
* barrier - acquire and release memory barriers (EXPERIMENTAL).
* buffer - used with heap feature to replace malloc/free with buffer pool.
* buffer_pool - buffer pool used with buffer feature.
* comparator - prototype of comparator function for search feartures.
* containerof - macro just like in the Linux kernel.
* controller - PID controller (EXPERIMENTAL).
* core - enable core dumps.
* countof - macro to compute array positions using sizeof operator.
* criticalsection - macros calls to implement pthread serialization.
* cue - logic level debouncer and edge detector.
* daemon - deamonizer that reports back to the parent on success.
* datum - memory mapped register operators for device drivers.
* delay - time delay and execution yield.
* dump - a variety of formatted memory dump functions.
* emit - example of how to use the log feature to make your own log output.
* endianess - determine processor endianess.
* escape - collapse and expand escape sequences in strings.
* fd - general handling of file descriptors (including sockets).
* frequency - determine common library time frequency.
* hangup - receive and check for a SIGHUP signal.
* heap - dependency injection mechanism for malloc/free alternatives.
* ipc4 - IPv4 socket interface.
* ipc6 - IPv6 socket interface.
* list - doubly linked list implementation.
* lock - lock, unlock, and check for lock files.
* log - generic logging mechanism for applications, daemons, and kernel modules.
* map - memory mapping in user or kernel state.
* memory - query system for memory architecture.
* mmdriver - kernel module implementing memory mapped I/O.
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
* serializedsection - macros use spinlocks to implement serialized code blocks.
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
* uninterruptiblesection - macros to block signals in a code block.
* unittest - simple unit test framework.
* well - like the pool feature but maintains requested alignment of objects.
* widthof - macro to return width in bits of a type.
 
UTILITIES
 
* coreable - enable core dumps.
* dec - display an argument number in decimal.
* dump - display a file in a formatted hexidecimal dump.
* gpiotool - simple get and set GPIO pins.
* hex - display an argument number in hexidecimal.
* ipcalc - calculate IPv4 addresses and masks.
* juliet - display the time in the local time zone in ISO 8601 format.
* memtool - manipulate memory mapped registers directly (requires root).
* mmdrivertool - manipulate memory mapped registers using a device driver.
* oct - display an argument number in octal.
* phex - display standard input in a printable form.
* pintool - scriptable get and set GPIO pins.
* shaper - shape traffic in a pipeline.
* sizeof - display sizeof of different data types.
* usecsleep - sleep for a specified number of microseconds.
* usectime - display the elapsed monotonic time in microseconds.
* wipe - aggressively wipe a storage device.
* zulu - display the time in UTC in ISO 8601 format.
 
DIRECTORIES
 
* drv - kernel space loadable module source files
* bin - utility source files
* inc - feature header files
* mod - user space dynamically linkable module source files
* src - feature source files
* tst - unit test source files
