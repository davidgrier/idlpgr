#
# flycapture2-directory Makefile for idlpgr
#
# Modification History
# 04/14/2016 Written by David G. Grier, New York University
#
# Copyright (c) 2016 David G. Grier
#
INSTALL = install
DESTINATION = lib
FILES = lib/libflycapture-c.so.2 \
	lib/libflycapture.so.2

all:

clean:
	-rm *~

install: all
	sudo $(INSTALL) -d $(DESTINATION)
	sudo $(INSTALL) -m 644 $(FILES) $(DESTINATION)

uninstall:
	sudo rm $(DESTINATION)/$(FILES)
	sudo rmdir $(DESTINATION)
