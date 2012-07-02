USE_OPENCV=true


CC=gcc
CFLAGS=-fPIC
CXX=g++
CXXFLAGS=$(CFLAGS)
INCLUDES=-I/usr/include/ni
LIBS=-lOpenNI -lboost_program_options

ifeq ($(USE_OPENCV), true)
LIBS+=-lopencv_highgui -lopencv_imgproc
endif


all: hand

hand: jmolwrapper.o hand.cpp config.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o hand hand.cpp jmolwrapper.o

config.h:
	touch config.h
ifeq ($(USE_OPENCV), true)
	echo "#define USE_OPENCV" >> config.h
endif

jmolwrapper.o: jsocket.o jmolwrapper.cpp
	$(CXX) $(CXXFLAGS) -Wl,-r -nostdlib -o jmolwrapper.o jmolwrapper.cpp jsocket.o

jsocket.o: socket/jsocket.c
	$(CC) $(CFLAGS) -c socket/jsocket.c

clean:
	rm -f *.o *.so config.h
