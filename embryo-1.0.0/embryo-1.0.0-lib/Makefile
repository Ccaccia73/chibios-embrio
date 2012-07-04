
# libraries
ARMLIB=lib_embryo_arm.a

SIMLIB=lib_embryo_sim.a

# define for ChibiOS virtual machine
DEF_CHIBIOS_VM=_CHIBIOS_VM_

TC = arm-none-eabi-

CC=gcc
AR=ar
MKDIR=mkdir

COMMONFLAGS = -g -mcpu=cortex-m3 -mthumb
#COMMONFLAGS = 

ARFLAGS=rc

#CFLAGS+=-c
#CFLAGS+=$(COMMONFLAGS) -Wall -Werror
CFLAGS+=-Wall -O2

.DEFAULT_GOAL = all

############################################################################

CHIBIOS = ../../ChibiOS_2.2.7_HY
CHIBIOS_EMBRIO = ../../ChibiOS-Embrio
BOARD=HY-STM32_STM32F103V

INCDIR+=-I$(CHIBIOS)/os/hal/include/
INCDIR+=-I$(CHIBIOS)/os/hal/templates/
INCDIR+=-I$(CHIBIOS)/os/hal/platform/STM32/
INCDIR+=-I$(CHIBIOS)/os/kernel/include/
INCDIR+=-I$(CHIBIOS)/os/ports/GCC/ARMCMx/
INCDIR+=-I$(CHIBIOS)/os/ports/GCC/ARMCMx/STM32/
INCDIR+=-I$(CHIBIOS)/os/various/
INCDIR+=-I$(CHIBIOS)/test/
INCDIR+=-I$(CHIBIOS)/boards/$(BOARD)/
INCDIR+=-I$(CHIBIOS_EMBRIO)

############################################################################


# dependencies for ARM library
OBJARMDIR = armdir
OBJARM = $(addprefix $(OBJARMDIR)/, $(patsubst %.c, %.o, $(wildcard *.c)))

arm: $(OBJARMDIR) $(OBJARMDIR)/$(ARMLIB)

$(OBJARMDIR):
	$(MKDIR) $(OBJARMDIR)


$(OBJARMDIR)/%.o: %.c Embryo.h embryo_private.h
	@echo ==== Compiling [$@] ====
	$(TC)$(CC) -D$(DEF_CHIBIOS_VM) -c $(COMMONFLAGS) $(INCDIR) $(CFLAGS) $< -o $@
	@echo


$(OBJARMDIR)/$(ARMLIB): $(OBJARM)
	@echo  ==== Building $@ ====
	$(TC)$(AR) $(ARFLAGS) $@ $(OBJARM)
	@echo "done."

############################################################################
	
# dependencies for SIM library
OBJSIMDIR = simdir
OBJSIM = $(addprefix $(OBJSIMDIR)/, $(patsubst %.c, %.o, $(wildcard *.c)))

sim: $(OBJSIMDIR) $(OBJSIMDIR)/$(SIMLIB)

$(OBJSIMDIR):
	$(MKDIR) $(OBJSIMDIR)


$(OBJSIMDIR)/%.o: %.c Embryo.h embryo_private.h
	@echo ==== Compiling [$@] ====
	$(CC) -c $(INCDIR) $(CFLAGS) $< -o $@
	@echo


$(OBJSIMDIR)/$(SIMLIB): $(OBJSIM)
	@echo  ==== Building $@ ====
	$(AR) $(ARFLAGS) $@ $(OBJSIM)
	@echo "done."


############################################################################

.PHONY: all arm sim cleanarm cleansim clean

############################################################################ 

all: arm sim

############################################################################
	
clean: cleanarm cleansim

cleanarm:
	@echo "removing $(OBJARMDIR)/$(ARMLIB)..."
	rm -f $(OBJARMDIR)/$(ARMLIB)
	@echo "removing objects..."
	rm -f $(OBJARMDIR)/*.o
	rm -rf $(OBJARMDIR)
	@echo "done."

	
cleansim:
	@echo "removing $(OBJSIMDIR)/$(SIMLIB)..."
	rm -f $(OBJSIMDIR)/$(SIMLIB)
	@echo "removing objects..."
	rm -f $(OBJSIMDIR)/*.o
	rm -rf $(OBJSIMDIR)
	@echo "done."
	
#############################################################################	