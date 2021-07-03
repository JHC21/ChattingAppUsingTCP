all: server client

CC=gcc
COMM=FTPtransfer.o recvFile.o sendFile.o DieWithError.o
CFLAGS = -Wall
SRC=$(COMM: .o=.c)
ifeq ($(OS),Windows_NT)
    RM=del -f
	CFLAGS+= -lwsock32 -lws2_32
else
    RM=rm -f

endif

server: ChattingServer.o HandleClient.o $(COMM)
	$(CC) -o server ChattingServer.o HandleClient.o $(COMM) $(CFLAGS) 

client: ChattingClient.o $(COMM)
	$(CC) -o client ChattingClient.o $(COMM) $(CFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *.exe