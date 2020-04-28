##################################
# Author: Parsa Bagheri
##################################
# this file will recurse down the
# src/ director and calls `make`
# in every module
##################################

TARGETS:=dispatcher tm-agent

#tm-agent src dir
TMAGENT_DIR:=src/tmagent

#dispatcher src dir
DISPATCHER_DIR:=src/dispatcher

all: $(TARGETS)

dispatcher:
	$(MAKE) -C $(DISPATCHER_DIR) -f Makefile
	mkdir -p bin
	cp $(DISPATCHER_DIR)/$@ bin

tm-agent: 
	$(MAKE) -C $(TMAGENT_DIR) -f Makefile
	mkdir -p bin
	cp $(TMAGENT_DIR)/$@ bin

clean:
	$(MAKE) -C $(TMAGENT_DIR) -f Makefile clean
	$(MAKE) -C $(DISPATCHER_DIR) -f Makefile clean
