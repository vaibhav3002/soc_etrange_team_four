/************************************************************
 *
 *      File : etrange.h
 *      Author: A. Schindler
 *      Credits: T. Graba
 *      Telecom ParisTECH
 *
 *      Etrange code to calculate the coefficients for the
 *      incremental calculation
 *
 ************************************************************/



#ifndef _ETRANGE_H
#define _ETRANGE_H

#include "mfixed.h"
#include "../segmentation.h"

	//a30 a21 a12 a03 a20 a11 a02 a10 a01 a00
	// 0   1   2   3   4   5   6   7   8   9

extern mfixed a[10];

/* registers connected to the wishbone slave interface */
/* ------------------------------------------------ */
mfixed p0;
mfixed s0;
mfixed s1;
mfixed r0;
mfixed r1;
mfixed r2;
mfixed q0;
mfixed q1;
mfixed q2;
mfixed q3;
/* ----------------------------------------------- */ 

mfixed r2_c1;
mfixed r2_c2;
mfixed q2_c1;
mfixed q2_c2;

mfixed x2;
mfixed x3;
mfixed y2;
mfixed y3;

/* called once at the beginning to initialise the constants */
void etrange_initialize(mfixed* p0, mfixed* s0, mfixed* r0, mfixed* q0, mfixed* r2_c1, mfixed* r2_c2, mfixed* q2_c1, mfixed* q2_c2);
/* called for each tile*/
void etrange_polyinit(mfixed* s1, mfixed* r1, mfixed* r2, mfixed* q1, mfixed* q2, mfixed* q3, mfixed x, mfixed y);


#endif
