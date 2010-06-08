#ifndef __PI_USB_USER__
#define __PI_USB_USER__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define PI_USB_LOCKFILE "/etc/pi_stage/usb_lockfile"
#define PI_USB_LOCKFILEDIR "/etc/pi_stage/"

#define	PI_USB_DEFAULT_USLEEP				100000

#define PI_USB_NOOPEN					21
#define PI_USB_NOATTR					20

#define	PI_USB_LOCKFILE_PROCESS_CONSISTENTLY_RUNNING	12
#define	PI_USB_CANNOT_WRITE_LOCKFILE			11
#define	PI_USB_CANNOT_WRITE_ERRORLOG_FILE		10

#define	PI_USB_WRITE_ERROR				5

#define	PI_USB_READ_SELECT_ERROR			4
#define	PI_USB_REPEATED_TIMEOUTS			3
#define	PI_USB_REPEATED_EMPTY_DATA			2
#define	PI_USB_EMPTY_DATA				1

#define PI_USB_OK					0

int	pi_usb_open(const char *tty);
int	pi_usb_close(void);
int	pi_usb_send(const char *cmd);
int	pi_usb_send(const char *cmd, int len);
int	pi_usb_receive(char *buf, int len);
int	pi_usb_send_and_receive(const char *cmd, char *buf, int buf_len);
int	pi_usb_motion_complete(void);
void	pi_usb_wait_motion_complete(void);
void	pi_usb_wait_motion_complete(useconds_t usleep_time);
void	pi_usb_create_lockfile(void);
void	pi_usb_release_lockfile(void);

//time_t	rawtime;
//char	human_time[50];

//char 	*asctime(const struct tm *timeptr);

#define PI_USB_BUF 128
#define TRUE 1
#define FALSE 0

#endif
