#include "pi_usb_user.h"

int	usb_dev; /* file descriptor of the usb serial port */

/****************************************************************************
 * OPEN
 ****************************************************************************/
int	pi_usb_open(const char *tty) {
char	device[100];
struct	termios options;

	pi_usb_create_lockfile();
    	if(tty[0]=='/') {
		sprintf(device, "%s", tty);
		}
	else {
		sprintf(device, "/dev/%s", tty);
		}
	if((usb_dev = open(device, O_RDWR)) < 0) {
		fprintf(stderr, "pi_usb_open: could not open device %s, ", device);
		fprintf(stderr, "the error is %d\n", usb_dev);
		if(usb_dev == -1) {
			fprintf(stderr, "Have you set the access permissions?\n");
			fprintf(stderr, "sudo chmod 666 %s\n", device);
			}
		return PI_USB_NOOPEN;
		}
	cfmakeraw(&options);
	options.c_cflag	= B9600|CS8|CLOCAL|CREAD;	//Baud, character size mask,Ignore modem control lines,Enable reciever. This is very important it doesn't work without it
	options.c_cc[VTIME]	= 0; //Timeout in deciseconds for non-canonical read
	options.c_cc[VMIN]	= 0;
	if(tcsetattr(usb_dev, TCSANOW, &options) < 0){
		fprintf(stderr, "pi_usb_open: Couldn't set the usb serial port attributes.\n");
		return PI_USB_NOATTR;
		}
	fcntl(usb_dev, F_SETFL, O_NONBLOCK);	       /* set up non-blocking read */
	return PI_USB_OK;
	}

/****************************************************************************
 * CLOSE
 ****************************************************************************/
int	pi_usb_close(void) {
	close(usb_dev);
	pi_usb_release_lockfile();
	return PI_USB_OK;
	}

/****************************************************************************
 * SEND
 ****************************************************************************/
int	pi_usb_send(const char *cmd){
	return pi_usb_send(cmd, strlen(cmd));
	}

int	pi_usb_send(const char *cmd, int len){
struct	timeval Timeout;
fd_set	writefs;
int	ret;

	Timeout.tv_usec = 0;  /* microseconds */
	Timeout.tv_sec  = 1;  //zero just hangs
	FD_ZERO(&writefs);
	FD_SET(usb_dev, &writefs);

	ret = select(usb_dev+1, (fd_set *)0, &writefs,(fd_set *)0, &Timeout);
	if(ret <= 0) {
		fprintf(stderr, "SERIAL_set: select on WRITE return value = %d (should be >0).\n", ret);
		return PI_USB_WRITE_ERROR;
		}
	write(usb_dev, (void *)cmd, len);
	FD_ZERO(&writefs);
	FD_SET(usb_dev, &writefs);
	ret = select(usb_dev+1, (fd_set *)0, &writefs,(fd_set *)0, &Timeout);
	if(ret <= 0) {
		fprintf(stderr, "SERIAL_set: select on WRITE return value = %d (should be >0).\n", ret);
		return PI_USB_WRITE_ERROR;
		}
	write(usb_dev, (void *)"\r", 2);
	return PI_USB_OK;
	}


/****************************************************************************
 * RECEIVE
 ****************************************************************************/
int	pi_usb_receive(char *buf,int buf_len) {

ssize_t	bytes_read;
ssize_t	total_bytes_read;
int	res;
int	i;
int	all_done;
int	hold_up;
struct timeval Timeout;
fd_set	readfs;
	total_bytes_read = 0;

	i = 0;
	hold_up = 0;
	do {
		bytes_read = 0;
		all_done = 0;
		Timeout.tv_usec = 300000;	/* microseconds */
		Timeout.tv_sec  = 0;		/* seconds */

		FD_ZERO(&readfs);
		FD_SET(usb_dev,&readfs);

		res = select(usb_dev+1, &readfs, (fd_set *)0,(fd_set *)0, &Timeout);

		if (res<0) {
			fprintf(stderr, "Serious error: select returned %d (< 0), returning %d\n", i, PI_USB_READ_SELECT_ERROR);
			return PI_USB_READ_SELECT_ERROR;
			}

		if (res==0) {
			//fprintf(stderr, "Timed out\n");
			hold_up++;
			}
		else{
			bytes_read = read(usb_dev,(void *)(buf + i), buf_len - i);
			total_bytes_read += bytes_read;
			//fprintf(stderr, "%d bytes read this time, %d bytes read so far\n", bytes_read, total_bytes_read);
			i = i + bytes_read;
			}
		} while (buf[i-1] != (unsigned char) 3 && i < buf_len && hold_up<10 && bytes_read != 0);
	// The PI Mercury controller sends a termination sequence of "\r\n\0x03" at then end of the message (ascii 03 is "ETX")
	// We look for "03" to find out when we're done.

//	printf("\ni = %d\n",i);
//	fprintf(stderr, "%d bytes read in total, which were: \n",(int)total_bytes_read);
//	for(i=0;i<(int)total_bytes_read;i++){
//		if(tmp_buff[i]>31)fprintf(stderr, "%c:",tmp_buff[i]);
//		else fprintf(stderr, "[%d]:",(int)tmp_buff[i]);
//		fflush(stdout);
//		}
//	fprintf(stderr, "\n");

	// This is relatively common, and can happen for a variety of reasons. Closing the serial
	// port and reopening it again usually fixes it.
	if(bytes_read == 0) {
		//fprintf(stderr, "Timed out, and have not read any data (0 bytes). Returning with -3\n");
		return PI_USB_EMPTY_DATA;
		}
	i = 0;
	do (1);
	while (buf[i++] != '\r' && i<buf_len);
	if(i>=buf_len) {
		fprintf(stderr, "Error: couldn't find CR in buf\n");
		}

	buf[i - 1]='\0'; // overwrite the \r

	if(hold_up == 10) {
		fprintf(stderr, "Timed out 10 times, although there was still data coming in.\nVery odd! Returning %d\n", PI_USB_REPEATED_TIMEOUTS);
		return PI_USB_REPEATED_TIMEOUTS;
		}

	return PI_USB_OK;
	}

