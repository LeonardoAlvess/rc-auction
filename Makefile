CC = g++
CFLAGS = -std=c++17 -Wall -g

CLIENT_SRC = client/user.cpp client/processes.cpp common/utils.cpp
CLIENT_OBJ = $(CLIENT_SRC:%.cpp=%.o)

SERVER_SRC = server/server.cpp server/sv_processes.cpp server/sv_actions.cpp server/sv_verifs.cpp common/utils.cpp
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
sv_processes.o: server/sv_processes.h server/sv_actions.h server/sv_verifs.h common/utils.h
sv_actions.o: server/sv_actions.h server/sv_verifs.h
sv_verifs.o: server/sv_verifs.h common/utils.h

.PHONY: clean

clean:
	rm -f client/*.o server/*.o
	rm -f user AS
	rm -f common/*.o
