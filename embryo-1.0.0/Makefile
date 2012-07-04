SUBDIRS = embryo-1.0.0-cc embryo-1.0.0-lib embryo-1.0.0-sim

.PHONY: subdirs $(SUBDIRS) all clean

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C  $@ $(MAKECMDGOALS)
	
all: subdirs

clean: subdirs
	
embryo-1.0.0-sim: embryo-1.0.0-cc

embryo-1.0.0-cc: embryo-1.0.0-lib