#include "header.h"
#include "utils.h"
#include <stdio.h>

// Timer variables
volatile uint8_t timeToSync = 0; 	// set in interrupt handler when it is time to synchronize wizzimotes
volatile uint8_t ackTimerUp = 0; 	// set in interrupt handler when ACK timer expires TODO(low priority)
volatile uint8_t gotUartMessage = 0; 	// set in interrupt handler when new UART message has arrived

// Other variables
static volatile uint32_t virtualClock = 0;
volatile uint8_t uartMessage[6]; 	// set in interrupt handler
static uint8_t sentSchdlMsg[6];		// used to make sure SCHDL messages get ACKs TODO(low priority)

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
	static uint32_t sentTime;

	// Start receiving over radio
	static uint8_t oldReceivedMsg[50] = "" ;
	static uint8_t msg[50] = "";
	rf1a_start_rx();
	
	//TODO: set up UART
	

	// Set up the ack timer to a given interval..? but don't run it yet...
	//TODO(low priority)

	while(1)
	{
		// Status log TODO: needed?
		loopCount++;	
		char counterStr[50];
		sprintf(counterStr,"System has been running for %d seconds.",loopCount);
		statusLog(counterStr);

		// Process received wizzimote messages
		getReceivedMessage(msg);
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			strcpy(oldReceivedMsg,msg);
			if(msg[0] == (CLKREQ | ACK))
			{
				// calculate difference
				uint32_t slaveTime = *((uint32_t*)(&msg[2]));
				uint32_t diff = ((sentTime + virtualClock) / 2) - slaveTime;
				// send SETCLK msg
				char returnMsg[7];
				returnMsg[0] = SETCLK;
				*((uint32_t*)(&returnMsg[2])) = diff;  //TODO: right?
				unicast_send(returnMsg,7, 4); //TODO: set dest ID
			}
			else if(msg[0] == (ACK))
			{
				//sentSchdlMsg TODO(low priority)
			}
		}
		
		// Process received UART messages
		if(gotUartMessage)
		{
			// Set clock message
			if(uartMessage[0] == SETCLK)
			{
				//TODO: disable interrupts
				virtualClock = *((uint32_t*)(&uartMessage[2])); //TODO: right?
				timeToSync = 1; // sync now since clock changed
			}
			// Schedule message
			else if(uartMessage[0] == SCHDL)
			{
				//store into sentSchdlMsg TODO(low priority)
				int i=0;
				for(i=0; i<7; i++){ sentSchdlMsg[i] = uartMessage[i]; }
				//start ack timer TODO(low priority)
				broadcast_send(uartMessage, 7);// broadcast msg TODO: right?
			}
			// Play now message
			else if(uartMessage[0] == 0x0)
			{
				broadcast_send(uartMessage, 7);// broadcast msg TODO: right?
			}
		}
		
		// Synchronize wizzimotes
		if(timeToSync)
		{
			uint8_t msg[6];
			msg[0] = CLKREQ;
			broadcast_send(msg, 7);// broadcast msg TODO: right?
			//TODO: disable interrupts
			sentTime = virtualClock;
			timeToSync = 0;
		}
	} //end while loop
	PROCESS_END();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	if(virtualClock % 64 == 0) // synchronize every second TODO: okay?
	{ 
		timeToSync = 1;
	}
	char debugStr[50];
	sprintf(debugStr,"In the interrupt, clock is %d", virtualClock);
	debugLog(debugStr);
}
