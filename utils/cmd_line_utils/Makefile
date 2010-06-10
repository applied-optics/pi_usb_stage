CFLAGS = -g

all:	pi_usb_test pi_usb_send pi_usb_query pi_usb_register
.PHONY:	all clean install

pi_usb_register: pi_usb_register.o pi_usb_user.o serial_user.o
	g++ $(CFLAGS) -o pi_usb_register pi_usb_register.o pi_usb_user.o serial_user.o

pi_usb_query: pi_usb_query.o pi_usb_user.o serial_user.o
	g++ $(CFLAGS) -o pi_usb_query pi_usb_query.o pi_usb_user.o serial_user.o

pi_usb_send: pi_usb_send.o pi_usb_user.o serial_user.o
	g++ $(CFLAGS) -o pi_usb_send pi_usb_send.o pi_usb_user.o serial_user.o

pi_usb_test: pi_usb_test.o pi_usb_user.o serial_user.o
	g++ $(CFLAGS) -o pi_usb_test pi_usb_test.o pi_usb_user.o serial_user.o

pi_usb_test.o: pi_usb_test.cc ../../library/pi_usb_user.cc
	g++ $(CFLAGS) -c pi_usb_test.cc -o pi_usb_test.o

pi_usb_user.o:	../../library/pi_usb_user.cc ../../library/pi_usb_user.h
	g++ $(CFLAGS) -c ../../library/pi_usb_user.cc -o pi_usb_user.o

serial_user.o:	../../../serial/serial_user.c ../../../serial/serial_user.h
	g++ $(CFLAGS) -c ../../../serial/serial_user.c -o serial_user.o

clean:
	rm -f *.o test.* pi_usb_test

install:
	cp -f pi_usb_send pi_usb_query pi_usb_register install_pi_usb remove_pi_usb /usr/local/bin
	[ -f /etc/udev/rules.d ] || cp 98-pi-usb.rules /etc/udev/rules.d/98-pi-usb.rules

