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
#include <ctype.h>

#define	PI_USB_OK		0

#define	PI_USB_LIMIT_OK			0
#define	PI_USB_ON_POSITIVE_LIMIT	1
#define	PI_USB_ON_NEGATIVE_LIMIT	2
#define	PI_USB_LIMIT_BAD		4

#define	PI_USB_DEFAULT_USLEEP	100000
#define PI_USB_BUF		128
#define PI_USB_MAX_CONTROLLERS	16

#define	PI_USB_NO_WAIT		0 // as in "do not wait for motion to stop" (move functions)
#define	PI_USB_WAIT		1

#define	PI_USB_DEFAULT_CPU	(float)440000/360	// "counts per degree" (should go in stage database, #define for now, change later

#define PI_USB_INSTALLED_STAGES_FILE	"/etc/pi_stage/usb_installed_stages"
#define PI_USB_POSITION_FILE		"/etc/pi_stage/usb_position_real_units"

#define PI_USB_AUTO_STAGE_PATH		"/etc/pi_stage/usb_stage_database/"
#define PI_USB_PATH_LENGTH		256
#define PI_USB_NOT_SET			-1

// error codes for not opening the database
#define PI_USB_NO_STAGE_FILE		10	// can't find a database file with the correct name (file not there or dir perms wrong)
#define PI_USB_CANT_READ_STAGE_FILE	11	// can't read the file (permissions wrong on file or directory)
#define PI_USB_CANT_OPEN_STAGE_FILE	12	// can't open the file (unknown error)
#define PI_USB_AUTO_STAGE_ERROR		13	// something wrong in the file - probably wrong format or missing "end" statement

#define PI_USB_OPTIONS_LIMIT	100	// max no. of things in a db file for catching format errors
#define PI_USB_OPTION_LENGTH	100	// size of string to receive options into

#define	PI_USB_MAX_ROT_DISCREPANCY	1	// degree
#define	PI_USB_MAX_LIN_DISCREPANCY	20.0	// microns

#ifndef	TOLOWER
#define	TOLOWER(A)		{int TP;for(TP=0;TP<(int)strlen(A);TP++){A[TP]=tolower(A[TP]);}}
#endif

#ifndef	BOOL
#define	BOOL			int
#define	TRUE			1
#define	FALSE			0
#endif


int	pi_usb_open(const char *tty);
int	pi_usb_open(int axis);
int	pi_usb_open(const char *tty, int axis);
void	pi_usb_init(int axis);
void	pi_usb_init(int axis, BOOL silent);
int	pi_usb_close(int axis);
int	pi_usb_send(int axis, const char *cmd);
int	pi_usb_send_raw(int axis, const char *cmd, int len);
int	pi_usb_receive(int axis, char *buf, int len);
int	pi_usb_send_and_receive(int axis, const char *cmd, char *buf, int buf_len);
int	pi_usb_obtain_integer_after_colon(int axis, const char *cmd);
int	pi_usb_send_cmd(int axis, const char *cmd, int number);
BOOL	pi_usb_recall_all_axes_pos_real(BOOL *found_axis, float *pos, BOOL silent);
float	pi_usb_recall_pos_real(int axis, BOOL interactive, BOOL silent);
float	pi_usb_recall_pos_real(int axis, BOOL interactive, float max_discrepancy, BOOL silent);
BOOL	pi_usb_save_pos_real(int axis, BOOL silent);
BOOL	pi_usb_recall_installed_stage(int axis, char *stage_type);
BOOL	pi_usb_recall_installed_stage(int axis, char *stage_type, BOOL silent);
int	pi_usb_auto_stage(int axis, const char *name);
int	pi_usb_auto_read_db(struct pi_usb_params *stage, const char *name, const char *path);
void	pi_usb_auto_set_stage(int axis, struct pi_usb_params *stage);
int	pi_usb_get_limit_status(int axis);
void	pi_usb_set_rotation_stage_flag(int axis, int is_rot);
int	pi_usb_is_rotation_stage(int axis);
float	pi_usb_get_cpu(int axis);
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
void	pi_usb_set_trigger_mode(int axis, int trigger_mode);
void	pi_usb_set_trigger_pos(int axis, int pos);
void	pi_usb_set_trigger_pos_real(int axis, float pos_real);
void	pi_usb_set_trigger_increment(int axis, int increment);
void	pi_usb_set_trigger_increment_real(int axis, float increment_real);
void	pi_usb_set_channel(int axis, int channel, int zero_or_one);

struct pi_usb_params{
	int	p;		// p-term
	int	i;		// i-term
	int	d;		// d-term
	int	vff;		// vff-term
	int	ilimit;		// integration limit
	int	acceleration;	// acceleration
	int	velocity;	// velocity (counts per cycle)
	int	motor_mode;	// analogue (0) or PWM (1)
	int	limit_mode;	// 0 = active low (non-PI), 1 = active high (usual PI), or 2=auto
	float	cpu;		// encoder counts per "real world unit" (um or deg)
	int	is_rot;		// 0 if linear stage, 1 if rotation stage
	};

#endif
