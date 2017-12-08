#include <stdio.h>
#include "header.h"
#include "drum_utils.h"
#include "fifo_utils.h"
#include "utils.h"

// Variables
static volatile uint32_t virtualClock = 0;
static struct etimer et;
static volatile uint8_t playNow = 0;
static char debugStr[50];

// Functions
void updateClock();

PROCESS(main_process, "Main Task");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data)
{
	PROCESS_BEGIN();	
	
	// Sets up the watchdog timer to use ACLK input and an interval of 16s TODO: shorter interval?
	WDTCTL = WDTPW + WDTSSEL0 + WDTHOLD + WDTIS1 + WDTIS0;
	WDTCTL = (WDTCTL_L&~(WDTHOLD))+ WDTPW; 	// Start the watchdog

	// Turn off LEDs and init as outputs
	G_OFF(); G_OUT();
	R_OFF(); R_OUT();
	Y_OFF(); Y_OUT();

	G_ON();

	// Begin status logging
	statusLog("Starting up the system RAD_TEAM");

	// Set up virtual clock timer
	TA1CTL = TASSEL0 + TAIE + MC0; // ACLK @ 32 kHz
	TA1CCR0 = 0x0400; // top value = 1024 -> interrupt at 64 Hz
	TA1CCTL0 = CCIE;

	// Start receiving over radio
	static uint8_t msg[10] = "";
	static uint8_t oldMsgCnt = -1;
	static uint8_t newMsgCnt = -1;
	rf1a_start_rx();

	// Set up motor output
	P1DIR |= LEFT_H;
	P1DIR |= LEFT_R;
	P2DIR = RIGHT_H + RIGHT_R;

	while(1)
	{

		kickWatchdog(); //TODO

		// Process received wizzimote messages
		getReceivedMessage(msg, &newMsgCnt);
		if(oldMsgCnt != newMsgCnt)
		{
			Y_T();
			oldMsgCnt = newMsgCnt;

			if(msg[0] == CLKREQ) // clock request message, for synchronization
			{	
				char returnMsg[6];
				returnMsg[0] = CLKREQ | ACK;
				returnMsg[1] = MY_ID;
				*((uint32_t*)(&returnMsg[2])) = virtualClock;  //TODO: disable interrupts
				unicast_send(returnMsg,6, 0xF);
				//debug info
				sprintf(debugStr,"Sent CLKREQ ACK message %d, %d, %d", 
					returnMsg[0], returnMsg[1], (*((uint32_t*)(&returnMsg[2]))) );
				debugLog(debugStr);
			}
			else if(msg[0] == SETCLK) // set clock message, for synchronization
			{
				uint32_t adjustment = *((uint32_t*)(&msg[2]));
				updateClock(adjustment);
				//debug info
				sprintf(debugStr,"Received SETCLK message %d, %d, %d", 
					msg[0], msg[1], (*((uint32_t*)(&msg[2]))) );
				debugLog(debugStr);
			}
			else if(msg[0] == SCHDL) // schedule message, for playing drums
			{
				//debug info
				sprintf(debugStr,"Received SCHDL message %d, %d, %d", 
					msg[0], msg[1], (*((uint32_t*)(&msg[2]))) );
				debugLog(debugStr);
				// if this drum's bit is set, add to FIFO queue
				if((msg[1] & MY_ID) != 0){
					writeFifo(*((uint32_t*)(&msg[2])));
					debugLog("Added a message to the FIFO queue");
				}
			}
			else if(msg[0] == 0x0) // hit now message, for playing drums
			{
				//debug info
				sprintf(debugStr,"Received hit message %d, %d, %d", 
					msg[0], msg[1], (*((uint32_t*)(&msg[2]))) );
				debugLog(debugStr);
				// if this drum's bit is set, play now
				if((msg[1] & MY_ID) != 0){
					playNow = 1;
					debugLog("This hit message was for me!");
				}
			}
		}

		// Check for a scheduled hit
		uint32_t clk;
		peekFifo(&clk);
		if(clk == virtualClock){
			playNow = 1;
			readFifo(&clk);
			debugLog("Playing from the FIFO queue.");
		}


		// Retract stick, if applicable
		static uint32_t retractTime = 0;
		static uint8_t stickStatus = READY;
		#ifdef SINGLE_STICK
			if(virtualClock == retractTime)
			{
				if(stickStatus == HIT)
				{
					hitDrum(0);
					stickStatus = RETRACTED;
					retractTime = virtualClock + (COOLDOWN*64);
					debugLog("Retracted the drum stick.");
				}
				else if(stickStatus == RETRACTED)
				{
					stickStatus = READY;
					debugLog("Drum stick is now ready.");
				}
			}
		#else
			if(virtualClock == retractTime)
			{
				if(stickStatus == HIT)
				{
					stickStatus = READY;
					debugLog("Drum stick is now ready.");
				}
			}
		#endif
		// Play drum, if applicable
		if(playNow == 1){
			playNow = 0;
			if(stickStatus == READY){
				hitDrum(1);
				stickStatus = HIT;
				retractTime = virtualClock + (COOLDOWN*64);
				debugLog("Played the drum.");
			}else{
				debugLog("Did not play because not ready.");
			}
		}
	}
	PROCESS_END();
}

void updateClock(uint32_t adjustment){
	uint32_t oldValue = virtualClock; //TODO: disable interrupts
	uint32_t newValue = oldValue + adjustment;

	// if skipping forward, discard any skipped entries from FIFO queue 
	if(newValue > oldValue){
		uint32_t clk;
		if(!peekFifo(&clk))
		{
			while(clk < newValue){
				playNow = 1;    // if we skip an entry, play now to make up for it
				readFifo(&clk); // discard skipped entry
				peekFifo(&clk); // peek next entry
			}
		}
	}
	virtualClock = newValue; // update clock
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
}
