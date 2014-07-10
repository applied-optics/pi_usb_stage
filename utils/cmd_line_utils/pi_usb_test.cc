#include <cstdio>
#include <cstring>
#include <ctype.h>
#include "pi_usb_stage.h"

int	main(int argc, char *argv[]) {

int	fd, ret=0, l;
float	pos_deg;
float	retf;
char	buf[1024];
char	tmp[128];
char	preamble[128];
int	sl;
char	c;
char	hex_str[2];
int	axis;
	memset(&buf, 0, 1024);
	memset(&tmp, 0, 128);

	ret=pi_usb_recall_installed_stage(0, tmp, false);
	printf("ret = %d, tmp = %s\n", ret, tmp);
	
	//open serial port
	if((axis = pi_usb_open("pi_usb0")) < 0) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", axis);
		return axis;
		}
	printf("managed pi_usb_open\n");
	pi_usb_init(axis, false);
	printf("managed pi_usb_init\n");

	pi_usb_auto_stage(axis, tmp);

	ret = pi_usb_get_limit_status(axis);
	printf("ret = %d\n",ret);
	

//	printf("Moving relative 1mm\n"); pi_usb_move_relative_real(axis, 1000, 1);



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

	pi_usb_close(axis);
	}

