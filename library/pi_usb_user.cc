#include "pi_usb_user.h"


/****************************************************************************
 * OPEN
 ****************************************************************************/
int	pi_usb_open(const char *tty) {
	return serial_open(tty, 9600); // default baud rate
	}

/****************************************************************************
 * CLOSE
 ****************************************************************************/
int	pi_usb_close(int fd) {
	return serial_close(fd);
	}

/****************************************************************************
 * SEND
 ****************************************************************************/
int	pi_usb_send(int fd, const char *cmd) {
	return serial_send(fd, cmd, "\r", 1);
	}

int	pi_usb_send_raw(int fd, const char *cmd, int len) {
	return serial_send_raw(fd, cmd, len);
	}


/****************************************************************************
 * RECEIVE
 ****************************************************************************/
int	pi_usb_receive(int fd, char *buf,int buf_len) {
	return serial_receive(fd, buf, buf_len, (const char) 3, (const char) '\r');
	}

/****************************************************************************
 * UTILITY FUCTIONS (send and receive, obtain value etc
 ****************************************************************************/
int	pi_usb_send_and_receive(int fd, const char *cmd, char *buf, int buf_len) {
int	send_ret, receive_ret;

	send_ret = pi_usb_send(fd, cmd);
	if(send_ret != SERIAL_OK) {
		fprintf(stderr, "Error on send, returning %d\n", send_ret);
		return send_ret;
		}

	receive_ret = pi_usb_receive(fd, buf, buf_len);
	if(receive_ret != SERIAL_OK) {
		//fprintf(stderr, "Error on receive, returning %d\n", get_ret);
		return receive_ret;
		}
	return receive_ret;
	}

/* Probes bit 2 of the second character of the first block of the "Tell Status"
 * (TS) response. Returns 1 if the motion is complete, 0 otherwise */
int	pi_usb_motion_complete(int fd) {
char	buf[PI_USB_BUF];
char	block_one_second_character;
int	i;

	pi_usb_send_and_receive(fd, "TS\r", buf, PI_USB_BUF);
	block_one_second_character = buf[3];
	i = (int) (block_one_second_character & (char) 4);
	if(i == 4) return 1;
	else return 0;
	}

/* Sits in a loop waiting for the motion to be complete */
void	pi_usb_wait_motion_complete(int fd) {
	pi_usb_wait_motion_complete(fd, PI_USB_DEFAULT_USLEEP);
	}

void	pi_usb_wait_motion_complete(int fd, useconds_t usleep_time) {
int	ret, done = 0;
	do{
		if(pi_usb_motion_complete(fd) == 1) done = 1;
		else usleep(usleep_time);
		} while(done == 0);
	}

