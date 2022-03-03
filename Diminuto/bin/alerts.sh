#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Emits zero or more BEL characters ('\a') to the controlling terminal with
# at least the specified number of microseconds delay in between.
#
# USAGE
#
# alerts [ COUNT [ MICROSECONDS ] ]
#
# NOTES
#
# When I enter the command "alert", bash appears to invoke something else,
# even though "which alert" returns nothing, and "man bash" doesn't suggest
# anything either.
#
# While "man sleep" suggests that the argument in seconds for the delay time
# need not be an integer, when I try something smaller than 1, no error message
# is emitted, and the delay appears to be (at least close to) zero.
#

COUNT=${1:-"1"}
USECONDS=${2:-"1000000"}

while [[ ${COUNT} > 0 ]]; do
	echo -e '\a'
	COUNT=$((${COUNT} - 1))
	if [[ ${COUNT} > 0 ]]; then
		usecsleep ${USECONDS}
	fi
done

exit 0