/****************************************************************************
 * UTILITY FUCTIONS (send and receive, obtain value etc
 ****************************************************************************/
int	pi_usb_send_and_receive(const char *cmd, char *buf, int buf_len) {
int	set_ret, get_ret;

	set_ret = pi_usb_send(cmd);
	if(set_ret != 0) {
		fprintf(stderr, "Error on send, returning %d\n", set_ret);
		return set_ret;
		}

	get_ret = pi_usb_receive(buf, buf_len);
	if(get_ret != 0) {
		//fprintf(stderr, "Error on receive, returning %d\n", get_ret);
		return get_ret;
		}
	return PI_USB_OK;
	}

/* Probes bit 2 of the second character of the first block of the "Tell Status"
 * (TS) response. Returns 1 if the motion is complete, 0 otherwise */
int	pi_usb_motion_complete(void) {
char	buf[PI_USB_BUF];
char	block_one_second_character;
int	i;

	pi_usb_send_and_receive("TS\r", buf, PI_USB_BUF);
	block_one_second_character = buf[3];
	i = (int) (block_one_second_character & (char) 4);
	if(i == 4) return 1;
	else return 0;
	}

/* Sits in a loop waiting for the motion to be complete */
void	pi_usb_wait_motion_complete(void) {
	pi_usb_wait_motion_complete(PI_USB_DEFAULT_USLEEP);
	}

void	pi_usb_wait_motion_complete(useconds_t usleep_time) {
int	ret, done = 0;
	do{
		if(pi_usb_motion_complete() == 1) done = 1;
		else usleep(usleep_time);
		} while(done == 0);
	}
		


/****************************************************************************
 * LOCKFILE
 ****************************************************************************/
void	pi_usb_create_lockfile(void) {
FILE	* temp;
char	pidname[200];
int	id, count = 0;

	if(access(PI_USB_LOCKFILE, F_OK) == 0){
		temp=fopen(PI_USB_LOCKFILE, "r");
		fscanf(temp, "%d", &id);
		fclose(temp);
		sprintf(pidname, "/proc/%d", id);
		while (count <= 3 && access(pidname, F_OK)==0) {
			sleep(1);
			count++;
			}	
		if (count >= 3) {
			fprintf(stderr, "Lockfile %s found.\n", PI_USB_LOCKFILE);
			fprintf(stderr, "It is being held by process with PID %d, which is still running.\n", id);
			fprintf(stderr, "Cannot release lock, quitting with exit code %d.\n", PI_USB_LOCKFILE_PROCESS_CONSISTENTLY_RUNNING);
			exit(PI_USB_LOCKFILE_PROCESS_CONSISTENTLY_RUNNING);
			}
		}

	if(access(PI_USB_LOCKFILEDIR,W_OK)!=0){
		fprintf(stderr, "No lock file found (good); however, cannot write to directory %s\n", PI_USB_LOCKFILEDIR);
		fprintf(stderr, "Quitting with exit code %d\n", PI_USB_CANNOT_WRITE_LOCKFILE);
		exit(PI_USB_CANNOT_WRITE_LOCKFILE);
		}
	temp = fopen(PI_USB_LOCKFILE, "w");
	id = getpid();
	fprintf(temp, "%d\n", (int)id);
	fclose(temp);
	}

void	pi_usb_release_lockfile(void) {
	unlink(PI_USB_LOCKFILE);
	}

