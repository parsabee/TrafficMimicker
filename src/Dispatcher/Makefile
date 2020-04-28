TARGET:=dispatcher
CC:=cc
CFLAGS:=-g -W -Wall -I../../include
OBJECTS:=TrafficDispatcher.o Client.o

all: $(TARGET)

dispatcher: TrafficDispatcher.o Client.o
	$(CC) $(CFLAGS) $^ -o $@

TrafficDispatcher.o: TrafficDispatcher.c
Client.o: Client.c

clean:
	rm -f $(OBJECTS) $(TARGET)
