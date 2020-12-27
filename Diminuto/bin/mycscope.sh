#!/bin/bash
# Copyright 2014 by the Digital Aggregates Corporation, Colorado, USA.
# Licensed under the terms in LICENSE.txt.
# See: https://github.com/coverclock/com-diag-bin/cscope.sh .

LST=".cscope.lst"
REF=".cscope.out"
INF=".cscope.out.in"
POF=".cscope.out.po"

if [ ! \( -f ${INF} -a -f ${POF} \) ]; then

	if [ $# -eq 0 ]; then
		INC="`${CROSS_COMPILE}gcc -x c -E -v - < /dev/null 2>&1 | grep '^[ ]' | sed 's/^ //' | grep -v ' '`"
		DIR="."
	else
		INC=""
		DIR="$*"
	fi

	rm -f .cscope.*

	cp /dev/null ${LST}

	(
		if [ "${INC}" != "" ]; then
			for II in ${INC}; do
				echo INCLUDE="${II}" 1>&2
				find -P ${II} -type f -print
			done
		fi
		for DD in ${DIR}; do
			find -P ${DD} \
				-type d -name .svn -prune -o \
				-type d -name .git -prune -o \
				-type d -path ./.repo -prune -o \
				-type d -path ./out -prune -o \
				-type f -a \( \
					-name '*.[CcHhSs]' -o \
					-name '*.[a-z]mk' -o \
					-name '*.cc' -o \
					-name '*.cpp' -o \
					-name '*.cxx' -o \
					-name '*.dts' -o \
					-name '*.hpp' -o \
					-name '*.hx' -o \
					-name '*.hxx' -o \
					-name '*.inl' -o \
					-name '*.js' -o \
					-name '*.json' -o \
					-name '*.ld' -o \
					-name '*.map' -o \
					-name '*.md' -o \
					-name '*.mk' -o \
					-name '*.py' -o \
					-name '*.rc' -o \
					-name '*.sh' -o \
					-name '*.txt' -o \
					-name '*.xml' -o \
					-name '*.xsd' -o \
					-name '*_defconfig' -o \
					-name '.config' -o \
					-name 'Config.in*' -o \
					-name 'GNUmakefile*' -o \
					-name 'Kconfig*' -o \
					-name '[Mm]akefile*' -o \
					-name 'configure' -o \
					-name '*.conf' -o \
					-name '*.inc' -o \
					-name '*.java' -o \
					-name '*.x' -o \
					-name '*.x[nru]' -o \
					-name '*.xbn' -o \
					-false \
				\) -print
		done
	) | sed 's/^/"/;s/$/"/' | sort | uniq > ${LST}

fi

exec cscope -kqR -i${LST} -f${REF}
