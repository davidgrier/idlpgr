#
# lib-directory Makefile for idlpgr
#
# Modification History
# 07/21/2013 Written by David G. Grier, New York University
# 03/17/2015 DGG Updated for DLM
#
# Copyright (c) 2013-2015 David G. Grier
#
TARGET = idlpgr
SRC = $(TARGET).c

SYS  = $(shell uname -s | tr '[:upper:]' '[:lower:]')
ARCH = $(shell uname -m)
LIBRARY = $(TARGET).$(SYS).$(ARCH).so

DLM = $(TARGET).dlm

IDL = idl -quiet
INSTALL = install
DESTINATION = lib

all: $(LIBRARY)

$(LIBRARY): $(SRC)
	@mkdir build 2>/dev/null ||:
	@$(IDL) compile_$(TARGET)

install: $(LIBRARY) $(DLM)
	sudo $(INSTALL) -d $(DESTINATION)
	sudo $(INSTALL) $(LIBRARY) $(DLM) $(DESTINATION)

uninstall:
	sudo rm $(DESTINATION)/*
	sudo rmdir $(DESTINATION)

test: $(TARGET)
	@$(IDL) testpgr

clean:
	-rm $(LIBRARY)
	-rm build
