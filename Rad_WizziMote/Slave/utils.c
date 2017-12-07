#include "utils.h"
#include "header.h"

static uint8_t PROGRESS=0;

// Returns the value of the progress variable
uint8_t getPROGRESS()
{
	return PROGRESS;
}

// Clears the progress variable - sets all bits to 0
void clearPROGRESS()
{
	PROGRESS=0;
}

// Takes an integer which represents which task has completed and sets the PROGRESS bit for that task
void setPROGRESS(uint8_t i)
{
	switch(i)
	{
		case 1:
			PROGRESS |= (1<<1);
			break;
		case 2:
			PROGRESS |= (1<<2);
			break;
		case 3:
			PROGRESS |= (1<<3);
			break;
		case 4:
			PROGRESS |= (1<<4);
			break;
		default:
			statusLog("ERROR: invalid input to setPROGRESS function");
			break;
	}
}

// Logs error or status information to stdout
void statusLog(char* str)
{
	printf("Status Log: %s\r\n",str);
}
// Logs debug information to stdout
void debugLog(char* str)
{
	if(DEBUG)
	{
		printf("DEBUG: %s\r\n",str);
	}
}

// Function to kick the watchdog
void kickWatchdog()
{
	uint8_t p = getPROGRESS();
	if((p & 0x00) == 0x00) //TODO
	{
		clearPROGRESS();
		//statusLog("Watchdog being kicked");
		WDTCTL = (WDTCTL & 0xFF) + WDTPW + WDTCNTCL; // kick watchdog
	}
}
