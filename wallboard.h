
#include <windows.h>

void printWindowsError(DWORD errorCode);
HANDLE openport(char * port);
BOOL SendWallMessage(HANDLE port, char *sayit, char pos, unsigned char style, char col, unsigned char special,  bool dumppkt);

// Colours
#define COL_RED		 '1'
#define COL_GREEN	 '2'
#define COL_AMBER	 '3'
#define COL_RAINBOW1 '9'
#define COL_RAINBOW2 'A'
#define COL_MIX		 'B'
#define COL_AUTO	 'C'

// Vertical positions
#define pos_middle 0x20
#define pos_top    0x22
#define pos_bottom 0x26
#define pos_fill   0x30

// 'Special' modes
#define SPECIAL_TWINKLE			0x30
#define SPECIAL_SPARKLE			0x31
#define SPECIAL_SNOW			0x32
#define SPECIAL_INTERLOCK		0x33
#define SPECIAL_SWITCH			0x34
#define SPECIAL_SLIDE			0x35
#define SPECIAL_SPRAY			0x36
#define SPECIAL_STARBURST		0x37
#define SPECIAL_WELCOME			0x38
#define SPECIAL_SLOT_MACHINE	0x39
#define SPECIAL_THANK_YOU		0x53
#define SPECIAL_NO_SMOKING		0x55
#define SPECIAL_DRINK_DRIVE		0x56
#define SPECIAL_RUNNING_ANIMAL	0x57
#define SPECIAL_FIREWORKS		0x58
#define SPECIAL_TURBO_CAR		0x59
#define SPECIAL_CHERRY_BOMB		0x5A

// this SPECIAL flag that indicates no special effect - it is not a valid wallboard code!
#define SPECIAL_NONE 0xff

// Styles
#define MODE_ROTATE	0x61
#define MODE_HOLD	0x62
#define MODE_FLASH  0x63
// 0x64 is reserved
#define MODE_ROLL_UP	0x65
#define MODE_ROLL_DOWN	0x66
#define MODE_ROLL_LEFT	0x67
#define MODE_ROLL_RIGHT	0x68
#define MODE_WIPE_UP	0x69
#define MODE_WIPE_DOWN	0x6a
#define MODE_WIPE_LEFT	0x6b
#define MODE_WIPE_RIGHT	0x6c
#define MODE_SCROLL		0x6d
// 0x6e is reserved
#define MODE_RANDOM		0x6f
#define MODE_ROLL_IN	0x70
#define MODE_ROLL_OUT	0x71
#define MODE_WIPE_IN	0x72
#define MODE_WIPE_OUT	0x73
#define MODE_COMPRESSED_ROTATE	0x74

