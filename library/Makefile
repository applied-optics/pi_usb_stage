include ../config.mk

.PHONY : all install clean dist distclean

all : $(full_libname)

$(full_libname) : pi_usb_stage.o
	$(CC) -shared -Wl,-soname,$(full_libname) $^ -o $@ -lserial_i13n

pi_usb_stage.o: pi_usb_stage.cc pi_usb_stage.h
	$(CC) -fPIC $(CFLAGS) -c $< -o $@

TAGS: $(wildcard *.c) $(wildcard *.h) $(wildcard *.c)
	etags $^

clean:
	rm -f *.o $(full_libname) TAGS

install: all
	$(INSTALL) -d $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	$(INSTALL) $(full_libname) $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	ln -sf $(full_libname) $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/$(libname)
	$(INSTALL) -d $(DESTDIR)$(prefix)/include/
	$(INSTALL) pi_usb_stage.h $(DESTDIR)$(prefix)/include/

