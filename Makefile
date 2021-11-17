all: server client

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INC_DIR=$(SRC_DIRS)/headers
SERVER_DIRS=$(SRC_DIRS)/server
CLIENT_DIRS=$(SRC_DIRS)/client
COMMON_DIRS=$(SRC_DIRS)/common

CC=gcc
SERVER=$(shell find $(SERVER_DIRS) -name *.c)
CLIENT=$(shell find $(CLIENT_DIRS) -name *.c)
COMMON=$(shell find $(COMMON_DIRS) -name *.c)

CFLAGS = -Wall -I$(INC_DIR)
ifeq ($(OS),Windows_NT)
    RM=del -f
	CFLAGS+= -lwsock32 -lws2_32
else
    RM=rm -f

endif

dep : 
	gccmakedep $(INC) $(SRCS)

server: $(SERVER) $(COMMON)
	$(MKDIR_P) $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/server.out $(SERVER) $(COMMON) $(CFLAGS) 

client: $(CLIENT) $(COMMON)
	$(MKDIR_P) $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/client.out $(CLIENT) $(COMMON) $(CFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

MKDIR_P ?= mkdir -p