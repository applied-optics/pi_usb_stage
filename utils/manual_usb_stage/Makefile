CFLAGS = -g

all:	manual_usb_stage
.PHONY:	all clean install

manual_usb_stage: manual_usb_stage.o pi_usb_user.o serial_user.o
	g++ $(CFLAGS) -o manual_usb_stage manual_usb_stage.o pi_usb_user.o serial_user.o -lncurses

manual_usb_stage.o: manual_usb_stage.cc ../../library/pi_usb_user.cc
	g++ $(CFLAGS) -c manual_usb_stage.cc -o manual_usb_stage.o

pi_usb_user.o:	../../library/pi_usb_user.cc ../../library/pi_usb_user.h
	g++ $(CFLAGS) -c ../../library/pi_usb_user.cc -o pi_usb_user.o

serial_user.o:	../../../serial/serial_user.c ../../../serial/serial_user.h
	g++ $(CFLAGS) -c ../../../serial/serial_user.c -o serial_user.o

clean:
	rm -f *.o test.* manual_usb_stage

install:
	cp -f manual_usb_stage /usr/local/bin

