#!/bin/bash

# This is built for a specific test fixture I breadboarded up.
# After the values of input pins 22 and 27 are read and displayed,
# and the LEDs on output pins 16, 20, and 21 cycle on and off,
# press the button on pin 27 to advance the 3-bit binary counter
# displayed on the LEDs, or press the button pn pin 22 simultaneously
# with pin 27 to exit.

. $(readlink -e $(dirname ${0})/../bin)/setup

pintool -p 16 -n -x -o
pintool -p 20 -n -x -o
pintool -p 21 -n -x -o
pintool -p 22 -n -x -i -L
pintool -p 27 -n -x -i -H

PIN22=$(pintool -p 22 -r)
echo PIN22=${PIN22}
PIN27=$(pintool -p 27 -r)
echo PIN27=${PIN27}

pintool -p 16 -s
sleep 1
pintool -p 20 -s
sleep 1
pintool -p 21 -s
sleep 1
pintool -p 16 -c
sleep 1
pintool -p 20 -c
sleep 1
pintool -p 21 -c
sleep 1

VALUE=0
pintool -p 27 -b 10000 | while read PIN27; do
	PIN22=$(pintool -p 22 -r)
	if [[ ${PIN22} -ne 0 ]]; then
		break
	elif [[ ${PIN27} -ne 0 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		pintool -p 16 -w ${PIN16}
		pintool -p 20 -w ${PIN20}
		pintool -p 21 -w ${PIN21}
	else
		:
	fi
done

exit 0
