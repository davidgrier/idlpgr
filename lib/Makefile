#
# lib-directory Makefile for idlpgr
#
# Modification History
# 07/21/2013 Written by David G. Grier, New York University
#
# Copyright (c) 2013 David G. Grier
#
TARGET = idlpgr.so
SRCS = idlpgr.c
IDL = idl -quiet
INSTALL = install
DESTINATION = lib

all: $(TARGET)

$(TARGET): $(SRCS)
	@$(IDL) compile_idlpgr

install: all
	sudo $(INSTALL) -d $(DESTINATION)
	sudo $(INSTALL) $(TARGET) $(DESTINATION)

uninstall:
	sudo rm $(DESTINATION)/$(TARGET)
	sudo rmdir $(DESTINATION)

test: $(TARGET)
	@$(IDL) testpgr

clean:
	-rm $(TARGET) *~
	-rm build/*
