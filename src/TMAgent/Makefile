TARGET:=tm-agent
CC:=cc
CFLAGS:=-g -W -Wall -I../../include
LDFLAGS:=-L../../lib -lCon
OBJECTS:=TMAgent.o Server.o UDPConnection.o TCPConnection.o

all: $(TARGET)

tm-agent: TMAgent.o Server.o UDPConnection.o TCPConnection.o
	$(CC) $^ -o $@ $(LDFLAGS) 

TMAgent.o: TMAgent.c
Server.o: Server.c
UDPConnection.o: UDPConnection.c
TCPConnection.o: TCPConnection.c

clean:
	rm -f $(OBJECTS) $(TARGET)
