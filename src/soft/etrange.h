#ifndef _ETRANGE_H
#define _ETRANGE_H

#include "mfixed.h"

typedef struct {
	//a30 a21 a12 a03 a20 a11 a02 a10 a01 a00
	mfixed a[10];
} P3;
typedef struct {
	//b20 b11 b02 b10 b01 b00
	mfixed b[6];
} P2;
typedef struct {
	//c10 c01 c00
	mfixed c[3];
} P1;
typedef struct {
	//d00
	mfixed d;
} P0;

void etrange_initialize(P3* p3, P2* p2, P1* p1, P0* p0);
void etrange_polyinit(P3* p3, P2* p2, P1* p1, mfixed x, mfixed y, mfixed* p30, mfixed* p20, mfixed* p10);


#endif
