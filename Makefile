VERSION=1.0

include config.mk

DIRS=library utils

.PHONY : all clean install

all :
	for d in ${DIRS}; do $(MAKE) -C $${d}; done

clean :
	for d in ${DIRS}; do $(MAKE) -C $${d} clean; done

install : all
	$(INSTALL) -d $(DESTDIR)/etc/pi_stage
	$(INSTALL) usb_stage_database $(DESTDIR)/etc/pi_stage/
	$(INSTALL) usb_installed_stages $(DESTDIR)/etc/pi_stage/
	for d in ${DIRS}; do $(MAKE) -C $${d} install; done

dist : distclean
	mkdir pi_usb-$(VERSION)
	cp -pr library utils pi_usb-$(VERSION)/
	cp -p Makefile CMakeLists.txt CHANGELOG.txt README.txt GNU_General_Public_License.txt pi_usb-$(VERSION)/
	tar -zcf pi_usb-$(VERSION).tar.gz pi_usb-$(VERSION)

distclean : 
	rm -rf pi_usb-$(VERSION)
	rm -f pi_usb-$(VERSION).tar.gz
