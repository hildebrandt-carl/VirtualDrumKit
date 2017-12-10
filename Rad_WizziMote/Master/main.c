#include "header.h"
#include "utils.h"
#include <stdio.h>

// Timer variables
volatile uint8_t syncID = 0; 		// set in interrupt handler when it is time to synchronize wizzimote with given ID
volatile uint8_t gotUartMessage = 0; 	// set in interrupt handler when new UART message has arrived

// Other variables
static volatile uint32_t virtualClock = 0;
volatile uint8_t uartMessage[6]; 	// set in interrupt handler
static char debugStr[50];

// Functions
int rx_handler(unsigned char c);

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
	//statusLog("Starting up the system RAD_TEAM");

	// Set up virtual clock timer
	TA1CTL = TASSEL0 + TAIE + MC0; // ACLK @ 32 kHz
	TA1CCR0 = 0x0400; // top value = 1024 -> interrupt at 64 Hz
	TA1CCTL0 = CCIE;
	static uint32_t sentTime;

	// Start receiving over radio
	static uint8_t msg[10] = "";
	static uint8_t oldMsgCnt = 0;
	static uint8_t newMsgCnt = 0;
	rf1a_start_rx();
	
	// set up UART - register handler function to contiki's uart code
	uart_set_input_handler(rx_handler);

	while(1)
	{
		kickWatchdog(); //TODO

		// Process received wizzimote messages
		getReceivedMessage(msg, &newMsgCnt);
		if(newMsgCnt != oldMsgCnt)
		{
			Y_T();
			oldMsgCnt = newMsgCnt;
			if(msg[0] == (CLKREQ | ACK))
			{
				uint8_t destID = msg[1];
				// calculate difference
				uint32_t slaveTime = *((uint32_t*)(&msg[2]));
				uint32_t diff = ((sentTime + virtualClock) / 2) - slaveTime;
				// setup SETCLK msg
				char returnMsg[6];
				returnMsg[0] = SETCLK;
				*((uint32_t*)(&returnMsg[2])) = diff;
				// send SETCLK msg
				unicast_send(returnMsg,6, destID);
				// debug info
				sprintf(debugStr,"Sent update message %d, %d, %d", returnMsg[0], returnMsg[1], (*((uint32_t*)(&returnMsg[2]))) );
				debugLog(debugStr);
			}
		}

		// Synchronize wizzimotes
		if(syncID != 0)
		{
			uint8_t msg[6];
			msg[0] = CLKREQ;
			unicast_send(msg, 6, syncID);
			sentTime = virtualClock; //TODO: disable interrupts
			syncID = 0;
			// debug info
			sprintf(debugStr,"Sent sync message %d, %d, %d", msg[0], msg[1], (*((uint32_t*)(&msg[2]))) );	
			debugLog(debugStr);
		}

		// Process received UART messages
		UCA0IE |= UCRXIE;
		if(gotUartMessage)
		{	
			G_T();
			gotUartMessage = 0;
			// Set clock message
			if(uartMessage[0] == SETCLK)
			{
				virtualClock = *((uint32_t*)(&uartMessage[2])); //TODO: disable interrupts
				// debug info
				sprintf(debugStr,"Got set clock message %d, %d, %d", uartMessage[0], uartMessage[1], (*((uint32_t*)(&uartMessage[2]))) );
				debugLog(debugStr);
			}
			// Schedule message
			else if(uartMessage[0] == SCHDL)
			{
				broadcast_send(uartMessage, 6);
				// debug info
				sprintf(debugStr,"Sent schedule message %d, %d, %d", uartMessage[0], uartMessage[1], (*((uint32_t*)(&uartMessage[2]))) );
				debugLog(debugStr);
			}
			// Cancel message
			else if(uartMessage[0] == CANCEL){
				broadcast_send(uartMessage, 6);
				// debug info
				sprintf(debugStr,"Sent cancel message %d, %d, %d", uartMessage[0], uartMessage[1], (*((uint32_t*)(&uartMessage[2]))) );
				debugLog(debugStr);
			}
			// Play now message
			else if(uartMessage[0] == 0x0)
			{
				broadcast_send(uartMessage, 6);	
				// debug info
				sprintf(debugStr,"Sent hit now message %d, %d, %d", uartMessage[0], uartMessage[1], *((uint32_t*)(&uartMessage[2])));
				debugLog(debugStr);
			}
		}

	} //end while loop
	PROCESS_END();
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1A0ISR(void)
{
	virtualClock++;
	int num = virtualClock % 64;
	switch(num)
	{
		case 0:  syncID = BASS;  break;
		case 8:  syncID = FLTOM; break;
		case 16: syncID = LOTOM; break;
		case 24: syncID = HITOM; break;
		case 32: syncID = SNARE; break;
		case 40: syncID = HIHAT; break;
		case 48: syncID = CYMBAL; break;

	}
	sprintf(debugStr,"In the interrupt, clock is %d", virtualClock);
	//debugLog(debugStr);
}

#define IDLE 0
#define DATA 1
#define CHKSUM 2
#define START_BYTE 0xFF
volatile static uint8_t dataCnt = 0;
volatile static uint8_t checksum = 0;
volatile static uint8_t state = IDLE;
volatile static uint8_t workingMessage[6];
int rx_handler(unsigned char c)
{
	R_T();
	uint8_t byte = (uint8_t)c;
	//debug info
	sprintf(debugStr,"Received UART character %d", byte);
	debugLog(debugStr);
	//receive msg state machine
	if(state == IDLE)
	{
		if(byte == START_BYTE)
		{
			state = DATA;
			dataCnt = 0;
			checksum = 0;
		}
	}
	else if(state == DATA)
	{
		workingMessage[dataCnt] = byte;
		dataCnt++;
		checksum += byte;
		if(dataCnt >= 6){ state = CHKSUM; }
	}
	else if(state == CHKSUM)
	{
		if(checksum == byte)
		{
			gotUartMessage = 1;
			int i=0; for(i=0; i<6; i++){ uartMessage[i] = workingMessage[i]; }
			debugLog("Received a full UART message!");
		}else{
			debugLog("UART checksum failed.");
		}
		state = IDLE;
	}
	return 0;
}
