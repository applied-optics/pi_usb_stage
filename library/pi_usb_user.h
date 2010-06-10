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
#include <math.h>

#define	PI_USB_DEFAULT_USLEEP	100000
#define PI_USB_BUF		128
#define PI_USB_MAX_CONTROLLERS	16

#define	PI_USB_NO_WAIT		0 // as in "do not wait for motion to stop" (move functions)
#define	PI_USB_WAIT		1

#define	PI_USB_DEFAULT_CPU	(float)440000/360	// "counts per degree" (should go in stage database, #define for now, change later

int	pi_usb_open(const char *tty);
int	pi_usb_open(const char *tty, int axis);
void	pi_usb_init(int axis);
int	pi_usb_close(int axis);
int	pi_usb_send(int axis, const char *cmd);
int	pi_usb_send_raw(int axis, const char *cmd, int len);
int	pi_usb_receive(int axis, char *buf, int len);
int	pi_usb_send_and_receive(int axis, const char *cmd, char *buf, int buf_len);
int	pi_usb_obtain_integer_after_colon(int axis, const char *cmd);
int	pi_usb_send_cmd(int axis, const char *cmd, int number);
int	pi_usb_is_rotation_stage(int axis);
void	pi_usb_set_cpu(int axis, float cpu);
int	pi_usb_real_to_count(int axis, float pos_real);
float	pi_usb_count_to_real(int axis, int pos);
void	pi_usb_motor_on(int axis);
void	pi_usb_motor_off(int axis);
int	pi_usb_motion_complete(int axis);
void	pi_usb_wait_motion_complete(int axis);
void	pi_usb_wait_motion_complete(int axis, useconds_t usleep_time);
int	pi_usb_get_pos(int axis);
float	pi_usb_get_pos_real(int axis);
void	pi_usb_set_pos(int axis, int pos);
void	pi_usb_set_pos_real(int axis, float pos_real);
void	pi_usb_set_origin(int axis);
void	pi_usb_move_relative(int axis, int pos, int wait);
void	pi_usb_move_relative_real(int axis, float pos_real, int wait);
void	pi_usb_move_absolute(int axis, int pos, int wait);
void	pi_usb_move_absolute_real(int axis, float pos_real, int wait);
int	pi_usb_get_vel(int axis);
float	pi_usb_get_vel_real(int axis);
void	pi_usb_set_vel(int axis, int vel);
void	pi_usb_set_vel_real(int axis, float vel_real);

#endif
