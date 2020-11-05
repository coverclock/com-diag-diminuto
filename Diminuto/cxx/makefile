# Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
# Licensed under the terms in README.h<BR>
#
# This is not strictly part of the Diminuto build since it includes
# C++ and I try to keep Diminuto just C to make things simpler.
# (This wasn't always the case, which is why there are some C++
# artifacts in the Diminuto Makefile). These translations units
# show how C++ can call C (trivial) and also how C can call C++
# with pointers to C++ objects and with type safety using the
# CXXCAPI ("sexy API") approach originally developed for my
# Grandote (nee Desperado) C++ framework.
# 
#  REFERENCES
# 
#  M. Cline et al., <I>C++ FAQs</I>, 2nd edition,
#  Addision-Wessley, 1999, pp. 538, "FAQ 36.05 How
#  can an object of a C++ class be passed to or from
#  a C function?"
# 
#  M. Cline, "C++ FAQ Lite", 2001-08-15, 29.8
#

all:	unittest-cxxcapi

unittest-cxxcapi:	thingc.o ../out/host/lib/libdiminuto.so
	g++ -o unittest-cxxcapi -I . -I ../inc Thing.cpp thingc.o -L../out/host/lib -ldiminuto -lpthread -lrt -ldl

thingc.o:	thingc.c
	gcc -o thingc.o -I . -I ../inc -c thingc.c

clean:
	rm -f thingc.o unittest-cxxcapi

run:	unittest-cxxcapi
	./unittest-cxxcapi
