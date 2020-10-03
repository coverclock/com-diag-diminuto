# vi: set ts=4 shiftwidth=4:
# Copyright 2017-2020 Digital Aggregates Corporation
# author:Chip Overclock
# mailto:coverclock@diag.com
# https://github.com/coverclock/com-diag-diminuto
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

.PHONY:	all bootstrap release

all:
	make -C Diminuto all

bootstrap:

release:	all

.PHONY:	scope

scope:
	cscope.sh

.PHONY:	rescope

rescope:
	rm -f .cscope*
	cscope.sh
