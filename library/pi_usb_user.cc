/* pi_usb_user.cc
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

#include "pi_usb_user.h"

// cpu = "counts per unit", either micrometres (linear) or degrees (rotation)
float	PI_USB_CPU[PI_USB_MAX_CONTROLLERS];

int	PI_USB_FD[PI_USB_MAX_CONTROLLERS]; // file descriptors, indexed by axis/address

/****************************************************************************
 * OPEN
 ****************************************************************************/

/* The file descriptors are stored in the global array PI_USB_FD, indexed by
 * axis, ie the address set on dip switches, and also the integer at the end
 * of the device name (eg /dev/pi_usb2 is axis 2). Humans think in terms of
 * this axis/address; serial communications (and files in general) think in
 * terms of file descriptors. This indexing by axis allows you to have other
 * global arrays, most notably PI_USB_CPU, which sets the "counts per unit"
 * (where unit is either microns or degrees, depending on the stage type).  */
int	pi_usb_open(const char *tty) {
int	axis;
char	preamble[SERIAL_MAX_DEV_LEN];
	// Finds out the integer at the end of the device name.
	sscanf(tty, "%[^'_']_usb%d", &preamble, &axis);
	return pi_usb_open(tty, axis);
	}

int	pi_usb_open(const char *tty, int axis) {
	PI_USB_FD[axis] = serial_open(tty, 9600); // default baud rate
	return axis;
	}

/****************************************************************************
 * INIT
 ****************************************************************************/
void	pi_usb_init(int axis) {
	pi_usb_set_cpu(axis, PI_USB_DEFAULT_CPU); // should read it from stage database
	pi_usb_motor_on(axis);
	}

/****************************************************************************
 * CLOSE
 ****************************************************************************/
int	pi_usb_close(int axis) {
	return serial_close(PI_USB_FD[axis]);
	}

/****************************************************************************
 * SEND
 ****************************************************************************/
int	pi_usb_send(int axis, const char *cmd) {
	return serial_send(PI_USB_FD[axis], cmd, "\r", 1);
	}

int	pi_usb_send_raw(int axis, const char *cmd, int len) {
	return serial_send_raw(PI_USB_FD[axis], cmd, len);
	}


/****************************************************************************
 * RECEIVE
 ****************************************************************************/
int	pi_usb_receive(int axis, char *buf,int buf_len) {
	return serial_receive(PI_USB_FD[axis], buf, buf_len, (const char) 3, (const char) '\r');
	}

/****************************************************************************
 * UTILITY FUCTIONS (send and receive, obtain value etc
 ****************************************************************************/
int	pi_usb_send_and_receive(int axis, const char *cmd, char *buf, int buf_len) {
int	send_ret, receive_ret;

	send_ret = pi_usb_send(axis, cmd);
	if(send_ret != SERIAL_OK) {
		fprintf(stderr, "Error on send, returning %d\n", send_ret);
		return send_ret;
		}

	receive_ret = pi_usb_receive(axis, buf, buf_len);
	if(receive_ret != SERIAL_OK) {
		//fprintf(stderr, "Error on receive, returning %d\n", get_ret);
		return receive_ret;
		}
	return receive_ret;
	}

/* Lots of queries get a response like "P:+00003452", this returns just the integer */
int	pi_usb_obtain_integer_after_colon(int axis, const char *cmd) {
char	buf[PI_USB_BUF];
char	preamble[PI_USB_BUF];
int	value;
	pi_usb_send_and_receive(axis, cmd, buf, PI_USB_BUF);
	sscanf(buf, "%[^':']:%d", &preamble, &value); // anything that comes before---and isn't---':' goes into "preamble", int goes into "value"
	return value;
	}

/* Lots of commands require you to send a couple of letters then an integer */
int	pi_usb_send_cmd(int axis, const char *cmd, int number) {
char	cmd_str[PI_USB_BUF];
	sprintf(cmd_str, "%s%d", cmd, number);
	return pi_usb_send(axis, cmd_str);
	}

/****************************************************************************
 * FUNCTIONS SPECIFIC TO CONTROLLER
 ****************************************************************************/

