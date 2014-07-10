#ifndef MANUAL_USB_STAGE_H
#define MANUAL_USB_STAGE_H

#include "pi_usb_user.h"

//direction chars
#define DITHER		'D'
#define DITHER_AXIS	'A'
#define DITHER_DIST	'd'
#define DITHER_VELOCITY	'V'
#define DET_UP		'x'
#define DET_DOWN	'r'
#define DET_LEFT	't'
#define DET_RIGHT	'v'
#define DET_UP_CHILLI	'8'
#define DET_DOWN_CHILLI	'2'
#define DET_LEFT_CHILLI	'4'
#define DET_RIGHT_CHILLI '6'
#define ESCAPE		27
#define OPEN_SQUARE_BRACKET	91
#define SAM_UP		KEY_UP
#define SAM_DOWN	KEY_DOWN
#define SAM_LEFT	KEY_LEFT
#define SAM_RIGHT	KEY_RIGHT

//command chars
#define VELOCITY	'v'
#define SHOW_POS	'p'
#define ORIGIN		'o'
#define ABORT		' '
#define QUIT		'q'
#define FASTER		'f'
#define SLOWER		's'
#define HELP		'h'
#define LIMIT		'l'
#define UNITS		'u'
#define MANUAL		'm'
#define GOHOME		'g'
#define IDIOT_OFF	'I'
#define IDIOT_ON	'i'

#define	DEFAULT_MOVE_SIZE_LIN	100.0
#define	DEFAULT_MOVE_SIZE_ROT	10.0

int move_interactive(float *, int n);

#endif
