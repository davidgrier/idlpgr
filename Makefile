#
# Top-level Makefile for idlpgr
#
# Modification History
# 07/21/2013 Written by David G. Grier, New York University
#
# Copyright (c) 2013 David G. Grier
#
LIB = idlpgr.so
IDLDIR = /usr/local/IDL
PRODIR = $(IDLDIR)/idlpgr
LIBDIR = $(PRODIR)

all:
	make -C lib
	make -C idl

install: all
	make -C idl install DESTINATION=$(PRODIR)
	make -C lib install DESTINATION=$(LIBDIR)

uninstall:
	make -C idl uninstall DESTINATION=$(PRODIR)
	make -C lib uninstall DESTINATION=$(LIBDIR)

clean:
	make -C idl clean
	make -C lib clean

dist:
	make clean
	cd ..
	tar czvf pointgrey.tgz pointgrey
