:
# vi: set ts=4:

###############################################################################
#
#	Copyright 2008 Digital Aggregates Corp., Arvada CO 80001-0587, USA.
#	This file is part of the Digital Aggregates Desperado library.
#	
#	This library is free software; you can redistribute it and/or
#	modify it under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation; either
#	version 2.1 of the License, or (at your option) any later version.
#
#	As a special exception, if other files instantiate templates or
#	use macros or inline functions from this file, or you compile
#	this file and link it with other works to produce a work based on
#	this file, this file does not by itself cause the resulting work
#	to be covered by the GNU Lesser General Public License. However
#	the source code for this file must still be made available in
#	accordance with section (4) of the GNU Lesser General Public
#	License.
#
#	This exception does not invalidate any other reasons why a work
#	based on this file might be covered by the GNU Lesser General
#	Public License.
#
#	This library is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General
#	Public License along with this library; if not, write to the
#	Free Software Foundation, Inc., 59 Temple Place, Suite 330,
#	Boston, MA 02111-1307 USA, or http://www.gnu.org/copyleft/lesser.txt.
#
#	$Name: $
#
#	$Id: $
#
###############################################################################

LST=.cscope.lst
REF=.cscope.out

rm -f .cscope.*
cp /dev/null ${LST}

if [ $# -eq 0 ]; then
	DIR="."
else
	DIR="$*"
fi

for DD in ${DIR}; do
	find ${DD} -type f -a \( \
		-name '*.[CcHhSs]' -o \
		-name '*.cpp' -o \
		-name '*.hpp' -o \
		-name '*.inl' -o \
		-name '*.dts' -o \
		-name '[Mm]akefile*' -o \
		-name 'GNUmakefile*' -o \
		-name '*.map' \
		-name '*.mk' -o \
		-name '*.[a-z]mk' -o \
		-name '.config' -o \
		-name 'Kconfig*' -o \
		-name 'Config.in*' -o \
		-name '*.sh' -o \
		-name '*.xml' -o \
		-name '*.xsd' \
	\) -print >> ${LST}
done

exec cscope -kqR -i${LST} -f${REF}
