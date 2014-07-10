include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../library
LDFLAGS:=$(LDFLAGS) -L../../library

all:	manual_usb_stage
.PHONY:	all clean install

manual_usb_stage: manual_usb_stage.o ../../library/$(full_libname)
	$(CXX) $(LDFLAGS) -o $@ $^ -lncurses

manual_usb_stage.o: manual_usb_stage.cc
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -f *.o test.* manual_usb_stage

install:
	cp -f manual_usb_stage /usr/local/bin

