TARGET:=dispatcher
CC:=cc
CFLAGS:=-g -W -Wall -I../../include
LDFLAG:=-L../../lib -lCon
OBJECTS:=TrafficDispatcher.o Client.o

all: $(TARGET)

dispatcher: TrafficDispatcher.o Client.o
	$(CC) $^ -o $@ $(LDFLAG) 

TrafficDispatcher.o: TrafficDispatcher.c
Client.o: Client.c

clean:
	rm -f $(OBJECTS) $(TARGET)
