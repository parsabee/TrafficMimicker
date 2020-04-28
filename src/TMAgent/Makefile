TARGET:=tm-agent
CC:=cc
CFLAGS:=-g -W -Wall -I../../include
OBJECTS:=TMAgent.o Server.o UDPConnection.o TCPConnection.o

all: $(TARGET)

tm-agent: TMAgent.o Server.o UDPConnection.o TCPConnection.o
	$(CC) $(CFLAGS) $^ -o $@

TMAgent.o: TMAgent.c
Server.o: Server.c
UDPConnection.o: UDPConnection.c
TCPConnection.o: TCPConnection.c

clean:
	rm -f $(OBJECTS) $(TARGET)
