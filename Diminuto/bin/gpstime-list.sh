#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# AAAA:BB:CC:DD:EEEE:FF:GG
# AAAA: year before leap second.
#      BB: month before leap second.
#         CC: day before leap second.
#            DD: leap seconds added or substracted.
#               EEEE: year after leap second.
#                    FF: month after leap second.
#                       GG: day after leap second.

LEPLST="
1981:06:30:+1:1981:07:01
1982:06:30:+1:1982:07:01
1983:06:30:+1:1983:07:01
1985:06:30:+1:1985:07:01
1987:12:31:+1:1988:01:01
1989:12:31:+1:1990:01:01
1990:12:31:+1:1991:01:01
1992:06:30:+1:1992:07:01
1993:06:30:+1:1993:07:01
1994:06:30:+1:1994:07:01
1995:12:31:+1:1996:01:01
1997:06:30:+1:1997:07:01
1998:12:31:+1:1999:01:01
2005:12:31:+1:2006:01:01
2008:12:31:+1:2009:01:01
2012:06:30:+1:2012:07:01
2015:06:30:+1:2015:07:01
2016:12:31:+1:2017:01:01
"
