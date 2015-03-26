// wallboard.cpp : Defines the entry point for the console application.

#include <windows.h>
#include "wallboard.h"
#include <stdio.h>

// Functions we will only call from this source file
unsigned char parsePositionArg(char* charIn);
unsigned char parseColourArg(char* colourArg);
unsigned char parseFromArray(char* human[], unsigned char wallboard[], unsigned int optionCount, char* toParse);
unsigned char parseStyleArg(char* styleArg);
unsigned char parseSpecialArg(char* specialArg);
void printModeNames();
void printSpecialNames();
void printAll(char** arrayToPrint);
void doUsage(void);

int main(int argc, char* argv[])
{
	HANDLE porthnd;

	// set some sensible defaults
	char* portname = "COM1";
	char* message  = "Hello world";
	bool dump = FALSE;
	unsigned char position = pos_fill;
	unsigned char col = COL_RED;
	unsigned char mode = 0;
	unsigned char special = SPECIAL_NONE;

	// parse commandline
	int n = 1;
	while(n < argc)
	{
		if (0==_strcmpi(argv[n],"-h") || 0==_strcmpi(argv[n],"-?") || 0==_strcmpi(argv[n],"-help") || 0==_strcmpi(argv[n],"--help") || 0==_strcmpi(argv[n],"/?") ) 
		{
			doUsage();
			exit(0);
		}
		if (0==_stricmp(argv[n],"-d"))
		{
			dump=TRUE;
			n++;
			continue;
		}

		if (n<argc-1)	// only look for two-part args if theres another after this one
		{
			if (0==_stricmp(argv[n],"-port")) 
			{
				portname=argv[n+1];
				n += 2;
				continue;
			}
			if (0==_stricmp(argv[n],"-msg"))  
			{
				message=argv[n+1];
				n += 2;
				continue;
			}

			if (0==_stricmp(argv[n],"-pos" )) 
			{
				position = parsePositionArg(argv[n+1]);
				if (FALSE == position)
				{
					printf("Failed to parse position argument '%s'\n", argv[n+1]);
					exit(-1);
				}
				n += 2;
				continue;
			}
			if (0==_stricmp(argv[n],"-col" )) 
			{
				col = parseColourArg(argv[n+1]);
				if (FALSE == col)
				{
					printf("Failed to parse colour argument '%s'\n", argv[n+1]);
					exit(-1);
				}
				n += 2;
				continue;
			}
			if (0==_stricmp(argv[n],"-style" )) 
			{
				mode = parseStyleArg(argv[n+1]);
				if (FALSE == mode)
				{
					printf("Failed to parse style argument '%s'\n", argv[n+1]);
					exit(-1);
				}
				n += 2;
				continue;
			}
			if (0==_stricmp(argv[n],"-special" )) 
			{
				special = parseSpecialArg(argv[n+1]);
				if (FALSE == special)
				{
					printf("Failed to parse special argument '%s'\n", argv[n+1]);
					exit(-1);
				}
				n += 2;
				continue;
			}

		}
		// Nothing matched!
		printf("Failed to parse argument '%s'\n", argv[n]);
		doUsage();
		exit(-1);
	}

	// Okay great, now it's time to actually do the business.
	porthnd = openport(portname);

	if (FALSE == porthnd) 
	{ 
		printWindowsError(GetLastError());
		return 1; 
	}

	if (!SendWallMessage(porthnd, message, position, mode, col, special, dump))
		printf("Unable to send message\n");

	CloseHandle(porthnd);
	return 0;
}

void doUsage(void)
{
	printf("Commandline wallboard test tool\n");
	printf("Usage: cmdline.exe ( -port [portname] ) (-d) (-h) (-pos [pos]) (-col [red|green|amber|rainbow1|rainbow2|mix|auto]) (-style [one listed style]) (-special [one listed special style]) (-msg message) \n");
	printf("  -port [portname] : set port to use, default COM1\n");
	printf("  -d : dump assembled packet to stdout\n");
	printf("  -h : this text\n");
	printf("  -pos [middle|top|bottom|fill] : position of text vertically\n");
	printf("  -col [red|green|amber|rainbow1|rainbow2|mix|auto] : set initial colour of text.\n");
	printf("		rainbow1: stripes the display into colours horizontally\n");
	printf("		rainbow2: stripes the display into colours diagonally\n");
	printf("		mix		: sets each character to a different colour\n");
	printf("		auto	: selects randomly\n");
	printf("  -style (style) - pick from one of:\n");
	printModeNames();
	printf("  -special (special style) - overrides style with a 'special style'. Chose from:\n");
	printSpecialNames();
	printf("Note that 'style' is ignored when a 'special' is specified.\n");
	printf("  -msg [msg] : message to send (don't forget quotes if it involves spaces!)\n");
	printf("\n");
	printf("Brackets indicate optional segments.\n");
	printf("Port is assumed to be 9600baud, 7E2.\n");
	printf("The following control characters can also be used -\n");
	printf("	CTRL-Q	: Disable wide characters\n");
	printf("	CTRL-R	: Enable wide characters\n");
	printf("\n");
	printf("For example:\n");
	printf("	cmdline.exe -port com3 -p top -d -col mix -style flash -msg \"This is a test.\"");
}

