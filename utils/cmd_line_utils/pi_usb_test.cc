#include "../../library/pi_usb_user.h"
#include <ctype.h>

int	main(int argc, char *argv[]) {

int	fd, ret=0, l;
float	pos_deg;
float	retf;
char	buf[1024];
char	tmp[128];
char	preamble[128];
int	sl;
char	c;
	memset(&buf, 0, 1024);

	sprintf(tmp,"/dev/pi_usb0");
	sl=strlen(tmp);
	if(isdigit(tmp[sl-2])) {
		printf("double-digit\n");
		ret = (int) strtol(tmp+sl-2, (char **) NULL, 10);
		}
	else if(isdigit(tmp[sl-1])) {
		printf("single-digit\n");
		ret = (int) strtol(tmp+sl-1, (char **) NULL, 10);
		}
//	sscanf(tmp, "%[^'_']_usb%d", &preamble, &ret);
	printf("strlen=%d, The number is: %d\n", sl,ret);

	printf("this number as a char: %c\n", ret+'0');

	//open serial port
//	if((fd = pi_usb_open("pi_usb0")) < 0) {
//		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", fd);
//		return fd;
//		}
//	pi_usb_init(fd);

//	printf("Setting position to 0 degrees\n"); pi_usb_set_pos_deg(fd, 0);
//	pos_deg = pi_usb_get_pos_deg(fd); ret = pi_usb_get_pos(fd); printf("Position: %f degrees, %d counts\n", pos_deg, ret);
//	printf("Moving absolute 0 deg and waiting\n"); pi_usb_move_absolute_deg(fd, 0, 1);
//	pos_deg = pi_usb_get_pos_deg(fd); ret = pi_usb_get_pos(fd); printf("Position: %f degrees, %d counts\n", pos_deg, ret);
//	printf("Moving absolute +90 deg and waiting\n"); pi_usb_move_absolute_deg(fd, 90, 1);
//	pos_deg = pi_usb_get_pos_deg(fd); ret = pi_usb_get_pos(fd); printf("Position: %f degrees, %d counts\n", pos_deg, ret);
//	printf("Pausing a second\n");sleep(1);
//	pos_deg = pi_usb_get_pos_deg(fd); ret = pi_usb_get_pos(fd); printf("Position: %f degrees, %d counts\n", pos_deg, ret);

//	ret = pi_usb_get_vel(fd); retf = pi_usb_get_vel_deg(fd);printf("Current velocity: %f deg/sec, %d counts/sec\n", retf, ret);
//	printf("Setting velocity to 90 deg/sec\n"); pi_usb_set_vel_deg(fd, 90);
//	ret = pi_usb_get_vel(fd); retf = pi_usb_get_vel_deg(fd);printf("Current velocity: %f deg/sec, %d counts/sec\n", retf, ret);
//	for(l = 0; l <= 90; l += 2) {
//		printf("Moving absolute %f deg and waiting...: ", (float) l); pi_usb_move_absolute_deg(fd, (float) l, 1);
//		pos_deg = pi_usb_get_pos_deg(fd); ret = pi_usb_get_pos(fd); printf("Position: %f degrees, %d counts\n", pos_deg, ret);
//		}

//	pi_usb_close(fd);
	}

