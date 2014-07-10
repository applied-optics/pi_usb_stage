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
	cp -f pi_usb_send pi_usb_query pi_usb_register install_pi_usb actually_install_pi_usb remove_pi_usb /usr/local/bin
	[ -d /etc/serial ] || mkdir /etc/serial
	chmod -R g+w /etc/serial
	chgrp -R scan /etc/serial
	[ -f /etc/udev/rules.d/98-pi-usb.rules ] || cp 98-pi-usb.rules /etc/udev/rules.d/98-pi-usb.rules
	[ -d /etc/pi_stage ] || mkdir /etc/pi_stage
	cp -av ../../usb_stage_database /etc/pi_stage/
	[ -f /etc/pi_stage/usb_installed_stages ] || cp ../../usb_installed_stages /etc/pi_stage/
	chmod -R g+w /etc/pi_stage
	chgrp -R scan /etc/pi_stage

