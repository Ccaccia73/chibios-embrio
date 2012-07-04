TESTLIB=libembryo_test.a

DEF_CHIBIOS_VM=_CHIBIOS_VM_

CC=gcc
AR=ar
MKDIR=mkdir

COMMONFLAGS = 

ARFLAGS=rc

#CFLAGS+=-c
#CFLAGS+=$(COMMONFLAGS) -Wall -Werror
CFLAGS+=-Wall -O2

.DEFAULT_GOAL = all

############################################################################

EMBRYOLIBDIR = ../embryo-1.0.0-mod-lib

CHIBIOS = ../ChibiOS_2.2.7
CHIBIOS_EMBRIO = ../ChibiOS-Embrio
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

LIBDIR = testlib
LIBSRC = $(notdir $(wildcard $(EMBRYOLIBDIR)/*.c))
OBJS = $(addprefix $(LIBDIR)/, $(patsubst %.c, %.o, $(LIBSRC) ) )

lib: $(LIBDIR) $(LIBDIR)/$(TESTLIB)
#	@echo "lib sources"
#	@echo $(LIBSRC)
#	@echo "Objects"
#	@echo $(OBJS)
#	@echo "Done"

$(LIBDIR):
	$(MKDIR) $(LIBDIR)


$(LIBDIR)/%.o: $(EMBRYOLIBDIR)/%.c $(EMBRYOLIBDIR)/Embryo.h $(EMBRYOLIBDIR)/embryo_private.h
	@echo ==== Compiling [$@] ====
	$(CC) -U$(DEF_CHIBIOS_VM) $(INCDIR) -c $(CFLAGS) $(INCDIR) $< -o $@
	@echo
	

$(LIBDIR)/$(TESTLIB): $(OBJS)
	@echo  ==== Building $@ ====
	$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo "done."
	
#############################################################################

.PHONY: all exe lib clean

############################################################################ 

all: exe lib

############################################################################ 

clean:
	@echo "removing $(LIBDIR)/$(TESTLIB)..."
	rm -f $(LIBDIR)/$(TESTLIB)
	@echo "removing objects..."
	rm -f $(LIBDIR)/*.o
	rm -rf $(LIBDIR)
	@echo "done."


############################################################################
