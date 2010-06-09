/* pi_usb_user.h
 * Copyright (C) 2010 Steve D. Sharples
 *
 * A simple user library that allows you to do useful things to
 * a PI (Physik Instrumente) USB motor controller, such as the C-863
 * Mercury device. Uses our serial_user libraries.
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

#ifndef __PI_USB_USER__
#define __PI_USB_USER__

#include "../../serial/serial_user.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define	PI_USB_DEFAULT_USLEEP	100000
#define PI_USB_BUF		128

int	pi_usb_open(const char *tty);
int	pi_usb_close(int fd);
int	pi_usb_send(int fd, const char *cmd);
int	pi_usb_send_raw(int fd, const char *cmd, int len);
int	pi_usb_receive(int fd, char *buf, int len);
int	pi_usb_send_and_receive(int fd, const char *cmd, char *buf, int buf_len);
int	pi_usb_motion_complete(int fd);
void	pi_usb_wait_motion_complete(int fd);
void	pi_usb_wait_motion_complete(int fd, useconds_t usleep_time);


#endif
