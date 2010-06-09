/* pi_usb_query.cc
 * Copyright (C) 2010 Steve D. Sharples
 *
 * A simple commandline utility that allows you to send queries to
 * a PI (Physik Instrumente) USB motor controller, such as the C-863
 * Mercury device. Uses our pi_usb_user and serial_user libraries.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The author's email address is steve.sharples@nottingham.ac.uk
 */

#include "../../library/pi_usb_user.h"

int	main(int argc, char *argv[]) {
int	fd; // file descriptor of the usb device
char	buf[1024];

	memset(&buf, 0, 1024); // clear the buffer

	if(argc < 2) {
		printf("Usage: %s query [device]\n", argv[0]);
		printf("If device is not specified, then pi_usb0 is used.\n");
		exit(1);
		}

	if(argc < 3)	fd = pi_usb_open("pi_usb0");
	else		fd = pi_usb_open(argv[2]);
	if(fd < 0) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", fd);
		return fd;
		}
	pi_usb_send_and_receive(fd, argv[1], buf, 1024);
	printf("%s\n", buf);
	pi_usb_close(fd);
	}

