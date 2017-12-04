#ifndef MESSAGE_H
#define MESSAGE_H

// CONTROL BYTE (byte 0)
#define ACK    (1<<7)
#define NACK   (1<<6)
#define CLKREQ (1<<5)
#define SETCLK (1<<4)
#define SCHDL  (1<<3)

// DRUMS BYTE (byte 1)
#define BASS   (1<<7)
#define FLTOM  (1<<6)
#define LOTOM  (1<<5)
#define HITOM  (1<<4)
#define SNARE  (1<<3)
#define HIHAT  (1<<2)
#define CYMBAL (1<<1)

// VCLK INT (bytes 2-5)

#endif
