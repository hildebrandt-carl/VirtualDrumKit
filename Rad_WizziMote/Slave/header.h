#ifndef HEADER_H
#define HEADER_H

#include "contiki.h"
#include "platform.h"
#include "dev/serial-line.h"
#include "config.h"

//DEBUG flag: set 1 to print debug info
#define DEBUG 1

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

// Declare the processes
PROCESS_NAME(main_process); // main process
PROCESS_NAME(WDTCHECK); // function to kick the watchdog

// Helper functions
uint8_t getPROGRESS();
void setPROGRESS(uint8_t);
void clearPROGRESS();

void statusLog(char*);
void debugLog(char*);

#endif
