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
int	PI_USB_IS_ROT[PI_USB_MAX_CONTROLLERS]; // = 0 if linear stage, = 1 if rotation stage
int	PI_USB_IS_STEPPER[PI_USB_MAX_CONTROLLERS]; // = 0 if DC controller, = 1 if stepper controller (different "motion complete")

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

/* Open by device name. Axis derived from device name (upto last 2 digits),
 * 0 if it cannot be found or if > 15 */
int	pi_usb_open(const char *tty) {
int	sl, axis = 0;
	sl = strlen(tty);
	if(isdigit(tty[sl - 2]))	axis = (int) strtol(tty + sl - 2, (char **) NULL, 10);
	else if(isdigit(tty[sl - 1]))	axis = (int) strtol(tty + sl - 1, (char **) NULL, 10);
	return pi_usb_open(tty, axis);
	}

/* Open by axis number. Device name /dev/pi_usbX is derived from this */
int	pi_usb_open(int axis) {
char	tty[SERIAL_MAX_DEV_LEN];
	snprintf(tty, SERIAL_MAX_DEV_LEN, "/dev/pi_usb%d", axis);
	return pi_usb_open(tty, axis);
	}

/* The above two wrapper functions ultimately use this, both the axis AND
 * the device name are specified. */
int	pi_usb_open(const char *tty, int axis) {
char	address_selection_code[2];
char	chan_str[2];

	if((axis > (PI_USB_MAX_CONTROLLERS - 1)) || (axis < 0))	axis = 0; // don't put up with any nonsense
	PI_USB_FD[axis] = serial_open(tty, 9600); // default baud rate
	// serial_open should return a file descriptor. If negative, something
	// is wrong, so return the error value
	if(PI_USB_FD[axis] < 0) return PI_USB_FD[axis];
	// otherwise, return the axis, rather than the file descriptor.
	// Before that, send it the address selection code again.
	else {
		sprintf(chan_str, "%hhX", axis); // Converts axis (0..15) into hex (0..F)
		address_selection_code[0] = (char) 1;
		address_selection_code[1] = chan_str[0]; // Just extract the char, make it the second byte

		pi_usb_send_raw(axis, address_selection_code, 2);
		return axis;
		}
	}

/****************************************************************************
 * INIT
 ****************************************************************************/
void	pi_usb_init(int axis) {
	pi_usb_init(axis, TRUE);
	}

