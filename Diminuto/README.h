/* vim: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_README_
#define _H_COM_DIAG_DIMINUTO_README_

/*******************************************************************************

    Copyright 2008-2014 by the Digital Aggregates Corporation, Colorado, USA.

    ----------------------------------------------------------------------

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

    ----------------------------------------------------------------------

    ABSTRACT

    This file is part of the Digital Aggregates Corporation Diminuto package.
    Diminuto is an open source library of systems programming functions and
    tools written in C to simplify the writing of device drivers, daemons, and
    systems applications for the Linux operating system. It has found it's way
    into a number of commercial products of the Corporation's clients, so don't
    be surprised if you find this code being used legitimately elsewhere in
    your travels.

    This software is an original work of its author(s).

    Information regarding Diminuto can be found here:

        http://www.diag.com/navigation/downloads/Diminuto.html

    Contact:

        Chip Overclock
        Digital Aggregates Corporation
        3440 Youngfield Street, Suite 209
        Wheat Ridge CO 80033 USA
        http://www.diag.com
        mailto:coverclock@diag.com

    ----------------------------------------------------------------------

    INSTALLATION

    Here is the sequence of commands to build Diminuto:

    vi Makefile						# establish your configuration.
    make TARGET=<target> depend     # make dependencies.
    make TARGET=<target>			# make library, binaries.
    make TARGET=<target> all        # make library, binaries, modules, drivers.
    make TARGET=<target> package	# make tarball to copy to target.

    . out/<target>/bin/setup		# establish PATH etc. environment.
    unittest-<test>					# run unit test.

*******************************************************************************/

/**
 *  @file
 *
 *  This is the README for this project.
 */

#endif
