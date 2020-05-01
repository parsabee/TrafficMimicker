CC:=cc
CFLAGS:=-g -I../../include
BIN:=udp-echo udp-server tcp-echo tcp-server
LIB:=libCon.a
OBJ:=UDPClient.o UDPTestClient.o UDPServer.o UDPTestServer.o TCPServer.o TCPTestServer.o TCPClient.o TCPTestClient.o Connection.o

libCon.a: UDPClient.o UDPServer.o TCPServer.o TCPClient.o Connection.o
	ar r $@ $^
	cp $@ ..

.PHONY: bin clean

bin: $(BIN)

udp-echo: UDPClient.o UDPTestClient.o Connection.o
	$(CC) -o $@ $^

tcp-echo: TCPClient.o TCPTestClient.o Connection.o
	$(CC) -o $@ $^

udp-server: UDPServer.o UDPTestServer.o Connection.o
	$(CC) -o $@ $^

tcp-server: TCPServer.o TCPTestServer.o Connection.o
	$(CC) -o $@ $^

UDPClient.o: UDPClient.c
UDPServer.o: UDPServer.c
UDPTestClient.o: UDPTestClient.c
UDPTestServer.o: UDPTestServer.c
Connection.o: Connection.c
TCPServer.o: TCPServer.c
TCPTestServer.o: TCPTestServer.c
TCPTestClient.o: TCPTestClient.c

clean:
	rm -f $(BIN) $(OBJ) $(LIB)
