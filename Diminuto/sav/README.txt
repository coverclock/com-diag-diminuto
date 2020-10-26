This directory contains some source code from the deleted com-diag-telegraph
repository that I thought might be useful someday. Telegraph was originally
created to store some ideas that came to me while working with very small
PIC microcontrollers, the smallest of which has a read/write memory store of
a mere ninety-six bytes. (that's bytes, not kilobytes, or any other *bytes).
Virtually everything that was useful in Telegraph has been ported to Diminuto.
The few things that weren't, I saved here, mostly just for documentary purposes.
I expect the Telegraph project name to be reused in the future. There will
surely be no connection between original Telegraph and the new project.

I've saved some functional tests from the com-diag-hazer repository that
are helpful in testing some of the GPIO support in the Pin feature. The
scripts use GPIO pins, two different GNSS devices, and the Hazer gpstool.
The gpstool application requires the Hazer library.

Also included here is a test of the Diminuto CXXCAPI ("Sexy API") feature
that provides support to integrate C++ and C code bases (e.g. legacy C
code bases with new C++ development, or vice versa). It sits outside of
the normal Diminuto build. See the comments in the makefile for more
detail.
