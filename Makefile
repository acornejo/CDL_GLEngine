all: libGLEngine.a
libGLEngine.a: $(patsubst %.cpp, %.o, $(wildcard GLEngine/*.cpp))

CPPFLAGS=-I.
install = install
prefix = /usr
incdir = $(prefix)/include
libdir = $(prefix)/lib

%.a: $(patsubst %.cpp, %.o, $(wildcard *.cpp))
	ar cru $@ $^

install: all
	$(install) -o root -g root -m 644 libGLEngine.a $(libdir)
	$(install) -o root -g root -m 644 GLEngine.h $(incdir)
	if [ ! -d "$(incdir)/GLEngine" ]; then mkdir $(incdir)/GLEngine; fi
	$(install) -o root -g root -m 644 GLEngine/*.h $(incdir)/GLEngine

uninstall:
	rm -f $(libdir)/libGLEngine.a
	rm -f $(incdir)/GLEngine.h
	rm -f $(incdir)/GLEngine/*.h
	rm -fR $(incdir)/GLEngine

dist:
	tar -cjvf GLEngine.tar.bz2 $(wildcard GLEngine/*.cpp GLEngine/*.h) GLEngine.h Makefile

clean:
	rm -f GLEngine/*.o *.a
