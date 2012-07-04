The directory contains mostly the Embryo compiler and library.
The original version of Embryo is 1.0.0 , later versions require some other libraries (e.g. Eina).

Differences form the original Embryo-1.0.0:

a) Compiler
- search path of *.inc files have been simplified: some functions are not available in mingw compiler
- one or more *.inc files can be added for ChibiOS specific native functions

b) Library
- makefile have been modified to generate a static library with arm-none-eabi-gcc
  it can also generate a static library with standard gcc compiler
- a specific #define _CHIBIOS_VM_ modifies all memory allocation functions with the ones provided by ChibiOS

c) Simulator
- it can be useful to test the virtual machine and mock native functions on a pc. The simulator should use the standard gcc library and is not yet implemented.
