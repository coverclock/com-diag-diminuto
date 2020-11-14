I've tried to keep C++ code out of the main Diminuto base, as it just
complicates the build. But I needed a way to test the CXXCAPI ("Sexy API")
feature, which supports C and C++ interoperability, and that requires
mixing C++ and C code. This directory contains the basic CXXCAPI unit
test. N.B. if you have C++ code in your application, you have to have
a C++ main program to insure the C++ run-time is initialized correctly.
Trying to hack a way to include C++ functions in a C main program is
a path to dispair, since in particular static constructors will not be
called, so static variables will not be initialized as expected.
