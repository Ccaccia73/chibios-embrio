*****************************************************************************
** ChibiOS/RT port for ARM-Cortex-M3 STM32F107.                            **
*****************************************************************************

** TARGET **

The demo runs on an Olimex STM32-P107 board.

** The Demo **

The demo flashes the board LED using a thread, by pressing the button located
on the board the test procedure is activated with output on the serial port
SD3 (USART3).

** Build Procedure **

The demo has been tested by using the free Codesourcery GCC-based toolchain
and YAGARTO.
Just modify the TRGT line in the makefile in order to use different GCC ports.

** Notes **

Some files used by the demo are not part of ChibiOS/RT but are copyright of
ST Microelectronics and are licensed under a different license.
Also note that not all the files present in the ST library are distributed
with ChibiOS/RT, you can find the whole library on the ST web site:

                             http://www.st.com

embryo-cc -c hello2.p -S 128 

arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm hello1.eaf hello1.o