// Parse a human-readable position string into a wallboard-readable position code.
// Return FALSE on failure.
unsigned char parsePositionArg(char* argIn)
{
	// Args we understand, and their wallboard equivalents
	char* humanArgs[]				= {"middle",	 "top",	  "bottom",	  "fill"};
	unsigned char wallboardArgs[]	= { pos_middle,  pos_top, pos_bottom, pos_fill };
	return parseFromArray(humanArgs, wallboardArgs, sizeof(wallboardArgs), argIn);
}

unsigned char parseColourArg(char* colourArg)
{
	char* humanArgs[]				= {"red",	 "green",	"amber",	 "rainbow1", "rainbow2",   "mix",   "auto" };
	unsigned char wallboardArgs[]	= { COL_RED, COL_GREEN, COL_AMBER, COL_RAINBOW1, COL_RAINBOW2, COL_MIX, COL_AUTO };
	return parseFromArray(humanArgs, wallboardArgs, sizeof(wallboardArgs), colourArg);
}

// This is a NULL-terminated list of special mode names. If you add, remove, or change the order, you must make the corresponding chant in 
// parseSpecialArg.
static char* specialNames[] = {
		"twinkle", "sparkle", "snow", "interlock", "switch", 
		"slide", "spray", "starburst", "welcome", "slotMachine",
		"thankYou", "noSmoking", "drinkDrive", "runningAnimal", "fireworks",
		"turboCar", "cherryBomb"
	};

unsigned char parseSpecialArg(char* specialArg)
{
	char** humanArgs				= specialNames;

	unsigned char wallboardArgs[]	= { 
		SPECIAL_TWINKLE, SPECIAL_SPARKLE, SPECIAL_SNOW, SPECIAL_INTERLOCK, SPECIAL_SWITCH, 
		SPECIAL_SLIDE, SPECIAL_SPRAY, SPECIAL_STARBURST, SPECIAL_WELCOME, SPECIAL_SLOT_MACHINE,
		SPECIAL_THANK_YOU, SPECIAL_NO_SMOKING, SPECIAL_DRINK_DRIVE, SPECIAL_RUNNING_ANIMAL, SPECIAL_FIREWORKS,
		SPECIAL_TURBO_CAR, SPECIAL_CHERRY_BOMB
	};
	return parseFromArray(humanArgs, wallboardArgs, sizeof(wallboardArgs), specialArg);
}

void printSpecialNames()
{
	printAll(specialNames);
}

// This is a NULL-terminated list of mode names. If you add, remove, or change the order, you must make the corresponding chant in 
// parseStyleArg.
static char* modeNames[] = 
	{
		"rotate",	"hold",			"flash",	"rollUp",	"rollDown",
		"rollLeft", "rollRight",	"wipeUp",	"wipeDown", "wipeLeft", 
		"wipeRight","scroll",		"random",	"rollIn",	"rollOut", 
		"wipeIn", "wipeOut", "compressedRotate", NULL
	};

unsigned char parseStyleArg(char* styleArg)
{
	char** humanArgs				= modeNames;
	unsigned char wallboardArgs[]	= {
		MODE_ROTATE,	MODE_HOLD,			MODE_FLASH,		MODE_ROLL_UP,	MODE_ROLL_DOWN,
		MODE_ROLL_LEFT, MODE_ROLL_RIGHT,	MODE_WIPE_UP,	MODE_WIPE_DOWN, MODE_WIPE_LEFT, 
		MODE_WIPE_RIGHT, MODE_SCROLL,		MODE_RANDOM,	MODE_ROLL_IN,	MODE_ROLL_OUT, 
		MODE_WIPE_IN, MODE_WIPE_OUT, MODE_COMPRESSED_ROTATE
	};
	return parseFromArray(humanArgs, wallboardArgs, sizeof(wallboardArgs), styleArg);
}

void printModeNames()
{
	printAll(modeNames);
}

// Print a null-terminated array
void printAll(char** arrayToPrint)
{
	for(unsigned int n=0; arrayToPrint[n] != NULL; n++)
		printf("\t%s\n", arrayToPrint[n]);
}

unsigned char parseFromArray(char* human[], unsigned char wallboard[], unsigned int optionCount, char* toParse)
{
	for( unsigned int n=0; n<optionCount; n++)
	{
		if (_strcmpi(human[n], toParse) == 0)
		{
			return wallboard[n];
		}
	}
	return FALSE;
}