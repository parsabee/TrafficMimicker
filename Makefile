##################################
# Author: Parsa Bagheri
##################################
# this file will recurse down the
# src/ director and calls `make`
# in every module
##################################

TARGETS:=dispatcher tm-agent

#tm-agent src dir
TMAGENT_DIR:=src/TMAgent

#dispatcher src dir
DISPATCHER_DIR:=src/Dispatcher

#libraries
LIB_DIR:=lib/

all: $(TARGETS)

dispatcher: lib
	$(MAKE) -C $(DISPATCHER_DIR) -f Makefile
	mkdir -p bin
	cp $(DISPATCHER_DIR)/$@ bin

tm-agent: lib
	$(MAKE) -C $(TMAGENT_DIR) -f Makefile
	mkdir -p bin
	cp $(TMAGENT_DIR)/$@ bin

.PHONY: lib clean

lib:
	$(MAKE) -C $(LIB_DIR) -f Makefile

clean:
	$(MAKE) -C $(TMAGENT_DIR) -f Makefile clean
	$(MAKE) -C $(DISPATCHER_DIR) -f Makefile clean
	$(MAKE) -C $(LIB_DIR) -f Makefile clean
