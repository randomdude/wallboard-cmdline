#include "wallboard.h"
#include <windows.h>
#include <stdio.h>

// Open the specified port. Return FALSE on failure.
HANDLE openport(char * port)
{
	DCB mydcb;
	HANDLE porthnd;

	// Open serial port
	porthnd = CreateFile(port, GENERIC_READ | GENERIC_WRITE ,NULL, NULL,OPEN_EXISTING, 0, NULL);
	if (porthnd==INVALID_HANDLE_VALUE) 
		return FALSE;
	// set port config - 9600, 7e1
	if (!GetCommState(porthnd, &mydcb)) 
		return FALSE;
	mydcb.BaudRate = 9600;
	mydcb.ByteSize = 7;
	mydcb.Parity = EVENPARITY;
	mydcb.StopBits = ONESTOPBIT	;
	if (!SetCommState(porthnd, &mydcb)) 
		return FALSE;

	return porthnd;
}

void printWindowsError(DWORD errorCode)
{
		char* errorText = NULL;

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,   NULL,
		errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorText,  0,  NULL);

		if ( NULL == errorText )
		{
			// Uhoh, FormatMessage has failed. Fallback to just displaying the error code - the user will have to sort it out.
			printf("GetLastError reported error code %d\n", errorCode); 
		}
		else
		{
			// Okay, we got an error message.
			printf("Error %d: '%s'\n", errorCode, errorText); 
			LocalFree(errorText);
		}
}

// displays message in sayit on all wallboards on handle 'port'. Set special to 0xff for no special mode. 
// returns FALSE on error and TRUE on success.
BOOL SendWallMessage(HANDLE port, char *sayit, char pos, unsigned char style, char col, unsigned char special,  bool dumppkt)
{
	// Points to the next free element of 'message'.
	unsigned int bytesWritten = 0;

	// Protocol format - 
	// 		   <NUL> <NUL> <NUL> <NUL> <NUL> 
	// SOH character - 0x01
	// Type code     - 'Z'  (all types of wallboard)
	// Address field - "??" (all stations)
	// Start of Text - 0x02
	// Command code  - 'A'  (Write text file)
	// Start of text file - 'A' 
	// File label	 - '0' (prioirity file)
	// Escape code   - 0x1B
	// Display position
	// Style code
	// (optional byte of 'special' set only when style = 'n')
	// ASCII data ....
	// EOT	         - 0x04

	unsigned char messagePrelude[]={ 00,00,00,00,00,// Five NULLs precede the packet							
						01,							// "Start Of Header", a constatn 0x01						
						'Z',						// The type code, set to 'All types of wallboard', or 'Z'
						'?','?',					// The address field, set to '??' for "all stations"
						 02,						// The "Start Of Text" marker, 0x02
						'A',						// The "Start of Text File" marker, 'A'
						'0',						// The file 'label' - '0' for "priority file", which gets displayed immediately
						0x1b	};					// The end of the prelude, 0x1b.

	// Allocate some space for our packet
	unsigned char* message = (unsigned char*)malloc(sizeof(messagePrelude) + 
							1+			// Display position
							1+			// the 'style' byte
							1+			// For any 'special' specifier that may be present (one byte for the 'special follows' byte and one for the actual special)
							1+			// Escape code, set colour (CTRL-\)
							1+			// Colour specifier
							strlen(sayit)+	// The actual string to send
							1);				// The 'End of transmission' marker

	// Copy in the constant prelude
	memcpy(message, messagePrelude, sizeof(messagePrelude));
	bytesWritten += sizeof(messagePrelude);

	// The next byte is the 'display position', which specifies the position that the message will be displayed.
	message[bytesWritten++] = pos;

	// And now the 'style', which can be 0x62 to denote no style, or 'n' to specify a 'special'.
	// Specials take precedence over styles.
	if (special != 0xff)
	{
		// Special is present.
		message[bytesWritten++]='n';
	}
	else
	{
		// No special. Write the specified style, or a default of 'STYLE_HOLD'.
		if (style != 0) 
			message[bytesWritten++] = style;
		else 
			message[bytesWritten++] = MODE_HOLD;
	}

	// If there is a 'special' specifier, insert the specifier.
	if (special != 0xff)
	{
		message[bytesWritten++]=special;
	}

	// Set the colour, which should be denoted by CTRL-\, which is 0x1C.
	message[bytesWritten++] = 0x1C;
	message[bytesWritten++] = col;

	// Now copy in our string
	bytesWritten += wsprintf((char*)&message[bytesWritten], "%s",  sayit);

	// and finally, the "End of transmission" marker, 0x04.
	message[bytesWritten++] = 0x04;

	// Write the packet to stdout if the user wants this to happen
	if (TRUE==dumppkt)
	{
		for (unsigned int n=0; n<bytesWritten; n++)
		{
			// Every five bytes, switch to a new line, and prefix it with our byte count and two tabs.
			if (n%5 == 0)
			{
				printf("\n%02x:\t", n);
			}
			printf(" %c (0x%02.2x) ", message[n], ((unsigned char)message[n]) );
		}
	}

	// And send it out to the serial port.
	DWORD done;
	BOOL s = WriteFile(port, message, bytesWritten, &done, NULL);
	if (!s || bytesWritten != done)
	{
		printWindowsError(GetLastError());
		free(message);
		return FALSE;
	}

	free(message);
	return TRUE;
}

