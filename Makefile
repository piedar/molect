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

LIBDIR=$(DESTDIR)/usr/lib
BINDIR=$(DESTDIR)/usr/bin

all: hand

hand: libjmolwrapper.so hand.cpp config.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -L. -ljmolwrapper -o hand hand.cpp 

config.h:
	touch config.h
ifeq ($(USE_OPENCV), true)
	echo "#define USE_OPENCV" >> config.h
endif

libjmolwrapper.so: jsocket.o jmolwrapper.cpp
	$(CXX) $(CXXFLAGS) -shared -o libjmolwrapper.so jmolwrapper.cpp jsocket.o

jsocket.o: socket/jsocket.c
	$(CC) $(CFLAGS) -c socket/jsocket.c

install:
	cp hand $(BINDIR)
	cp libjmolwrapper.so $(LIBDIR)
	
uninstall:
	rm $(BINDIR)/hand
	rm $(LIBDIR)/libjmolwrapper.so

clean:
	rm -f *.o *.so config.h
