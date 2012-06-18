CC=gcc
CFLAGS=-fPIC
CXX=g++
CXXFLAGS=$(CFLAGS)


all: jmolwrapper.o

jmolwrapper.o: jsocket.o jmolwrapper.cpp
	$(CXX) $(CXXFLAGS) -Wl,-r -nostdlib -o jmolwrapper.o jmolwrapper.cpp jsocket.o 

jsocket.o: socket/jsocket.c
	$(CC) $(CFLAGS) -c socket/jsocket.c

clean:
	rm -f *.o
