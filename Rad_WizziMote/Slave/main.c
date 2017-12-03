#include <stdio.h>

#include "header.h"
#include "drumPorts.h"
#include "message.h"

// Variables
static volatile uint32_t virtualClock = 0;
static struct etimer et;

// Functions
void hitDrum();

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

	// Turn off LEDs
	G_OFF();
	R_OFF();
	Y_OFF();

	//Start receiving
	static uint8_t oldReceivedMsg[50] = "" ;
	static uint8_t msg[50] = "";
	rf1a_start_rx();

	// Set up virtual clock timer
	TA1CTL = TASSEL0 + TAIE + MC0;
	TA1CCR0 = 0x0800;
	TA1CCTL0 = CCIE;

	while(1)
	{
		// Status log TODO: needed?
		loopCount++;	
		char* counterStr[50];
		sprintf(counterStr,"System has been running for %d seconds.",loopCount);
		statusLog(counterStr);

		// TEST HITTING THE DRUM
		G_ON();
		hitDrum();
		G_OFF();
		etimer_set(&et, 5*CLOCK_SECOND); //hit every 5 seconds for now
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		// Process received wizzimote messages
	/*	getReceivedMessage(msg);
		if(strcmp(msg,oldReceivedMsg) != 0)
		{
			strcpy(oldReceivedMsg,msg);
			if(msg[0] == (CLKREQ))
			{
				// TODO: send CLKREQ ACK msg with virtual clock value
				char returnMsg[6];
				returnMsg[0] = CLKREQ | ACK;
				returnMsg[2] = virtualClock>>24;  //TODO: right? also disable interrupts
				returnMsg[3] = virtualClock>>16;
				returnMsg[4] = virtualClock>>8;
				returnMsg[5] = virtualClock>>0;
				unicast_send(returnMsg,14,4); //TODO: set IDs
			}
			else if(msg[0] == SETCLK)
			{
				//TODO: disable interrupts?
				//TODO: check FIFO queue!
				int adjustment = atoi(&msg[2]);
				virtualClock += adjustment;
			}
			else if(msg[0] == SCHDL) // Schedule message
			{
				// TODO: FIFO queue stuff
			}
			else if(msg[0] == 0x0) // Hit now message
			{
				hitDrum();
			}
		}
		*/
		
	}
	PROCESS_END();
}

void hitDrum(){
	// Code based off ThatOneTeam's code from last year
	static uint8_t hitLeft = 0x0;
	if (hitLeft)
        {
                LEFT_PORT |= LEFT_H;
                LEFT_PORT &= ~LEFT_R;
		R_ON();
        }
        else
        {
                RIGHT_PORT |= RIGHT_H;
                RIGHT_PORT &= ~RIGHT_R;
                Y_ON();
        }

        // Delay?
	etimer_set(&et, 0.001*CLOCK_SECOND);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        //retract strike
        if (hitLeft)
        {
                LEFT_PORT &= ~LEFT_H;
                LEFT_PORT |= LEFT_R;
        }
        else
        {
                RIGHT_PORT &= ~RIGHT_H;
                RIGHT_PORT |= RIGHT_R;
        }

        hitLeft = !hitLeft;
	R_OFF();
	Y_OFF();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	char* debugStr[50];
	sprintf(debugStr,("In the interrupt, clock is %d", virtualClock));
	debugLog(debugStr);
}
