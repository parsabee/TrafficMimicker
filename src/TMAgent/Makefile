TARGETS:=tm-agent tm-client tm-server
CC:=cc
CFLAGS:=-g -W -Wall -I../../include -DDEBUG
LDFLAGS:=-L../../lib -lCon -lADTs
OBJECTS:=TMAgent.o TMClient.o TMServer.o TMServices.o TMAgent_main.o TMServer_main.o TMClient_main.o

all: $(TARGETS)

tm-agent: TMAgent.o TMAgent_main.o TMClient.o TMServer.o TMServices.o
	$(CC) $^ -o $@ $(LDFLAGS) 

tm-client: TMClient.o TMServices.o TMClient_main.o
	$(CC) $^ -o $@ $(LDFLAGS)

tm-server: TMServer.o TMServices.o TMServer_main.o 
	$(CC) $^ -o $@ $(LDFLAGS)

TMAgent.o: TMAgent.c
TMAgent_main.o: TMAgent_main.c
TMClient.o: TMClient.c 
TMClient_main.o: TMClient_main.c
TMServer.o: TMServer.c
TMServer_main.o: TMServer_main.c
TMServices.o : TMServices.c

clean:
	rm -f $(OBJECTS) $(TARGETS)
