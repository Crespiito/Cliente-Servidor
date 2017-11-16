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
ZMQ_PATH=/usr/local
#ZMQ_PATH=/home/utp/zmq
ZMQ_INC=$(ZMQ_PATH)/include
ZMQ_LIB=$(ZMQ_PATH)/lib
LIBS=-lzmq -lzmqpp -pthread 

all: nodo

nodo: nodo.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) nodo.cpp -o nodo.out $(LIBS) 

clean:
	rm nodo.out

#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utp/zmq/lib