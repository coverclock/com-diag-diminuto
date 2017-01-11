#!/bin/bash
SN=0
while read LINE; do echo $(hostname) $(zulu) ${SN} ${LINE}; SN=$((${SN} + 1)); sleep 1; done
