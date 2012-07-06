SUBDIRS = embryo-1.0.0 ChibiOS-EmbrIO

.PHONY: subdirs $(SUBDIRS) all clean

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C  $@ $(MAKECMDGOALS)
	
all: subdirs

clean: subdirs
	
ChibiOS-EmbrIO: embryo-1.0.0
