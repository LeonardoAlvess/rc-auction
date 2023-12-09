CC = g++
CFLAGS = -std=c++17 -Wall -g

SRC = user.cpp processes.cpp utils.cpp server.cpp sv_processes.cpp sv_actions.cpp sv_verifs.cpp
OBJ = $(SRC:%.cpp=%.o)

EXECUTABLES = user AS

all: $(EXECUTABLES)

user: user.o processes.o utils.o
	$(CC) $(CFLAGS) $^ -o $@

AS: server.o sv_processes.o sv_verifs.o sv_actions.o utils.o 
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

user.o: processes.h
processes.o: processes.h utils.h
server.o: utils.h sv_processes.h
sv_processes.o: sv_processes.h sv_actions.h sv_verifs.h utils.h
sv_actions.o: sv_actions.h sv_verifs.h
sv_verifs.o: sv_verifs.h utils.h

.PHONY: clean

clean:
	rm -f $(OBJ) $(EXECUTABLES)
