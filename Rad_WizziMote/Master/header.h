#ifndef HEADER_H
#define HEADER_H

#include "contiki.h"
#include "platform.h"
#include "dev/serial-line.h"
#include "config.h"

//DEBUG flag: set 1 to print debug info
#define DEBUG 0

#define MY_ID NODE_ID
//NOTE: change drum ID in config.h


//LED helpers
#define G_ON()   (P3OUT |=  (1<<6))
#define G_OFF()  (P3OUT &= ~(1<<6))
#define Y_ON()   (P3OUT |=  (1<<7))
#define Y_OFF()  (P3OUT &= ~(1<<7))
#define R_ON()   (P1OUT |=  (1<<7))
#define R_OFF()  (P1OUT &= ~(1<<7))
#define G_T()   (P3OUT ^=  (1<<6))
#define Y_T()   (P3OUT ^=  (1<<7))
#define R_T()   (P1OUT ^=  (1<<7))

#define G_OUT()   (P3DIR |=  (1<<6))
#define Y_OUT()   (P3DIR |=  (1<<7))
#define R_OUT()   (P1DIR |=  (1<<7))

// Declare the processes
PROCESS_NAME(main_process); // main process

#endif
