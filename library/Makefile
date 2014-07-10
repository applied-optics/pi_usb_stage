include ../config.mk

.PHONY : all install clean dist distclean

all : libpi_usb.so.0

libpi_usb.so.0 : pi_usb_user.o
	$(CC) -shared -Wl,-soname,libpi_usb.so.0 $^ -o $@

pi_usb_user.o: pi_usb_user.cc pi_usb_user.h
	$(CC) -fPIC $(CFLAGS) -c $< -o $@

TAGS: $(wildcard *.c) $(wildcard *.h) $(wildcard *.c)
	etags $^

clean:
	rm -f *.o libpi_usb.so.0 TAGS

install: all
	$(INSTALL) -d $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	$(INSTALL) libpi_usb.so.0 $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/
	ln -sf libpi_usb.so.0 $(DESTDIR)$(prefix)/lib${LIB_SUFFIX}/libpi_usb.so
	$(INSTALL) -d $(DESTDIR)$(prefix)/include/instrumentation/
	$(INSTALL) pi_usb_user.h $(DESTDIR)$(prefix)/include/instrumentation/

