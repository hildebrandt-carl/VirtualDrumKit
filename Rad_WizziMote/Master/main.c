#include "header.h"
#include "drumPorts.h"
#include "message.h"
#include <stdio.h>

// Timer variables
volatile uint8_t timeToSync = 0; 	// set in interrupt handler when it is time to synchronize wizzimotes
volatile uint8_t ackTimerUp = 0; 	// set in interrupt handler when ACK timer expires
volatile uint8_t gotUartMessage = 0; 	// set in interrupt handler when new UART message has arrived

// Other variables
static volatile uint32_t virtualClock = 0;
volatile char* uartMessage; 		// set in interrupt handler
static uint8_t sentSchdlMsg[6];		// used to make sure SCHDL messages get ACKs TODO


PROCESS(main_process, "Main Task");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data)
{
	// Begin the process
	PROCESS_BEGIN();	
	
	// Sets up the watchdog timer to use ACLK input and an interval of 16s TODO: shorter interval?
	WDTCTL = WDTPW + WDTSSEL0 + WDTHOLD + WDTIS1 + WDTIS0;
	WDTCTL = (WDTCTL_L&~(WDTHOLD))+ WDTPW; 	// Start the watchdog

	// Begin status logging
	statusLog("Starting up the system RAD_TEAM");
	static int loopCount = 0;

	//Start receiving
	static uint8_t oldReceivedMsg[50] = "" ;
	static uint8_t msg[50] = "";
	rf1a_start_rx();
	
	//TODO: set up UART?

	// Set up virtual clock timer
	TA1CTL = TASSEL0 + TAIE + MC0;
	TA1CCR0 = 0x0800;
	TA1CCTL0 = CCIE;
	static int sentTime = 0;

	// Set up the ack timer to a given interval..? but don't run it yet...
	//TODO

	while(1)
	{
		// Status log TODO: needed?
		loopCount++;	
		char* secondCounterStr[50];
		sprintf(secondCounterStr,"System has been running for %d seconds.",loopCount);
		statusLog(secondCounterStr);

		// Check sync timer
		if(timeToSync)
		{
			uint8_t msg[6];
			msg[0] = CLKREQ;
			//TODO: broadcast msg
			timeToSync = 0; //TODO: disable interrupts around this
		}

		// Process received wizzimote messages
		getReceivedMessage(msg);
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			strcpy(oldReceivedMsg,msg);
			if(msg[0] == (CLKREQ | ACK))
			{
				// calculate difference
				uint32_t slaveTime = atoi((char *)(&msg[2]));
				uint32_t diff = ((sentTime + virtualClock) / 2) - slaveTime;
				// send SETCLK msg
				char returnMsg[7];
				returnMsg[0] = SETCLK;
				returnMsg[2] = diff>>24;  //TODO: right?
				returnMsg[3] = diff>>16;
				returnMsg[4] = diff>>8;
				returnMsg[5] = diff>>0;
				unicast_send(returnMsg,14,4); //TODO: set IDs
			}
			else if(msg[0] == (ACK))
			{
				//sentSchdlMsg TODO
			}
		}
		
		// Process received UART messages
		//TODO
		//if SCHDL, store into sentSchdlMsg and start ACK timer
		//regardless, broadcast to wizzimotes
	}
	PROCESS_END();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	if(virtualClock % 64 == 0) //TODO: okay?
	{ 
		timeToSync = 1;
		debugLog("Time to sync.");
	}
	char* debugStr[50];
	sprintf(debugStr,("In the interrupt, clock is %d", virtualClock));
	debugLog(debugStr);
}
