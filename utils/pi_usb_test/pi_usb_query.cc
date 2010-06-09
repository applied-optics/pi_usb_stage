#include "../../library/pi_usb_user.h"

int	main(int argc, char *argv[]) {

int	fd, ret, l;
char	buf[1024];
	memset(&buf, 0, 1024);

	if(argc != 2) {
		printf("Usage: %s query\n", argv[0]);
		exit(1);
		}

	//open serial port
	if((fd = pi_usb_open("ttyUSB0")) < 0) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", fd);
		return fd;
		}


	ret = pi_usb_send_and_receive(fd, argv[1], buf, 1024);
	printf("%s\n", buf);
	pi_usb_close(fd);
	}

