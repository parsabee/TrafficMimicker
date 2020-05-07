CC:=cc
CFLAGS:=-g -I../../include
LIB:=libADTs.a
OBJ:=arraylist.o iterator.o

$(LIB): $(OBJ)
	ar r $@ $^
	cp $@ ..

.PHONY: clean

arraylist.o: arraylist.c
iterator.o: iterator.c

clean:
	rm -f $(BIN) $(OBJ) $(LIB)
