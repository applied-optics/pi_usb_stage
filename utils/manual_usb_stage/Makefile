include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../library
LDFLAGS:=$(LDFLAGS) -L../../library

all:	pi_usb_manual_stage
.PHONY:	all clean install

pi_usb_manual_stage: manual_usb_stage.o ../../library/$(full_libname)
	$(CXX) $(LDFLAGS) -o $@ $^ -lncurses

manual_usb_stage.o: manual_usb_stage.cc
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -f *.o test.* pi_usb_manual_stage

install:
	$(INSTALL) -d $(DESTDIR)$(prefix)/bin/
	$(INSTALL) pi_usb_manual_stage $(DESTDIR)$(prefix)/bin/

