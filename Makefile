LIBNAME = libCDL_GLEngine.dll
OBJS =  $(patsubst %.cpp, %.o, $(wildcard CDL/GLEngine/*.cpp))
$(LIBNAME): $(OBJS)

prefix = /usr
incdir = $(prefix)/include
libdir = $(prefix)/lib
CPPFLAGS = -I.
CXXFLAGS = -O3 -DGLEW_STATIC
install = install

%.dll: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,--out-implib=$@.a -Wl,--export-all-symbols -Wl,--enable-auto-import -Wl,--whole-archive $? glew.o -Wl,--no-whole-archive -lCDL -lglu32 -lopengl32 -o $@
	
%.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared 	-Wl,--soname,$@ -Wl,--whole-archive $? -Wl,--no-whole-archive -o $@

install:
	$(install) -o root -g root -m 755 $(LIBNAME) $(libdir)
	$(install) -o root -g root -m 644 CDL/GLEngine.h $(incdir)/CDL
	if [ ! -d "$(incdir)/CDL/GLEngine" ]; then mkdir $(incdir)/CDL/GLEngine; fi
	$(install) -o root -g root -m 644 CDL/GLEngine/*.h $(incdir)/CDL/GLEngine
	$(install) -m 644 CDL_GLEngine.pc $(libdir)/pkgconfig

uninstall:
	rm -f $(libdir)/$(LIBNAME)
	rm -f $(incdir)/CDL/GLEngine.h
	rm -fR $(incdir)/CDL/GLEngine
	rm -fR $(libdir)/pkgconfig/CDL_GLEngine.pc

clean:
	rm -f CDL/GLEngine/*.o *.so *.dll
