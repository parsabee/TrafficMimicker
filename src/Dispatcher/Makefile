TARGET:=dispatcher
CC:=cc
CFLAGS:=-g -W -Wall -I../../include
LDFLAG:=-L../../lib -lCon
OBJECTS:=Dispatcher.o Dispatcher_main.o

all: $(TARGET)

dispatcher: Dispatcher.o Dispatcher_main.o
	$(CC) $^ -o $@ $(LDFLAG) 

Dispatcher.o: Dispatcher.c
Dispatcher_main.o: Dispatcher_main.c

clean:
	rm -f $(OBJECTS) $(TARGET)