/* There needs to be a way of the user program to discover if the stage is a
 * rotation stage or a linear stage. This doesn't change the way the driver 
 * or user libraries work, but it will change the way a user program will
 * interact with the user, for instance the "real world unit" for linear
 * stages is the micron, it's not unusual to want to move 10cm (10000um), 
 * however it's unlikely that you're ever going to want to move 10000 degrees.
 * This info should come from the stage database. For now, just return
 * "1". Fix later. */
int	pi_usb_is_rotation_stage(int axis) {
	return 1;
	}

/* Sets "counts per degree" for rotation stages, or "counts per um" for linear stages */
void	pi_usb_set_cpu(int axis, float cpu) {
	PI_USB_CPU[axis] = cpu;
	}

int	pi_usb_real_to_count(int axis, float pos_real) {
	return (int) roundf(pos_real * PI_USB_CPU[axis]);
	}

float	pi_usb_count_to_real(int axis, int pos) {
	return (float) (pos / PI_USB_CPU[axis]);
	}

void	pi_usb_motor_on(int axis) {
	pi_usb_send(axis, "MN");
	}

void	pi_usb_motor_off(int axis) {
	pi_usb_send(axis, "MF");
	}

/* Probes bit 2 of the second character of the first block of the "Tell Status"
 * (TS) response. Returns 1 if the motion is complete, 0 otherwise */
int	pi_usb_motion_complete(int axis) {
char	buf[PI_USB_BUF];
char	block_one_second_character;
int	i;

	pi_usb_send_and_receive(axis, "TS\r", buf, PI_USB_BUF);
	block_one_second_character = buf[3];
	i = (int) (block_one_second_character & (char) 4);
	if(i == 4) return 1;
	else return 0;
	}

/* Sits in a loop waiting for the motion to be complete */
void	pi_usb_wait_motion_complete(int axis) {
	pi_usb_wait_motion_complete(axis, PI_USB_DEFAULT_USLEEP);
	}

void	pi_usb_wait_motion_complete(int axis, useconds_t usleep_time) {
int	ret, done = 0;
	do{
		if(pi_usb_motion_complete(axis) == 1) done = 1;
		else usleep(usleep_time);
		} while(done == 0);
	}

int	pi_usb_get_pos(int axis) {
	return pi_usb_obtain_integer_after_colon(axis, "TP"); // TP = "Tell Position"
	}

float	pi_usb_get_pos_real(int axis) {
	return pi_usb_count_to_real(axis, pi_usb_get_pos(axis));
	}

void	pi_usb_set_pos(int axis, int pos) {
	pi_usb_send_cmd(axis, "DH", pos); // DH = "Define Home"
	}

void	pi_usb_set_pos_real(int axis, float pos_real) {
	pi_usb_set_pos(axis, pi_usb_real_to_count(axis, pos_real));
	}

void	pi_usb_set_origin(int axis) {
	pi_usb_set_pos(axis, 0);
	}

void	pi_usb_move_relative(int axis, int pos, int wait) {
	pi_usb_send_cmd(axis, "MR", pos); // MR = "Move Relative"
	if(wait == PI_USB_WAIT) pi_usb_wait_motion_complete(axis);
	}

void	pi_usb_move_relative_real(int axis, float pos_real, int wait) {
	pi_usb_move_relative(axis, pi_usb_real_to_count(axis, pos_real), wait);
	}

void	pi_usb_move_absolute(int axis, int pos, int wait) {
	pi_usb_send_cmd(axis, "MA", pos); // MA = "Move Absolute"
	if(wait == PI_USB_WAIT) pi_usb_wait_motion_complete(axis);
	}

void	pi_usb_move_absolute_real(int axis, float pos_real, int wait) {
	pi_usb_move_absolute(axis, pi_usb_real_to_count(axis, pos_real), wait);
	}

int	pi_usb_get_vel(int axis) {
	return pi_usb_obtain_integer_after_colon(axis, "TY"); // TP = "Tell programmed velocity"
	}

float	pi_usb_get_vel_real(int axis) {
	return pi_usb_count_to_real(axis, pi_usb_get_vel(axis));
	}

void	pi_usb_set_vel(int axis, int vel) {
	pi_usb_send_cmd(axis, "SV", vel); // SV = "Set Velocity"
	}

void	pi_usb_set_vel_real(int axis, float vel_real) {
	pi_usb_set_vel(axis, pi_usb_real_to_count(axis, vel_real));
	}



