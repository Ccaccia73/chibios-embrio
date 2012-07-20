EXE+=embryo_sim
EXEDIR=./

###############################################################################

EMBRYOLIB_INCLUDE = ../embryo-1.0.0-lib
EMBRYOLIBDIR = $(EMBRYOLIB_INCLUDE)/simdir
EMBRYOLIB=_embryo_sim

###############################################################################

INCDIR+= -I$(EMBRYOLIB_INCLUDE)

CC=gcc
MKDIR=mkdir

COMMONFLAGS = 
CFLAGS+=-O2 -Wall -g3 $(INCDIR)

###############################################################################

.DEFAULT_GOAL = all

###############################################################################

OBJDIR = obj
SRC = $(wildcard *.c)
OBJ = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(wildcard *.c)))

LIBS+=-l$(EMBRYOLIB)
LIBSINCLUDE+=-L$(EMBRYOLIBDIR)

###############################################################################

all: $(OBJDIR) $(OBJDIR)/$(EXE)

$(OBJDIR):
	@echo ==== Making directory [$@] ====
	$(MKDIR) $(OBJDIR)


$(OBJDIR)/$(EXE): $(OBJ)
	@echo -n "Building $@ ..."
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS) $(LIBSINCLUDE)
	mv $@ $(EXEDIR)
	@echo "done."


$(OBJDIR)/%.o: %.c  $(EMBRYOLIB_INCLUDE)/Embryo.h
	@echo ==== Compiling [$@] ====
	$(CC) -c $(CFLAGS) $< -o $@
	@echo

###############################################################################

.PHONY: all clean

###############################################################################
 
clean:
	@echo "removing $(EXE)..."
	rm -f $(EXEDIR)$(EXE)
	@echo "removing objects..."
	rm -f $(OBJDIR)/*.o
	rm -rf $(OBJDIR)
	@echo "done."

###############################################################################
