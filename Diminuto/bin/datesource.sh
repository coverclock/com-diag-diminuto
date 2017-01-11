#!/bin/bash
SN=0
while true; do echo $(hostname) $(zulu) ${SN}; SN=$((${SN} + 1)); sleep 1; done
