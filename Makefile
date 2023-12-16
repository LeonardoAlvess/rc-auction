CC = g++
CFLAGS = -std=c++17 -Wall -g

CLIENT_SRC = src/client/user.cpp src/client/processes.cpp src/common/utils.cpp
CLIENT_OBJ = $(CLIENT_SRC:%.cpp=%.o)

SERVER_SRC = src/server/server.cpp src/server/sv_processes.cpp src/server/sv_files.cpp src/server/sv_verifs.cpp src/common/utils.cpp
SERVER_OBJ = $(SERVER_SRC:%.cpp=%.o)

EXECUTABLES = client server

all: $(EXECUTABLES)

client: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $^ -o user

server: $(SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o AS

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

user.o: client/processes.h common/utils.h
processes.o: client/processes.h common/utils.h
server.o: common/utils.h server/sv_processes.h
sv_processes.o: server/sv_processes.h server/sv_files.h server/sv_verifs.h common/utils.h
sv_files.o: server/sv_files.h server/sv_verifs.h
sv_verifs.o: server/sv_verifs.h common/utils.h

.PHONY: clean

clean:
	rm -f src/client/*.o src/server/*.o
	rm -f user AS
	rm -f src/common/*.o
server_clean:
	rm -rf USERS/*
	rm -rf AUCTIONS/*
