CC=gcc
CFLAGS=-fPIC
CXX=g++
CXXFLAGS=$(CFLAGS)


all: jmolwrapper.o jsocket.o

jmolwrapper.o: jmolwrapper.cpp
	$(CXX) -c $(CXXFLAGS) jmolwrapper.cpp

jsocket.o: socket/jsocket.c
	$(CC) -c $(CFLAGS) socket/jsocket.c

clean:
	rm -f *.o
