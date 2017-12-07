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
		kickWatchdog();
		// Status log TODO: needed?
		loopCount++;	
		char counterStr[50];
		//sprintf(counterStr,"System has been running for %d seconds.",loopCount);
		//statusLog(counterStr);

		// Process received wizzimote messages
		getReceivedMessage(msg);
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			strcpy(oldReceivedMsg,msg);
			if(msg[0] == (CLKREQ | ACK))
			{
				// calculate difference
				uint8_t destID = msg[1];
				uint32_t slaveTime = *((uint32_t*)(&msg[2]));
				uint32_t diff = ((sentTime + virtualClock) / 2) - slaveTime;
				// send SETCLK msg
				char returnMsg[6];
				returnMsg[0] = SETCLK;
				*((uint32_t*)(&returnMsg[2])) = diff;  //TODO: right?
				unicast_send(returnMsg,6, 5); //TODO: set dest ID
				char debugStr[50];
				sprintf(debugStr,"Sending update message %d, %d, %d", returnMsg[0], returnMsg[1], 
						(*((uint32_t*)(&returnMsg[2]))) );
				debugLog(debugStr);
			}
			else if(msg[0] == (ACK))
			{
				//sentSchdlMsg TODO(low priority)
			}
		}
		//GENERATE MESSAGES FOR TESTING
		static int cnt = 500;
		static int newCnt = 500;
		while(virtualClock % 16 == 0 && virtualClock < 200){ 
			gotUartMessage = 1; 
			*((uint32_t*)(&uartMessage[2])) = cnt;
			newCnt = cnt+4;
		}
		while(virtualClock % 16 == 8 && virtualClock < 200){ 
			gotUartMessage = 1; 
			*((uint32_t*)(&uartMessage[2])) = cnt;
			newCnt = cnt+12;
		}
		cnt = newCnt;
		uartMessage[0] = SCHDL;
		uartMessage[1] = SNARE;
		
		// Process received UART messages
		if(gotUartMessage)
		{	Y_T();
			// Set clock message
			if(uartMessage[0] == SETCLK)
			{
				//TODO: disable interrupts
				virtualClock = *((uint32_t*)(&uartMessage[2]));
				timeToSync = 1; // sync now since clock changed
			}
			// Schedule message
			else if(uartMessage[0] == SCHDL)
			{
				//store into sentSchdlMsg TODO(low priority)
				//int i=0;
				//for(i=0; i<6; i++){ sentSchdlMsg[i] = uartMessage[i]; }
				//start ack timer TODO(low priority)
				broadcast_send(uartMessage, 6);
				char debugStr[50];
				sprintf(debugStr,"Sending schedule message %d, %d, %d", uartMessage[0], uartMessage[1], 
						(*((uint32_t*)(&uartMessage[2]))) );
				debugLog(debugStr);
			}
			// Play now message
			else if(uartMessage[0] == 0x0)
			{
				unicast_send(uartMessage, 6, 5); //TODO	
				char debugStr[50];
				sprintf(debugStr,"Sending hit now message %d, %d, %d", uartMessage[0], uartMessage[1], *((uint32_t*)(&uartMessage[2])));
				debugLog(debugStr);
			}
			gotUartMessage = 0;
		}

		// Synchronize wizzimotes
		if(timeToSync)
		{
			uint8_t msg[6];
			msg[0] = CLKREQ;
			broadcast_send(msg, 6);
			char debugStr[50];
			sprintf(debugStr,"Sending sync message %d, %d, %d", msg[0], msg[1], 
						(*((uint32_t*)(&msg[2]))) );	
			debugLog(debugStr);
			//TODO: disable interrupts
			sentTime = virtualClock;
			timeToSync = 0;
			//char debugStr[50];
			sprintf(debugStr,"Synchronizing at virtual clock val %d", virtualClock);
			//debugLog(debugStr);
		}
	} //end while loop
	PROCESS_END();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	if(virtualClock % 64 == 0)
	{ 
		timeToSync = 1;
	}
	char debugStr[50];
	sprintf(debugStr,"In the interrupt, clock is %d", virtualClock);
	//debugLog(debugStr);
}
