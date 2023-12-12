CC = g++
CFLAGS = -std=c++17 -Wall -g

USER_SRC = user/user.cpp user/processes.cpp user/utils.cpp
USER_OBJ = $(USER_SRC:%.cpp=%.o)

SERVER_SRC = as/server.cpp as/sv_processes.cpp as/sv_actions.cpp as/sv_verifs.cpp user/utils.cpp
SERVER_OBJ = $(SERVER_SRC:%.cpp=%.o)

EXECUTABLES = user as

all: $(EXECUTABLES)

user: $(USER_OBJ)
	$(CC) $(CFLAGS) $^ -o client

as: $(SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o server

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

user.o: user/processes.h user/utils.h
processes.o: user/processes.h user/utils.h
server.o: user/utils.h as/sv_processes.h
sv_processes.o: as/sv_processes.h as/sv_actions.h as/sv_verifs.h user/utils.h
sv_actions.o: as/sv_actions.h as/sv_verifs.h
sv_verifs.o: as/sv_verifs.h user/utils.h

.PHONY: clean

clean:
	rm -f user/*.o as/*.o
	rm -f as.o user.o
