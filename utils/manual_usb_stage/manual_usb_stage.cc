#include "manual_usb_stage.h"


int	main(int argc, char *argv[]) {
int	n = 0;
int	axis;
int	ret;
char	device[SERIAL_MAX_DEV_LEN];
float	initial_pos[PI_USB_MAX_CONTROLLERS];
int	restore_position;

	// first find out how many stages (axes) we've got connected, we do this by
	// looking through /dev/pi_usb[0..15]

	cout<<"USB stage control\n";
	for(axis=0; axis<PI_USB_MAX_CONTROLLERS; axis++) {
		snprintf(device, SERIAL_MAX_DEV_LEN, "/dev/pi_usb%d", axis);
		if(access(device, F_OK) == 0) {
			if((ret = pi_usb_open(device)) < 0) {
				fprintf(stderr, "Error in pi_usb_open, quitting with exit value %d\n", ret);
				return ret;
				}
			pi_usb_init(axis,FALSE);
			pi_usb_recall_pos_real(axis, TRUE, FALSE); // interactive = TRUE, silent = FALSE
			// round the position on purpose, so we don't get cumulative errors.
			initial_pos[axis] = roundf(pi_usb_get_pos_real(axis));
			n++;
			}
                }

	printf("A total of %d axes were found.\n", n);

	restore_position = move_interactive(initial_pos, n);

	for(axis=0; axis<n; axis++) {
		if(restore_position == 1) pi_usb_move_absolute_real(axis, initial_pos[axis], PI_USB_WAIT);
		pi_usb_save_pos_real(axis, FALSE); // silent = FALSE
		printf("Closing axis %d\n", axis);
		pi_usb_close(axis);
		}
	printf("Done.\n");
	}

