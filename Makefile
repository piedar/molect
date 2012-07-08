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

LIBDIR=/usr/lib
BINDIR=/usr/bin
CONFDIR=/etc/molect

all: hand

hand: libjmolwrapper.so hand.cpp config.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -L. -ljmolwrapper -o hand hand.cpp 

config.h:
	touch config.h
	echo "#define CONFDIR \"$(CONFDIR)/\"" >> config.h
ifeq ($(USE_OPENCV), true)
	echo "#define USE_OPENCV" >> config.h
endif

libjmolwrapper.so: jsocket.o jmolwrapper.cpp
	$(CXX) $(CXXFLAGS) -shared -o libjmolwrapper.so jmolwrapper.cpp jsocket.o

jsocket.o: socket/jsocket.c
	$(CC) $(CFLAGS) -c socket/jsocket.c

install:
	mkdir -p $(DESTDIR)/$(BINDIR) $(DESTDIR)/$(LIBDIR) $(DESTDIR)/$(CONFDIR)
	cp hand $(DESTDIR)/$(BINDIR)
	cp libjmolwrapper.so $(DESTDIR)/$(LIBDIR)
	cp -r configNI $(DESTDIR)/$(CONFDIR)
	
uninstall:
	rm $(DESTDIR)/$(BINDIR)/hand
	rm $(DESTDIR)/$(LIBDIR)/libjmolwrapper.so
	rm -r $(DESTDIR)/$(CONFDIR)

clean:
	rm -f *.o *.so config.h
