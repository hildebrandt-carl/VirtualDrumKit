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

	// Begin status logging
	statusLog("Starting up the system RAD_TEAM");
	static int loopCount = 0;

	// Set up virtual clock timer
	TA1CTL = TASSEL0 + TAIE + MC0; // ACLK @ 32 kHz
	TA1CCR0 = 0x0400; // top value = 1024 -> interrupt at 64 Hz
	TA1CCTL0 = CCIE;

	// Start receiving over radio
	static uint8_t oldReceivedMsg[50] = "" ;
	static uint8_t msg[50] = "";
	rf1a_start_rx();

	// Set up motor output TODO
	P1DIR |= LEFT_H;
	P1DIR |= LEFT_R;
	P2DIR = RIGHT_H + RIGHT_R;

	while(1)
	{
		// Status log TODO: needed?
		loopCount++;	
		char counterStr[50];
		sprintf(counterStr,"System has been running for %d seconds.",loopCount);
		statusLog(counterStr);

		// TEST HITTING THE DRUM
		G_ON();
		hitDrum();
		printf("hit drum\r\n");

		etimer_set(&et, 1*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		G_OFF();
		unhitDrum();
		printf("unhit drum\r\n");

		etimer_set(&et, 5*CLOCK_SECOND); //hit every 5 seconds for now
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		//end TEST HITTING DRUM
		kickWatchdog(); //TODO

		// Process received wizzimote messages
	/*	getReceivedMessage(msg);
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			strcpy(oldReceivedMsg,msg);
			if(msg[0] == CLKREQ) // clock request message, for synchronization
			{
				// TODO: send CLKREQ ACK msg with virtual clock value
				char returnMsg[6];
				returnMsg[0] = CLKREQ | ACK;
				*((uint32_t*)(&returnMsg[2])) = virtualClock;  //TODO: right? also disable interrupts
				unicast_send(returnMsg,14,4); //TODO: set IDs
			}
			else if(msg[0] == SETCLK) // set clock message, for synchronization
			{
				int adjustment = atoi(&msg[2]);
				updateClock(adjustment);
			}
			else if(msg[0] == SCHDL) // schedule message, for playing drums
			{
				// if this drum set, add to FIFO queue
				if((msg[1] & MY_ID) != 0){
					writeFifo(*((uint32_t*)(&msg[2]))); //TODO: right?
				}
			}
			else if(msg[0] == 0x0) // hit now message, for playing drums
			{
				// if this drum set, play now
				if((msg[1] & MY_ID) != 0){
					playNow = 1;
				}
			}
		}

		// Check for a scheduled hit
		uint32_t clk;
		if(!readFifo(&clk)){ // fifo not empty
			if(clk == virtualClock){
				playNow = 1;
			}
		}

		// Play drum, if applicable
		if(playNow != 0){
			hitDrum();
			playNow = 0;
		}
		*/
		
	}
	PROCESS_END();
}

void updateClock(uint32_t adjustment){
	//TODO: disable interrupts
	uint32_t oldValue = virtualClock;
	uint32_t newValue = virtualClock + adjustment;
	// if skipping forward, discard any skipped entries from FIFO queue 
	if(newValue > oldValue){
		uint32_t clk;
		peekFifo(&clk);
		while(clk < newValue){
			playNow = 1;    // if we skip an entry, play now to make up for it
			readFifo(&clk); // discard skipped entry
			peekFifo(&clk); // peek next entry
		}
		virtualClock = newValue; // update clock
	}
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	printf("%d\r\n", virtualClock);
	char debugStr[50];
	sprintf(debugStr,"In the interrupt, clock is %d",virtualClock);
	statusLog(debugStr);
}
