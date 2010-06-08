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
