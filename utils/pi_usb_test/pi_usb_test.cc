#include "../../library/pi_usb_user.h"

int	main(int argc, char *argv[]) {

int	fd, ret, l;
char	buf[1024];
	memset(&buf, 0, 1024);
	//open serial port
	if((fd = pi_usb_open("ttyUSB0")) < 0) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", fd);
		return fd;
		}


//	ret = pi_usb_send(fd, "FE");
	ret = pi_usb_send(fd, "MR440000");
//	do {
//		ret = pi_usb_motion_complete();
//		usleep(100000);
//		} while(ret==0);
//	if(ret==1) printf("Motion complete\n");
//	for(l=0; l < 1024; l++) {
//		printf("%hhu ", buf[l]);
//		}

	pi_usb_wait_motion_complete(fd);
	printf("Motion complete\n");
	pi_usb_close(fd);
	printf("\nDone.\n");
	}

