#include "../../library/pi_usb_user.h"

int	main(int argc, char *argv[]) {

int	ret, l;
char	buf[1024];
	memset(&buf, 0, 1024);
	//open serial port
	if((ret = pi_usb_open("ttyUSB0")) != PI_USB_OK) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", ret);
		return ret;
		}


	ret = pi_usb_send("MR-110000\r");
//	do {
//		ret = pi_usb_motion_complete();
//		usleep(100000);
//		} while(ret==0);
//	if(ret==1) printf("Motion complete\n");
//	for(l=0; l < 1024; l++) {
//		printf("%hhu ", buf[l]);
//		}

	pi_usb_wait_motion_complete();
	printf("Motion complete\n");
	pi_usb_close();
	printf("\nDone.\n");
	}

