CC=gcc
CFLAGS=-fPIC
CXX=g++
CXXFLAGS=$(CFLAGS)
INCLUDES=-I/usr/include/ni
LIBS=-lOpenNI -lboost_program_options -lopencv_highgui -lopencv_core


all: hand

hand: jmolwrapper.o hand.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o hand hand.cpp jmolwrapper.o

jmolwrapper.o: jsocket.o jmolwrapper.cpp
	$(CXX) $(CXXFLAGS) -Wl,-r -nostdlib -o jmolwrapper.o jmolwrapper.cpp jsocket.o

jsocket.o: socket/jsocket.c
	$(CC) $(CFLAGS) -c socket/jsocket.c

clean:
	rm -f *.o *.so