void	pi_usb_init(int axis, BOOL silent) {
char	stage_type[PI_USB_BUF];
BOOL	got_installed_stage_type;

	if(pi_usb_recall_installed_stage(axis, stage_type, silent) == TRUE) {
		pi_usb_auto_stage(axis, stage_type);
		}
	else { // best to assume rotation stage, then the speeds will be too low rather than too high
		pi_usb_set_cpu(axis, PI_USB_DEFAULT_CPU);
		pi_usb_set_rotation_stage_flag(axis, 1);
		}
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
 * HOUSEKEEPING - STAGE TYPE ETC
 ****************************************************************************/
BOOL	pi_usb_recall_all_axes_pos_real(BOOL *found_axis, float *pos, BOOL silent) {
char	temp[30];
int	axis;
int	counter=0;
FILE	*fi;
int	i;

	for(i=0; i<PI_USB_MAX_CONTROLLERS; i++) {
		found_axis[i] = FALSE;
		}
	//check on the existing file---does it exist?
	if(access(PI_USB_POSITION_FILE, F_OK)!=0){
		printf("Recalling USB stage position: %s doesn't exist\n",PI_USB_POSITION_FILE);
		return FALSE;
		}
	//can we read the file?
	if(access(PI_USB_POSITION_FILE, R_OK)!=0){
		printf("Recalling USB stage position: %s can't be read (bad)\n",PI_USB_POSITION_FILE);
		return FALSE;
		}

	fi=fopen(PI_USB_POSITION_FILE,"r");

	//can we open the file?
	if(fi<0){
		printf("Recalling USB stage position: %s can't be opened (bad)\n",PI_USB_POSITION_FILE);
		fclose(fi);
		return FALSE;
		}

	do{
		counter++;
		fscanf(fi,"%s",temp);
		TOLOWER(temp);
		if(strstr(temp,"axis")!=NULL) {
			fscanf(fi,"%d",&axis);
			if((axis < 0) || (axis >= PI_USB_MAX_CONTROLLERS)) {
				if(silent == FALSE) printf("Illegal axis (%d)\n", axis);
				}
			else {
				fscanf(fi,"%f",&pos[axis]);
				found_axis[axis] = TRUE;
				if(silent == FALSE) printf("Axis %d: %.2f\n", axis, pos[axis]);
				}
			}
		}while((counter<PI_USB_OPTIONS_LIMIT) && (strstr(temp,"end")==NULL));
	fclose(fi);

	return TRUE;
	}

float   pi_usb_recall_pos_real(int axis, BOOL interactive, BOOL silent) {
	if(pi_usb_is_rotation_stage(axis) == 1)	return pi_usb_recall_pos_real(axis, interactive, PI_USB_MAX_ROT_DISCREPANCY, silent);
	else					return pi_usb_recall_pos_real(axis, interactive, PI_USB_MAX_LIN_DISCREPANCY, silent);
	}

float	pi_usb_recall_pos_real(int axis, BOOL interactive, float max_discrepancy, BOOL silent) {
BOOL	found_axis[PI_USB_MAX_CONTROLLERS];
float	pos[PI_USB_MAX_CONTROLLERS];
float	current_pos, saved_pos, ret_pos;
char	response[10];

	current_pos = pi_usb_get_pos_real(axis);
	pi_usb_recall_all_axes_pos_real(found_axis, pos, TRUE); // even if we're not silent, we don't want to hear about all the other axes
	if(found_axis[axis] == FALSE) {
		ret_pos = current_pos;
		if(silent == FALSE) printf("Position for axis %d could not be found from file, using current position: %.1f\n", axis, ret_pos);
		}
	else {
		saved_pos = pos[axis];
		if(silent == FALSE) {
			printf("Recalling USB stage positions: axis %d is at %.1f\n", axis, saved_pos);
			printf("According to the controller  : axis %d is at %.1f\n", axis, current_pos);
			}
		if(fabs(current_pos - saved_pos) <= max_discrepancy) {
			if(silent == FALSE) printf("Difference is <= %.1f, using controller's position\n", max_discrepancy);
			ret_pos = current_pos; // use the current position (from the controller)
			}
		else {
			if(interactive == FALSE) {
				if(silent == FALSE) printf("Difference is > %.1f, but interactive mode is off, trusting position from file\n", max_discrepancy);
				ret_pos = saved_pos;
				}
			else {
				// If interactive mode AND diff>discrepancy, then ask what to do (no matter what silent status is)
				printf("\nDecision time. Difference between controller and file > %.1f\n", max_discrepancy);
				printf("Which do you trust more, ");
				do{
					printf("the (f)ile or the (c)ontroller? ");
					fgets(response,9,stdin);
					} while((response[0]!='f')&&(response[0]!='c'));
				if(response[0]=='f'){
					printf("Ok then, restoring position from the file\n");
					ret_pos = saved_pos;
					}
				else {
					printf("Ok then, using the position according to the controller\n");
					ret_pos = current_pos;
					}
				}
			}
		}
	pi_usb_set_pos_real(axis, ret_pos);
	return ret_pos;
	}

BOOL	pi_usb_save_pos_real(int axis, BOOL silent) {
BOOL	found_axis[PI_USB_MAX_CONTROLLERS];
float	pos[PI_USB_MAX_CONTROLLERS];
int	i;
FILE	*out;

	// first read in all the stored positions, so we don't lose any information about the other axes
	pi_usb_recall_all_axes_pos_real(found_axis, pos, TRUE); // even if we're not silent, we don't want to hear about all the other axes
	pi_usb_wait_motion_complete(axis);
	pos[axis] = pi_usb_get_pos_real(axis);
	found_axis[axis] = TRUE; // just in case it wasn't in the file already

	out = fopen(PI_USB_POSITION_FILE, "w");
		if(out < 0) {
		printf("Saving USB stage position: %s can't be opened for writing (bad)\n", PI_USB_POSITION_FILE);
		return FALSE;
		}

	for(i=0; i<PI_USB_MAX_CONTROLLERS; i++) {
		if(found_axis[i] == TRUE) {
			fprintf(out,"axis %d %f\n", i, pos[i]);
			}
		}
	fclose(out);

	if(silent == FALSE) printf("Saving axis %d position (%.2f) to %s\n", axis, pos[axis], PI_USB_POSITION_FILE);
	return TRUE;
	}

BOOL	pi_usb_recall_installed_stage(int axis, char *stage_type) {
	return pi_usb_recall_installed_stage(axis, stage_type, FALSE);
	}

BOOL	pi_usb_recall_installed_stage(int axis, char *stage_type, BOOL silent) {
char	temp[30];
int	axis_no;
int	counter=0;
BOOL	found_stage_type=FALSE;
FILE	*fi;

	//check on the existing file---does it exist?
	if(access(PI_USB_INSTALLED_STAGES_FILE, F_OK)!=0){
		printf("Recalling USB installed stages: %s doesn't exist (bad)\n",PI_USB_INSTALLED_STAGES_FILE);
		return FALSE;
		}
	//can we read the file?
	if(access(PI_USB_INSTALLED_STAGES_FILE, R_OK)!=0){
		printf("Recalling USB installed stages: %s can't be read (bad)\n",PI_USB_INSTALLED_STAGES_FILE);
		return FALSE;
		}

	fi=fopen(PI_USB_INSTALLED_STAGES_FILE,"r");

	//can we open the file?
	if(fi<0){
		printf("Recalling USB installed stages: %s can't be opened (bad)\n",PI_USB_INSTALLED_STAGES_FILE);
		fclose(fi);
		return FALSE;
		}

	do{
		counter++;
		fscanf(fi,"%s",temp);
		TOLOWER(temp);
		if(strstr(temp,"axis")!=NULL) {
			fscanf(fi,"%d",&axis_no);
			if (axis_no == axis) {
				fscanf(fi,"%s",stage_type);
				TOLOWER(stage_type);
				found_stage_type=TRUE;
				}
			}
		}while((counter<PI_USB_OPTIONS_LIMIT) && (strstr(temp,"end")==NULL));
	fclose(fi);
	if(found_stage_type==TRUE && silent==FALSE) {
		printf("From %s: axis %d is a %s\n",PI_USB_INSTALLED_STAGES_FILE, axis, stage_type);
		}
	return  found_stage_type;
	}

int	pi_usb_auto_stage(int axis, const char *name) {
struct	pi_usb_params stage;
int	err;
	stage.p			= PI_USB_NOT_SET;
	stage.i			= PI_USB_NOT_SET;
	stage.d			= PI_USB_NOT_SET;
	stage.vff		= PI_USB_NOT_SET;
	stage.ilimit		= PI_USB_NOT_SET;
	stage.acceleration	= PI_USB_NOT_SET;
	stage.velocity		= PI_USB_NOT_SET;
	stage.motor_mode	= PI_USB_NOT_SET;
	stage.limit_mode	= PI_USB_NOT_SET;
	stage.drive_current	= PI_USB_NOT_SET;
	stage.hold_current	= PI_USB_NOT_SET;
	stage.hold_time		= PI_USB_NOT_SET;
	stage.is_stepper	= PI_USB_NOT_SET;

	err = pi_usb_auto_read_db(&stage, name, PI_USB_AUTO_STAGE_PATH);
	if(err != PI_USB_OK) return err;

	pi_usb_auto_set_stage(axis, &stage);
	return PI_USB_OK;
	}

int	pi_usb_auto_read_db(struct pi_usb_params *stage, const char *name, const char *path){
char    filename[PI_USB_PATH_LENGTH], temp[PI_USB_OPTION_LENGTH];
int     counter = 0;
FILE    *fi;

	strncpy(filename,path, PI_USB_PATH_LENGTH);
	strncat(filename,name, PI_USB_PATH_LENGTH);

	// enforce lower case filenames!
	TOLOWER(filename);

	// look for the file
	if(access(filename, F_OK) != 0)	return PI_USB_NO_STAGE_FILE;
	if(access(filename, R_OK) != 0)	return PI_USB_CANT_READ_STAGE_FILE;

	// open file
	fi = fopen(filename, "r");
	if(fi < 0)			return PI_USB_CANT_OPEN_STAGE_FILE;

	// now go through file and look for strings
	do{
		counter++;
		fscanf(fi, "%s", temp);
		TOLOWER(temp);
		if(strstr(temp, "p-term") != NULL)		fscanf(fi, "%d", &stage->p);
		if(strstr(temp, "i-term") != NULL)		fscanf(fi, "%d", &stage->i);
		if(strstr(temp, "d-term") != NULL)		fscanf(fi, "%d", &stage->d);
		if(strstr(temp, "i-limit") != NULL)		fscanf(fi, "%d", &stage->ilimit);
		if(strstr(temp, "vff-term") != NULL)		fscanf(fi, "%d", &stage->vff);
		if(strstr(temp, "acceleration") != NULL)	fscanf(fi, "%d", &stage->acceleration);
		if(strstr(temp, "velocity") != NULL)		fscanf(fi, "%d", &stage->velocity);
		if(strstr(temp, "motor_mode") != NULL)		fscanf(fi, "%d", &stage->motor_mode);
		if(strstr(temp, "limit_mode") != NULL)		fscanf(fi, "%d", &stage->limit_mode);
		if(strstr(temp, "cpu") != NULL)			fscanf(fi, "%f", &stage->cpu);
		if(strstr(temp, "rotation_stage") != NULL)	fscanf(fi, "%d", &stage->is_rot);
		if(strstr(temp, "stepper") != NULL)		fscanf(fi, "%d", &stage->is_stepper);
		if(strstr(temp, "drive_current") != NULL)	fscanf(fi, "%d", &stage->drive_current);
		if(strstr(temp, "hold_current") != NULL)	fscanf(fi, "%d", &stage->hold_current);
		if(strstr(temp, "hold_time") != NULL)		fscanf(fi, "%d", &stage->hold_time);
		}while((counter < PI_USB_OPTIONS_LIMIT) && (strstr(temp, "end") == NULL));

	fclose(fi);
	if(counter == PI_USB_OPTIONS_LIMIT)	return PI_USB_AUTO_STAGE_ERROR;

	return PI_USB_OK;
	}

void	pi_usb_auto_set_stage(int axis, struct pi_usb_params *stage){

	if(stage->p!=PI_USB_NOT_SET)		pi_usb_send_cmd(axis, "DP", stage->p);
	if(stage->i!=PI_USB_NOT_SET)		pi_usb_send_cmd(axis, "DI", stage->i);
	if(stage->d!=PI_USB_NOT_SET)		pi_usb_send_cmd(axis, "DD", stage->d);
	if(stage->ilimit!=PI_USB_NOT_SET)	pi_usb_send_cmd(axis, "DL", stage->ilimit);
	if(stage->acceleration!=PI_USB_NOT_SET)	pi_usb_send_cmd(axis, "SA", stage->acceleration);
	if(stage->velocity!=PI_USB_NOT_SET)	pi_usb_set_vel(axis, stage->velocity);
	if(stage->cpu!=PI_USB_NOT_SET)		pi_usb_set_cpu(axis, stage->cpu);
	if(stage->is_rot!=PI_USB_NOT_SET)	pi_usb_set_rotation_stage_flag(axis, stage->is_rot);
	if(stage->is_stepper!=PI_USB_NOT_SET)	pi_usb_set_stepper_flag(axis, stage->is_stepper);
	if(stage->drive_current!=PI_USB_NOT_SET)pi_usb_send_cmd(axis, "DC", stage->drive_current);
	if(stage->hold_current!=PI_USB_NOT_SET)	pi_usb_send_cmd(axis, "HC", stage->hold_current);
	if(stage->hold_time!=PI_USB_NOT_SET)	pi_usb_send_cmd(axis, "HT", stage->hold_time);

//	if(stage->vff!=PI_USB_NOT_SET) // I don't know what vff is, for a USB controller...
//		pi_setQMC(PI_USB_SET_KVFF,		  axis, stage->vff);
//	if(stage->motor_mode!=PI_USB_NOT_SET) // again, not sure what this is for a USB controller...
//		pi_setQMC(PI_USB_SET_OUTPUT_MODE,	   axis, stage->motor_mode);

	}



/****************************************************************************
 * FUNCTIONS SPECIFIC TO CONTROLLER
 ****************************************************************************/

int	pi_usb_get_limit_status(int axis) {
char	buf[PI_USB_BUF];
char	block_five_second_character;
char	hex_str[2];
int	i, check;
int	limits_so_far = 0;
int	ret = PI_USB_LIMIT_OK;

	pi_usb_send_and_receive(axis, "TS", buf, PI_USB_BUF);
	block_five_second_character = buf[15];
	hex_str[0] = block_five_second_character;
	hex_str[1] = '\0';
	sscanf(hex_str, "%X", &i);
	check = i & 4;
	if(check == 4) {
		limits_so_far++;
		ret = PI_USB_ON_POSITIVE_LIMIT;
		}
	check = i & 8;
	if(check == 8) {
		limits_so_far++;
		ret = PI_USB_ON_NEGATIVE_LIMIT;
		}
	if(limits_so_far == 2) return PI_USB_LIMIT_BAD;
	else return ret;
	}


/* There needs to be a way of the user program to discover if the stage is a
 * rotation stage or a linear stage. This doesn't change the way the driver 
 * or user libraries work, but it will change the way a user program will
 * interact with the user, for instance the "real world unit" for linear
 * stages is the micron, it's not unusual to want to move 10cm (10000um), 
 * however it's unlikely that you're ever going to want to move 10000 degrees.
 * This info comes from the stage database (or can be set explicitly). */
void	pi_usb_set_rotation_stage_flag(int axis, int is_rot) {
	PI_USB_IS_ROT[axis] = is_rot;
	}

int	pi_usb_is_rotation_stage(int axis) {
	return PI_USB_IS_ROT[axis];
	}

void	pi_usb_set_stepper_flag(int axis, int is_stepper) {
	PI_USB_IS_STEPPER[axis] = is_stepper;
	printf("Axis %d is a stepper motor\n", axis);
	}

int	pi_usb_is_stepper(int axis) {
	return PI_USB_IS_STEPPER[axis];
	}

float	pi_usb_get_cpu(int axis) {
	return PI_USB_CPU[axis];
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
 * (TS) response, if it's a C-863 DC motor controller, or bit 1 of the same if
 * it's a C-663 stepper motor controller. Returns 1 if the motion is complete,
 * 0 otherwise */
int	pi_usb_motion_complete(int axis) {
char	buf[PI_USB_BUF];
char	block_one_second_character;
int	i, ret = 0;

	pi_usb_send_and_receive(axis, "TS", buf, PI_USB_BUF);
	if(PI_USB_IS_STEPPER[axis] == 1) {
		block_one_second_character = buf[4];
		i = (int) (block_one_second_character & (char) 2);
		//printf("\r\nbuf = %s, block_one_second_character = %c, i = %d\r\n",buf,block_one_second_character,i);
		if(i == 2) ret = 1;
		}
	else {
		block_one_second_character = buf[3];
		i = (int) (block_one_second_character & (char) 4);
		if(i == 4) ret = 1;
		}
	return ret;
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
	return pi_usb_obtain_integer_after_colon(axis, "TY"); // TY = "Tell programmed velocitY"
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

void	pi_usb_set_trigger_mode(int axis, int trigger_mode) {
	pi_usb_send_cmd(axis, "ZM", trigger_mode); // ZM = "set trigger[Z] Mode"
	}

void	pi_usb_set_trigger_pos(int axis, int pos) {
	pi_usb_send_cmd(axis, "ZP", pos); // ZP = "set trigger[Z] Position"
	}

void	pi_usb_set_trigger_pos_real(int axis, float pos_real) {
	pi_usb_set_trigger_pos(axis, pi_usb_real_to_count(axis, pos_real));
	}

void	pi_usb_set_trigger_increment(int axis, int increment) {
	pi_usb_send_cmd(axis, "ZI", increment); // ZP = "set trigger[Z] Increment"
	}

void	pi_usb_set_trigger_increment_real(int axis, float increment_real) {
	pi_usb_set_trigger_increment(axis, pi_usb_real_to_count(axis, increment_real));
	}

void	pi_usb_set_channel(int axis, int channel, int zero_or_one) {
	if(zero_or_one == 0)	pi_usb_send_cmd(axis, "CF", channel); // CF = "Channel oFF"
	else			pi_usb_send_cmd(axis, "CN", channel); // CN = "Channel oN"
	}

