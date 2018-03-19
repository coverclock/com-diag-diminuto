#!/bin/bash

# usage: $0 PIN STATE PRIOR

PROGRAM=$(basename $0)
PIN=${1:-"-"}
STATE=${2:-"-"}
PRIOR=${3:-"-"}

echo ${PROGRAM} ${PIN} ${STATE} ${PRIOR} 1>&2

exit 0