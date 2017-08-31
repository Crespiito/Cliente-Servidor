#CC = g++
#FLAGS = -O2 -std=c++11 -Wall
#LIBS = -lzmqpp -lzmq -lsfml-audio -lpthread

#all: server client

#server: server.cpp	
#	$(CC) $(FLAGS) server.cpp $(LIBS) -o server.out 
#
#client: client.cpp
#	$(CC) $(FLAGS) client.cpp $(LIBS) -o client.out 

#clean:
#	rm client.out server.out


CC=g++ -std=c++11
#ZMQ_PATH=/usr/local
ZMQ_PATH=/home/utp/zmq
ZMQ_INC=$(ZMQ_PATH)/include
ZMQ_LIB=$(ZMQ_PATH)/lib
LIBS=-lzmq -lzmqpp

all: client server

client: client.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) client.cpp -o client.out $(LIBS) -pthread -lsfml-audio -lsfml-system

server: server.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) server.cpp -o server.out $(LIBS)
	

clean:
rm client.out server.out song.ogg

#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utp/zmq/lib