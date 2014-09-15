#
# IDL-directory Makefile for idlpgr
#
# Modification History
# 07/21/2013 Written by David G. Grier, New York University
#
# Copyright (c) 2013 David G. Grier
#
INSTALL = install
DESTINATION = pro
FILES = open_pgr.pro \
        close_pgr.pro \
        read_pgr.pro \
        dgghwpointgrey__define.pro

all:

clean:
	-rm *~

install: all
	sudo $(INSTALL) -d $(DESTINATION)
	sudo $(INSTALL) -m 644 $(FILES) $(DESTINATION)

uninstall:
	sudo rm $(DESTINATION)/$(FILES)
	sudo rmdir $(DESTINATION)
