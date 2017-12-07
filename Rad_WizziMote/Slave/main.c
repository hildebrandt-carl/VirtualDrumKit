#include <stdio.h>
#include "header.h"
#include "drum_utils.h"
#include "fifo_utils.h"
#include "utils.h"

// Variables
static volatile uint32_t virtualClock = 0;
static struct etimer et;
static volatile uint8_t playNow = 0;

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
	static uint8_t oldReceivedMsg[10] = "" ;
	static uint8_t msg[10] = "";
	rf1a_start_rx();

	// Set up motor output TODO
	P1DIR |= LEFT_H;
	P1DIR |= LEFT_R;
	P2DIR = RIGHT_H + RIGHT_R;

	//light_inverter variable
	int messageReceived = 0;
	int oldMessage = -1;

	statusLog("Started");
	while(1)
	{

		kickWatchdog(); //TODO

		// Process received wizzimote messages
		int newMessageFlag ;
		getReceivedMessage(msg, &newMessageFlag);
		
		if(oldMessage != newMessageFlag)
		{
			oldMessage = newMessageFlag;
			statusLog("Recieved a new message");
			printf("%d, ",	msg[0]);
			printf("%d, ",	msg[1]);
			uint32_t msgIntField =  *((uint32_t*)(&msg[2]));
			printf("%d\r\n",	msgIntField);

			if(messageReceived == 0)
			{
				Y_ON();
				messageReceived = 1 ;
			}
			else
			{
				messageReceived = 0;
				Y_OFF();
			}
			strcpy(oldReceivedMsg,msg);
			if(msg[0] == CLKREQ) // clock request message, for synchronization
			{	
				char returnMsg[6];
				returnMsg[0] = CLKREQ | ACK;
				returnMsg[1] = MY_ID;
				*((uint32_t*)(&returnMsg[2])) = virtualClock;  //TODO: also disable interrupts
				printf("---------------------Clock request returning - %d\r\n",virtualClock);
				unicast_send(returnMsg,6,1); 
			}
			else if(msg[0] == SETCLK) // set clock message, for synchronization
			{
				uint32_t adjustment = *((uint32_t*)(&msg[2]));
				printf("I was told to setclock by %d\r\n",adjustment);
				updateClock(adjustment);
			}
			else if(msg[0] == SCHDL) // schedule message, for playing drums
			{
				printf("I was told to schedule message\r\n");
				// if this drum set, add to FIFO queue
				if((msg[1] & MY_ID) != 0){
					printf("Adding a message to the FIFO queue\r\n");
					writeFifo(*((uint32_t*)(&msg[2])));
				}
			}
			else if(msg[0] == 0x0) // hit now message, for playing drums
			{
				printf("Hit message now sent\r\n");
				// if this drum set, play now
				if((msg[1] & MY_ID) != 0){
					printf("This hit message was for me!\r\n");
					playNow = 1;
				}
			}
		}

		// Check for a scheduled hit
		uint32_t clk;
		peekFifo(&clk);
		if(clk == virtualClock){
			playNow = 1;
			readFifo(&clk);
		}

		// Play drum, if applicable
		if(playNow == 1){
			playNow = 0;
			hitDrum();
		}
	}
	PROCESS_END();
}

void updateClock(uint32_t adjustment){
	//TODO: disable interrupts
	printf("adjustment %d\r\n", adjustment);

	uint32_t oldValue = virtualClock;
	uint32_t newValue = virtualClock + adjustment;

	printf("Old value %d\r\n", oldValue);
	printf("New value %d\r\n", newValue);

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
