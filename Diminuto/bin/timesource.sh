#!/bin/bash
NEHOST=$(hostname)
NEFREQ=$(frequency)
NELINE=0
while true; do
	echo ${NEHOST} $(elapsedtime ${NEFREQ}) ${NELINE}
	sleep 1
	NELINE=$((${NELINE} + 1))
done
