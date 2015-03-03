#
# lib-directory Makefile for idlpgr
#
# Modification History
# 07/21/2013 Written by David G. Grier, New York University
#
# Copyright (c) 2013 David G. Grier
#
SRCS = idlpgr.c
DLM = idlpgr.dlm
IDL = idl -quiet
INSTALL = install
DESTINATION = lib

all: library

library: $(SRCS)
	@$(IDL) compile_idlpgr

install: all
	sudo $(INSTALL) -d $(DESTINATION)
	sudo $(INSTALL) idlpgr*.so $(DLM) $(DESTINATION)

uninstall:
	sudo rm $(DESTINATION)/*
	sudo rmdir $(DESTINATION)

test: $(TARGET)
	@$(IDL) testpgr

clean:
	-rm $(TARGET) *~
	-rm build/*