int	move_interactive(float initial_pos[PI_USB_MAX_CONTROLLERS], int n) {
int	axis, ax, ret = 0;
int	idiot_mode = 1;
char	keypress;
int	kp;
char	inptvalue[12];
int	is_rot[PI_USB_MAX_CONTROLLERS];
float	last_pos[PI_USB_MAX_CONTROLLERS];
float	move_size[PI_USB_MAX_CONTROLLERS];
float	rot_move_size[7] = {0.1, 0.5, 1.0, 5.0, 10.0, 45.0, 90.0};
float	lin_move_size[5] = {1.0, 10.0, 100.0, 1000.0, 10000.0};
int	msi[PI_USB_MAX_CONTROLLERS]; // move size index
int	limit_status;

	// ncurses stuff
	initscr(); cbreak();
	noecho();
	nonl();
	intrflush(stdscr, 0);
	keypad(stdscr,1);
	refresh();

	cout<<"PI USB stage control\r\n";
	cout<<"(H)elp   (Q)uit   set (O)rigin"<<"\r\n";
	cout<<"A total of "<<n<<" axes are connected\r\n";
	if(n > 0) cout<<"Use normal left-right arrow keys to move axis 0"<<"\r\n";
	if(n > 1) cout<<"Use normal up-down arrow keys to move axis 1"<<"\r\n";
	if(n > 2) cout<<"Use keypad left-right arrow keys with NumLock ON to move axis 2"<<"\r\n";
	if(n > 3) cout<<"Use keypad up-down arrow keys with NumLock ON to move axis 3"<<"\r\n"<<flush;

	for(axis=0; axis<n; axis++) {
		last_pos[axis] = initial_pos[axis];
		if(pi_usb_is_rotation_stage(axis) == 1) {
			is_rot[axis] = 1;
			msi[axis] = 4;
			move_size[axis] = rot_move_size[msi[axis]];
			cout<<"Axis "<<axis<<" is a rotation stage, default move size set to "<<move_size[axis]<<" deg"<<"\r\n"<<flush;
			}
		else {
			is_rot[axis] = 0;
			msi[axis] = 3;
			move_size[axis] = lin_move_size[msi[axis]];
			cout<<"Axis "<<axis<<" is a linear stage, default move size set to "<<move_size[axis]<<" um"<<"\r\n"<<flush;
			}
		}

	for(axis=0; axis<n; axis++) {
		if((is_rot[axis] == 1) && (msi[axis] < 3))	last_pos[axis] = pi_usb_get_pos_real(axis);
		else						last_pos[axis] = roundf(pi_usb_get_pos_real(axis));
		cout<<"a"<<axis<<": "<<last_pos[axis]<<" ";
		}
	cout<<"\r\n"<<flush;

	do
	{
		kp=getch();
		switch(kp)
		{
// 			case DITHER:
// 				echo();
// 				cout<<"Input no of dither oscillations: "<<flush;
// 				getnstr(inptvalue,10);no_of_dithers=(int)strtod(inptvalue,NULL);
// 				if(no_of_dithers<0||no_of_dithers>50){
// 					cout<<"Should be >=0 and <=50\r\n";
// 					no_of_dithers=0;
// 					}
// 				pi_set_vel_umps(dither_axis,dither_vel);
// 				for(l=0;l<no_of_dithers;l++){
// 					pi_move_abs_um(dither_axis,position[dither_axis]+(float)(dither_dir*dither_dist));
// 					pi_wait_stop(dither_axis);
// 					pi_move_abs_um(dither_axis,position[dither_axis]);
// 					pi_wait_stop(dither_axis);
// 					cout<<"."<<flush;
// 					}
// 				pi_set_vel_umps(dither_axis,velocity[dither_axis]);
// 				cout<<"\r\n";
// 				noecho();
// 				break;
// 
// 			case DITHER_AXIS:
// 				orig=dither_axis;
// 				cout<<"Input axis to dither (enter 0,1,2 or 3): "<<flush;
// 				echo();
// 				kp=getch();
// 				switch(kp){
// 					case '0':
// 						dither_axis=0;
// 						break;
// 					case '1':
// 						dither_axis=1;
// 						break;
// 					case '2':
// 						dither_axis=2;
// 						break;
// 					case '3':
// 						dither_axis=3;
// 						break;
// 					default:
// 						cout<<"\r\nUnknown axis\r\n"<<flush;
// 						dither_axis=orig;
// 						break;
// 					}
// 				noecho();
// 				cout<<"\r\n";
// 				break;
// 
// 			case DITHER_DIST:
// 				orig=dither_dist;
// 				echo();
// 				cout<<"Input dither distance: "<<flush;
// 				getnstr(inptvalue,10);dither_dist=(int)strtod(inptvalue,NULL);
// 				if(dither_dist<1||dither_dist>10000){
// 					cout<<"Should be >=1 and <=10000\r\n";
// 					cout<<"Reverting back to original value of "<<orig<<"\r\n";
// 					dither_dist=orig;
// 					}
// 				noecho();
// 				break;
// 
// 			case DITHER_VELOCITY:
// 				echo();
// 				cout<<"Input dither velocity (in microns/sec): "<<flush;
// 				getnstr(inptvalue,10);dither_vel=strtod(inptvalue,NULL);
// 				if(dither_vel<10.0 || dither_vel>50000.0){
// 					cout<<"Should be >10 and <50000, reverting to 5000.\r\n";
// 					dither_vel=5000.0;
// 					}
// 				noecho();
// 				break;
// 
// 			//movements
// 			case LIMIT:
// 				cout<<"limit status: "<<pi_get_limit_status()<<"\r\n"<<flush;
// 				break;
// 
			case KEY_UP: // these definitions are from curses.h
				if(n > 1)	pi_usb_move_relative_real(1, move_size[1], PI_USB_WAIT);
				break;
			case KEY_DOWN:
				if(n > 1)	pi_usb_move_relative_real(1, -move_size[1], PI_USB_WAIT);
				break;
			case KEY_LEFT:
				if(n > 0)	pi_usb_move_relative_real(0, -move_size[0], PI_USB_WAIT);
				break;
			case KEY_RIGHT:
				if(n > 0)	pi_usb_move_relative_real(0, move_size[0], PI_USB_WAIT);
				break;

			case DET_UP_CHILLI: // for when num lock hard maps the keypad (see also note below)
				if((n > 3) && (move_size[3] >= 10000) && (idiot_mode == 1) && (is_rot[3] == 0)){
					cout<<"Oi! Sharples! No!\r\nReduce your step size before moving the detector stages!\r\n";
					cout<<"Press 'I' to turn off idiot mode, this will allow you\r\nto crash the stages much more easily.\r\n";
					break;
					}
				if(n > 3)	pi_usb_move_relative_real(3, move_size[3], PI_USB_WAIT);
				break;
			case DET_DOWN_CHILLI:
				if((n > 3) && (move_size[3] >= 10000) && (idiot_mode == 1) && (is_rot[3] == 0)){
					cout<<"Oi! Sharples! No!\r\nReduce your step size before moving the detector stages!\r\n";
					cout<<"Press 'I' to turn off idiot mode, this will allow you\r\nto crash the stages much more easily.\r\n";
					break;
					}
				if(n > 3)	pi_usb_move_relative_real(3, -move_size[3], PI_USB_WAIT);
				break;
			case DET_LEFT_CHILLI:
				if((n > 2) && (move_size[2] >= 10000) && (idiot_mode == 1) && (is_rot[2] == 0)){
					cout<<"Oi! Sharples! No!\r\nReduce your step size before moving the detector stages!\r\n";
					cout<<"Press 'I' to turn off idiot mode, this will allow you\r\nto crash the stages much more easily.\r\n";
					break;
					}
				if(n > 2)	pi_usb_move_relative_real(2, -move_size[2], PI_USB_WAIT);
				break;
			case DET_RIGHT_CHILLI:
				if((n > 2) && (move_size[2] >= 10000) && (idiot_mode == 1) && (is_rot[2] == 0)){
					cout<<"Oi! Sharples! No!\r\nReduce your step size before moving the detector stages!\r\n";
					cout<<"Press 'I' to turn off idiot mode, this will allow you\r\nto crash the stages much more easily.\r\n";
					break;
					}
				if(n > 2)	pi_usb_move_relative_real(2, move_size[2], PI_USB_WAIT);
				break;


			//change move size
			case FASTER:
				cout<<"Increase move size (\"faster\"): 'l' for linear stages, 'r' for rotation: "<<flush;
				echo();
				kp=getch();
				cout<<"\r"<<endl;
				switch(kp){
					case 'r':
						for(axis=0; axis<n; axis++) {
							if(is_rot[axis] == 1) {
								if(msi[axis] < 6) msi[axis]++;
								move_size[axis] = rot_move_size[msi[axis]];
								cout<<"\t"<<move_size[axis]<<" degrees/keypress on axis "<<axis<<"\r\n"<<flush;
								}
							}
						break;
					case 'l':
						for(axis=0; axis<n; axis++) {
							if(is_rot[axis] == 0) {
								if(msi[axis] < 4) msi[axis]++;
								move_size[axis] = lin_move_size[msi[axis]];
								cout<<"\t"<<move_size[axis]<<" microns/keypress on axis "<<axis<<"\r\n"<<flush;
								}
							}
						break;
					default:
						cout<<"Move size not changed\r\n"<<flush;
						break;
					}
				noecho();
				break;
			case SLOWER:
				cout<<"Decrease move size (\"slower\"): 'l' for linear stages, 'r' for rotation: "<<flush;
				echo();
				kp=getch();
				cout<<"\r"<<endl;
				switch(kp){
					case 'r':
						for(axis=0; axis<n; axis++) {
							if(is_rot[axis] == 1) {
								if(msi[axis] > 0) msi[axis]--;
								move_size[axis] = rot_move_size[msi[axis]];
								cout<<"\t"<<move_size[axis]<<" degrees/keypress on axis "<<axis<<"\r\n"<<flush;
								}
							}
						break;
					case 'l':
						for(axis=0; axis<n; axis++) {
							if(is_rot[axis] == 0) {
								if(msi[axis] > 0) msi[axis]--;
								move_size[axis] = lin_move_size[msi[axis]];
								cout<<"\t"<<move_size[axis]<<" microns/keypress on axis "<<axis<<"\r\n"<<flush;
								}
							}
						break;
					default:
						cout<<"Move size not changed\r\n"<<flush;
						break;
					}
				noecho();
				break;
			//commands
			case ORIGIN:
				cout<<"Set origin: 'd' to set detector origin (axes 2 & 3),\r\n"<<flush;
				cout<<"            's' to set sample origin (axes 0 & 1),\r\n"<<flush;
				cout<<"            'a' for all axes, '0' to 'F' (caps A-F) for those axes,\r\n"<<flush;
				cout<<"            any other key does not set the origin.... : "<<flush;
				echo();
				kp=getch();
				cout<<"\r\t"<<endl;
				switch(kp){
					case 'a':
						for(axis=0; axis<n; axis++)	pi_usb_set_origin(axis);
						cout<<"Origin set on all axes"<<"\r\n";
						break;
					case 'd':
						if(n > 2)			pi_usb_set_origin(2);
						if(n > 3)			pi_usb_set_origin(3);
						cout<<"Origin set on axes 2 and 3 (detector)"<<"\r\n";
						break;
					case 's':
						if(n > 0)			pi_usb_set_origin(0);
						if(n > 1)			pi_usb_set_origin(1);
						cout<<"Origin set on axes 0 and 1 (sample)"<<"\r\n";
						break;
					case '0': if(n > 0) { pi_usb_set_origin(0); cout<<"Origin set on axis 0 (sample)"<<"\r\n"; }
						break;
					case '1': if(n > 1) { pi_usb_set_origin(1); cout<<"Origin set on axis 1 (sample)"<<"\r\n"; }
						break;
					case '2': if(n > 2) { pi_usb_set_origin(2); cout<<"Origin set on axis 2 (detector)"<<"\r\n"; }
						break;
					case '3': if(n > 3) { pi_usb_set_origin(3); cout<<"Origin set on axis 3 (detector)"<<"\r\n"; }
						break;
					case '4': if(n > 4) { pi_usb_set_origin(4); cout<<"Origin set on axis 4"<<"\r\n"; }
						break;
					case '5': if(n > 5) { pi_usb_set_origin(5); cout<<"Origin set on axis 5"<<"\r\n"; }
						break;
					case '6': if(n > 6) { pi_usb_set_origin(6); cout<<"Origin set on axis 6"<<"\r\n"; }
						break;
					case '7': if(n > 7) { pi_usb_set_origin(7); cout<<"Origin set on axis 7"<<"\r\n"; }
						break;
					case '8': if(n > 8) { pi_usb_set_origin(8); cout<<"Origin set on axis 8"<<"\r\n"; }
						break;
					case '9': if(n > 9) { pi_usb_set_origin(9); cout<<"Origin set on axis 9"<<"\r\n"; }
						break;
					case 'A': if(n > 10) { pi_usb_set_origin(10); cout<<"Origin set on axis 10"<<"\r\n"; }
						break;
					case 'B': if(n > 11) { pi_usb_set_origin(11); cout<<"Origin set on axis 11"<<"\r\n"; }
						break;
					case 'C': if(n > 12) { pi_usb_set_origin(12); cout<<"Origin set on axis 12"<<"\r\n"; }
						break;
					case 'D': if(n > 13) { pi_usb_set_origin(13); cout<<"Origin set on axis 13"<<"\r\n"; }
						break;
					case 'E': if(n > 14) { pi_usb_set_origin(14); cout<<"Origin set on axis 14"<<"\r\n"; }
						break;
					case 'F': if(n > 15) { pi_usb_set_origin(15); cout<<"Origin set on axis 15"<<"\r\n"; }
						break;
					default:
						cout<<"Origin not set\r\n"<<flush;
						break;
					}
				noecho();
				break;
//			case ABORT:
//				pi_stop_dead();
//				cout<<"Movement aborted"<<"\r\n";
//				break;
//
			case VELOCITY:
				cout<<"Input axis to set velocity on (enter 0-9 or A-F): "<<flush;
				echo();
				kp=getch();
				switch(kp){
					case '0': axis = 0; break;
					case '1': axis = 1; break;
					case '2': axis = 2; break;
					case '3': axis = 3; break;
					case '4': axis = 4; break;
					case '5': axis = 5; break;
					case '6': axis = 6; break;
					case '7': axis = 7; break;
					case '8': axis = 8; break;
					case '9': axis = 9; break;
					case 'A': axis = 10; break;
					case 'B': axis = 11; break;
					case 'C': axis = 12; break;
					case 'D': axis = 13; break;
					case 'E': axis = 14; break;
					case 'F': axis = 15; break;
					default:
						cout<<"\r\nUnknown axis\r\n"<<flush;
						axis =- 1;
						break;
					}
				if(axis == -1) break;
				echo();
				if(is_rot[axis] == 0)	cout<<"\r\nInput velocity (in microns/sec): "<<flush;
				else			cout<<"\r\nInput velocity (in degrees/sec): "<<flush;
				getnstr(inptvalue, 10);
				if(n > axis) pi_usb_set_vel_real(axis, strtof(inptvalue, NULL));
				noecho();
				break;

			case MANUAL:
				cout<<"Input axis to move (enter 0,1,2 or 3): "<<flush;
				echo();
				kp=getch();
				switch(kp){
					case '0': axis = 0; break;
					case '1': axis = 1; break;
					case '2': axis = 2; break;
					case '3': axis = 3; break;
					case '4': axis = 4; break;
					case '5': axis = 5; break;
					case '6': axis = 6; break;
					case '7': axis = 7; break;
					case '8': axis = 8; break;
					case '9': axis = 9; break;
					case 'A': axis = 10; break;
					case 'B': axis = 11; break;
					case 'C': axis = 12; break;
					case 'D': axis = 13; break;
					case 'E': axis = 14; break;
					case 'F': axis = 15; break;
					default:
						cout<<"\r\nUnknown axis\r\n"<<flush;
						axis =- 1;
						break;
					}
				if(axis == -1) break;
				if(is_rot[axis] == 0)	cout<<"\r\nInput relative distance in microns: "<<flush;
				else			cout<<"\r\nInput relative distance in degrees: "<<flush;
				getnstr(inptvalue, 10);
				if(n > axis) pi_usb_move_relative_real(axis, strtof(inptvalue, NULL), PI_USB_WAIT);
				noecho();
				break;

			case GOHOME:
				cout<<"All connected axes going home to (0,0,0,0,...)"<<"\r\n";
				for(axis=0; axis<n; axis++) pi_usb_move_absolute(axis, 0, PI_USB_WAIT);
				break;

			case IDIOT_OFF:
				cout<<"Idiot mode is now turned off, you will now\r\n";
				cout<<"be able to move the detector stages by upto\r\n";
				cout<<"10mm at a time (rather than 1mm)... be careful!\r\n";
				idiot_mode = 0;
				break;
			case IDIOT_ON:
				cout<<"Idiot mode is now turned on, you will now\r\n";
				cout<<"NOT be able to move the detector stages by any\r\n";
				cout<<"more than 1mm at a time...\r\n";
				idiot_mode = 1;
				break;

			case HELP:
				cout<<"*** Move interactive help page***\r\n\n"<<flush;
				cout<<"Make sure Caps Lock is OFF and Num Lock is ON"<<"\r\n"<<endl;
				cout<<"Axes 2 and 3, use arrow keys on number pad, with Num Lock ON"<<"\r\n";
				cout<<"Axes 0 and 1, use normal arrow keys (left/right = axis 0)"<<"\r\n"<<endl;
				cout<<"Faster (increase step size)   : "<<FASTER<<"\r\n";
				cout<<"Slower (decrease step size)   : "<<SLOWER<<"\r\n";
				cout<<"Set origin to present location: "<<ORIGIN<<"\r\n";
				cout<<"Goto current origin           : "<<GOHOME<<"\r\n";
				cout<<"Set velocity (in microns/sec) : "<<VELOCITY<<"\r\n";
				cout<<"Move manually (by typing)     : "<<MANUAL<<"\r\n";
//				cout<<"Abort movement                : <spacebar>"<<"\r\n";
				cout<<"Idiot mode off (big det moves): "<<IDIOT_OFF<<"\r\n";
				cout<<"Idiot mode on (1mm det moves) : "<<IDIOT_ON<<"\r\n";
//				cout<<"Dither (move back and forth)  : "<<DITHER<<"\r\n";
//				cout<<"Dither axis (default 1)       : "<<DITHER_AXIS<<"\r\n";
//				cout<<"Dither distance (default 1000): "<<DITHER_DIST<<"\r\n";
//				cout<<"Dither velocity (default 5000): "<<DITHER_VELOCITY<<"\r\n";
				cout<<"Quit                          : "<<QUIT<<"\r\n"<<endl;
				break;
// NOTE ADDED sds 23/11/01
// since stages 1+2 are bound to the keypad keys we need to handle these as
// escape sequences, but only when controlling them from duvet. dunno if this
// is a 'modern keyboard' thing (ie the num lock key no longer hard maps keypad
// numbers to normal keyboard numbers) or whether is a communication via telnet/
// ssh thing. In any case, the code below catches the escape sequences.

// on pressing these keys ESCAPE is sent (27) followed by another character (79)
// then the key code for the appropriate key, I don't know why it just is.....
			case ESCAPE:
				kp=getch(); //read in the next character (79)
				kp=getch();
				if((n > 3) && (move_size[3] >= 10000) && (idiot_mode == 1) && (is_rot[3] == 0)){
					cout<<"Oi! Sharples! No!\r\nReduce your step size before moving the detector stages!\r\n";
					cout<<"Press 'I' to turn off idiot mode, this will allow you\r\nto crash the stages much more easily.\r\n";
					break;
					}
				switch(kp)
				{
					case DET_DOWN:
						if(n > 3)	pi_usb_move_relative_real(3, -move_size[3], PI_USB_WAIT);
						break;
					case DET_UP:
						if(n > 3)	pi_usb_move_relative_real(3, move_size[3], PI_USB_WAIT);
						break;
					case DET_LEFT:
						if(n > 2)	pi_usb_move_relative_real(2, -move_size[2], PI_USB_WAIT);
						break;
					case DET_RIGHT:
						if(n > 2)	pi_usb_move_relative_real(2, move_size[2], PI_USB_WAIT);
						break;
					default:
						break;
					}
				break;
			default:
				break;
		}

// Note to sds 10/6/10: need to sort out limit switches, on the usb driver generally. Not relevant for rotation stage though.

//		for(axis=0;axis<4;axis++) {
//			if(got_stage[axis]==TRUE) {
//				limit_status=pi_get_limit_status(axis);
//				if(limit_status==PI_ON_POSITIVE_LIMIT || limit_status==PI_ON_NEGATIVE_LIMIT) {
//					cout<<"PROBLEM: Axis "<<axis<<" is on a limit switch at "<<pi_get_pos_um(axis)<<"! Moving off...\r\n"<<flush;
//					pi_move_off_limit_switch(axis);
//					pi_set_vel_umps(axis,velocity[axis]);
//					}
//				}
//			}

		for(axis=0; axis<n; axis++) {
			limit_status = pi_usb_get_limit_status(axis);
			if((limit_status ==  PI_USB_ON_POSITIVE_LIMIT) || (limit_status ==  PI_USB_ON_NEGATIVE_LIMIT)) {
				cout<<"LIMIT SWITCH reached on axis "<<axis<<" at "<<pi_usb_get_pos_real(axis)<<", moving back to "<<last_pos[axis]<<"\r\n"<<flush;
				pi_usb_move_absolute_real(axis, last_pos[axis], PI_USB_WAIT);
				}
			}

		for(axis=0; axis<n; axis++) {
			if((is_rot[axis] == 1) && (msi[axis] < 3))	last_pos[axis] = pi_usb_get_pos_real(axis);
			else						last_pos[axis] = roundf(pi_usb_get_pos_real(axis));
			cout<<"a"<<axis<<": "<<last_pos[axis]<<" ";
			}
		cout<<"\r\n"<<flush;

		} while(kp!=QUIT);

	cout<<"Caution: return to initial position ( ";
	for(axis=0; axis<n; axis++) cout<<"a"<<axis<<": "<<initial_pos[axis]<<" ";
	cout<<") (y/n) ?"<<flush;
	do {
		keypress = getchar();
		if(keypress == 'y') {
			ret = 1;
			keypress = 'n';
			}
		} while(keypress != 'n');

	cout<<"\r\n";
	// ncurses stuff
	nocbreak();
	echo();
	endwin();
	return ret;
	}

