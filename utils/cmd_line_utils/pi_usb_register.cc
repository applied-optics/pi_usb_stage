/* pi_usb_register.cc
 * Copyright (C) 2010 Steve D. Sharples
 *
 * A simple commandline utility that reports the "address" of
 * a PI (Physik Instrumente) USB motor controller, such as the C-863
 * Mercury device. Uses our pi_usb_user and serial_user libraries.
 * The address is set using the dip switches on the front of the 
 * controller, and can have a value from 0 to 15. It does this by
 * sending two bytes to the controller, (char) 1 and then (in turn)
 * the ascii values of '0' to '9', then 'A' to 'F'. If the correct
 * value is sent, then we will be able to "talk to" - ie get a
 * response from - the device. Once the correct address is found,
 * the program quits, outputting the value (in decimal) to stdout.
 * This can be used e.g. by a script that would take this value
 * and use it to give a sensible device name link that matches the
 * dip switch settings.
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
int	axis, ret;
int	l = 0;
int	chan = -1;
char	out_bytes[2];
char	chan_str[2]; // 0-9, A-F, but needs to be 2 chars long to store \0 (use sprintf to convert 10-15 to A-F)
char	buf[128];

	if(argc != 2) {
		printf("Usage: %s /dev/ttyUSB0 (or whatever)\n", argv[0]);
		exit(1);
		}

	if((axis = pi_usb_open(argv[1]), 0) < 0) {
		fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", axis);
		return axis;
		}

	out_bytes[0] = (char) 1; // First of the two bytes needs to be 1. (Not ascii(1), but actually 1).

	do{
		sprintf(chan_str, "%hhX", l); // Converts l (0..15) into hex (0..F)
		out_bytes[1] = chan_str[0]; // Just extract the char, make it the second byte
		pi_usb_send_raw(axis, out_bytes, 2); // Send these two bytes, without any trailing \r
		ret = pi_usb_send_and_receive(axis, "VE", buf, 128); // Ask the controller what version it is
		if(ret == SERIAL_OK) chan = l; // We don't care about the reponse, just the function return value
		l++;
		} while(l <= 15 && chan < 0);
	printf("%d\n", chan); // Output the number (in decimal) to stdout
	pi_usb_close(axis);
	}

