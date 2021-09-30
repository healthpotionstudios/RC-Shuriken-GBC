#ifndef TRIG_H
#define TRIG_H

#include <gb/gb.h>

extern const UINT8 sin[];
extern const UINT8 arctan[];

#define SIN(A) (sin[0xFF & (A)] - 128)
#define COS(A) (sin[0xFF & ((A) - 64)] - 128)

//arctan
UINT8 AngleTowards(INT16 x1, INT16 y1, INT16 x2, INT16 y2);

#endif