EXE+=embryo_cc
EXEDIR=./

EMBRYOLIBDIR = ../embryo-1.0.0-lib

INCDIR+= -I$(EMBRYOLIBDIR)

LIBS+= -lm

CFLAGS+=-O2 -Wall $(INCDIR)


CC=gcc
MKDIR=mkdir

.DEFAULT_GOAL = all

###########################################################

OBJDIR = obj
SRC = $(wildcard *.c)
OBJ = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(wildcard *.c)))
# OBJ = $(SRC:.c=.o)

###########################################################


all: $(OBJDIR) $(OBJDIR)/$(EXE)

$(OBJDIR):
	@echo ==== Making directory [$@] ====
	$(MKDIR) $(OBJDIR)


$(OBJDIR)/$(EXE): $(OBJ)
	@echo -n "Building $@ ..."
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
	mv $@ $(EXEDIR)
	@echo "done."


$(OBJDIR)/%.o: %.c embryo_cc_amx.h embryo_cc_prefix.h embryo_cc_sc.h embryo_cc_sc5.scp embryo_cc_sc7.scp $(EMBRYOLIBDIR)/Embryo.h
	@echo ==== Compiling [$@] ====
	$(CC) -c $(CFLAGS) $< -o $@
	@echo

###########################################################

.PHONY: all exe clean

###########################################################

 
clean:
	@echo "removing $(EXE)..."
	rm -f $(EXEDIR)$(EXE)
	@echo "removing objects..."
	rm -f $(OBJDIR)/*.o
	rm -rf $(OBJDIR)
	@echo "done."

###########################################################
