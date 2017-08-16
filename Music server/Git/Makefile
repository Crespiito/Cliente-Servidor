CC = g++
FLAGS = -O2 -std=c++11 -Wall
LIBS = -lzmqpp -lzmq

all: server client

server: server.cpp
	$(CC) $(FLAGS) server.cpp $(LIBS) -o server.out 

client: client.cpp
	$(CC) $(FLAGS) client.cpp $(LIBS) -o client.out -lsfml-audio

clean:
	rm client.out server.out
