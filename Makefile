CC = g++
CFLAGS = -std=c++11 -Wall

SRC = user.cpp processes.cpp utils.cpp
OBJ = $(SRC:%.cpp=%.o)

EXECUTABLE = user

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

user.o: processes.h
processes.o: processes.h utils.h

.PHONY: clean

clean:
	rm -f $(OBJ) $(EXECUTABLE)
	