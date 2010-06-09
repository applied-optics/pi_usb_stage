

#include "../../library/pi_usb_user.h"

int	main(int argc, char *argv[]) {

int	fd, ret;
int	l = 0;
int	chan = -1;
char	out_bytes[2];
char	chan_str[2]; // 0-9, A-F, but needs to be 2 chars long to store \0 (use sprintf to convert 10-15 to A-F)
char	buf[128];

	if(argc != 2) {
		printf("Usage: %s /dev/ttyUSB0 (or whatever)\n", argv[0]);
		exit(1);
		}

	if((fd = pi_usb_open(argv[1])) < 0) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", fd);
		return fd;
		}

	out_bytes[0] = (char) 1;

	do{
		sprintf(chan_str, "%hhX", l);
		out_bytes[1] = chan_str[0];
		pi_usb_send_raw(fd, out_bytes, 2);
		ret = pi_usb_send_and_receive(fd, "VE", buf, 128);
		//printf("l = %d, ret = %d\n", l, ret);
		if(ret == SERIAL_OK) chan = l;
		l++;
		} while(l <= 15 && chan < 0);

	printf("%d\n", chan);
	pi_usb_close(fd);
	}

