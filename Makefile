CC = g++
CFLAGS = -std=c++17 -Wall

SRC = user.cpp processes.cpp utils.cpp server.cpp
OBJ = $(SRC:%.cpp=%.o)

EXECUTABLES = user AS

user: user.o processes.o utils.o
	$(CC) $(CFLAGS) $^ -o $@

AS: server.o utils.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


user.o: processes.h
processes.o: processes.h utils.h
server.o: utils.h

.PHONY: clean

clean:
	rm -f $(OBJ) $(EXECUTABLES)
	