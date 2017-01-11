#!/bin/bash
NEHOST=$(hostname)
NEFREQ=$(frequency)
NELINE=0
while read FEHOST FETIME FELINE; do
	NETIME=$(elapsedtime ${NEFREQ})
	DTIME=$((${NETIME} - ${FETIME}))
	DLINE=$((${NELINE} - ${FELINE}))
	echo ${NEHOST} ${NETIME} ${NELINE} ":" ${FEHOST} ${FETIME} ${FELINE} ":" ${DTIME} ${DLINE}
	sleep 1
	NELINE=$((${NELINE} + 1))
done
