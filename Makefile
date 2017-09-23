# vi: set ts=4 shiftwidth=4:
# Copyright 2017 Digital Aggregates Corporation
# author:Chip Overclock
# mailto:coverclock@diag.com
# https://github.com/coverclock/com-diag-bin
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

.PHONY:	scope

scope:
	cscope.sh

.PHONY:	rescope

rescope:
	rm -f .cscope*
	cscope.sh
