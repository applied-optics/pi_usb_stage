include ../../config.mk

CXXFLAGS:=$(CXXFLAGS) -I../../library

all:	pi_usb_test pi_usb_send pi_usb_query pi_usb_register

.PHONY:	all clean install

pi_usb_register: pi_usb_register.o ../../library/$(full_libname)
	$(CXX) $(LDFLAGS) -o $@ $^

pi_usb_query: pi_usb_query.o ../../library/$(full_libname)
	$(CXX) $(LDFLAGS) -o $@ $^

pi_usb_send: pi_usb_send.o ../../library/$(full_libname)
	$(CXX) $(LDFLAGS) -o $@ $^

pi_usb_test: pi_usb_test.o ../../library/$(full_libname)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o test.* pi_usb_test

install:
	$(INSTALL) -d $(DESTDIR)$(prefix)/bin/
	$(INSTALL) pi_usb_send $(DESTDIR)$(prefix)/bin/
	$(INSTALL) pi_usb_query $(DESTDIR)$(prefix)/bin/
	$(INSTALL) pi_usb_register $(DESTDIR)$(prefix)/bin/
	$(INSTALL) install_pi_usb $(DESTDIR)$(prefix)/bin/
	$(INSTALL) actually_install_pi_usb $(DESTDIR)$(prefix)/bin/
	$(INSTALL) remove_pi_usb $(DESTDIR)$(prefix)/bin/
	$(INSTALL) -d $(DESTDIR)/etc/udev/rules.d/
	$(INSTALL) 98-pi-usb.rules $(DESTDIR)/etc/udev/rules.d/

