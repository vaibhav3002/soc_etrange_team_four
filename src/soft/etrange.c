/************************************************************
 *
 *      File : etrange.c
 *      Author: A. Schindler
 *      Credits: T. Graba
 *      Telecom ParisTECH
 *
 *      Etrange code to calculate the coefficients for the
 *      incremental calculation
 *
 ************************************************************/

#include "etrange.h"

#include "mfixed.h"

void etrange_initialize(mfixed* p0, mfixed* s0, mfixed* r0, mfixed* q0, mfixed* r2_c1, mfixed* r2_c2, mfixed* q2_c1, mfixed* q2_c2) {

  *p0 = fx_mul(nmfixed(6),a[0]);
  *s0 = fx_mul(nmfixed(2),(a[1]));
  *r0 = fx_mul(nmfixed(2),(a[2]));
  *q0 = fx_mul(nmfixed(6),(a[4]));
    
  *r2_c1 = fx_add(fx_mul(nmfixed(3),a[0]),fx_mul(nmfixed(2),a[4]));
       
  *r2_c2 = fx_add(a[1],a[5]);

       
  *q2_c1 = fx_add(a[2],a[5]);
      
  *q2_c2 = fx_add(fx_mul(nmfixed(3),a[3]),fx_mul(nmfixed(2),a[6]));

  // writes to the slave, x coordinates

  *((volatile unsigned long*) (COPROC_BASE)) = p0->all;
  *((volatile unsigned long*) (COPROC_BASE + 4)) = s0->all;
  *((volatile unsigned long*) (COPROC_BASE + 12)) = r0->all;
  *((volatile unsigned long*) (COPROC_BASE + 24)) = q0->all;

  // writes y coordinates, offset of 40

 *((volatile unsigned long*) (COPROC_BASE + 40)) = p0->all;
  *((volatile unsigned long*) (COPROC_BASE + 40 + 4)) = s0->all;
  *((volatile unsigned long*) (COPROC_BASE + 40 + 12)) = r0->all;
  *((volatile unsigned long*) (COPROC_BASE + 40 + 24)) = q0->all;

}

void etrange_polyinit(mfixed* s1, mfixed* r1, mfixed* r2, mfixed* q1, mfixed* q2, mfixed* q3, mfixed x, mfixed y) {

    x2 = fx_mul(x,x);
    x3 = fx_mul(x2,x);

    y2 = fx_mul(y,y);
    y3 = fx_mul(y2,y);


    *s1 = fx_add(fx_mul(nmfixed(6),a[0]),fx_add(fx_mul(nmfixed(2),a[4]),fx_add(fx_mul(nmfixed(6),fx_mul(a[0],x)),fx_mul(nmfixed(2),fx_mul(a[1],y)))));

    *r1 = fx_add(fx_mul(nmfixed(2),fx_mul(a[2],x)),fx_add(fx_mul(nmfixed(2),fx_mul(a[2],y)),fx_add(a[2],(fx_add(a[1],(fx_add(a[5],(fx_mul(nmfixed(2),fx_mul(a[2],y))))))))));


    *r2 = fx_add(fx_mul(x2,fx_mul(nmfixed(3),a[0])),fx_add(fx_mul(nmfixed(2),fx_mul(a[1],fx_mul(y,x))),fx_add(fx_mul(a[2],y2),fx_add(fx_mul(r2_c1,x),fx_add(fx_mul(r2_c2,y),fx_add(a[0],(fx_add(a[4],(a[7])))))))));

  
    *q1 = fx_add(fx_mul(nmfixed(2),fx_mul(a[2],x)),fx_add(fx_mul(nmfixed(6),fx_mul(a[3],y)),fx_add(fx_mul(nmfixed(6),a[3]),fx_add(fx_mul(nmfixed(2),a[6]),fx_mul(nmfixed(6),fx_mul(a[3],y))))));

    *q2 = fx_add(fx_mul(a[1],x2),fx_add(fx_mul(nmfixed(2),fx_mul(a[2],fx_mul(x,y))),fx_add(fx_mul(nmfixed(3),fx_mul(a[3],y2)),fx_add(fx_mul(q2_c1,x),fx_add(fx_mul(q2_c2,y),fx_add(a[3],(fx_add(a[6],(a[8])))))))));

    *q3 = fx_add(fx_mul(a[0],x3),fx_add(fx_mul(a[1],fx_mul(x2,y)),fx_add(fx_mul(a[2],fx_mul(x,y2)),fx_add(fx_mul(a[3],y3),fx_add(fx_mul(a[4],x2),fx_add(fx_mul(a[5],fx_mul(x,y)),fx_add(fx_mul(a[6],y2),fx_add(fx_mul(a[7],x),fx_add(fx_mul(a[8],y),a[9])))))))));
    
    // writes to the slave, x coordinates

    *((volatile unsigned long*) (COPROC_BASE + 8)) = s1->all;
    *((volatile unsigned long*) (COPROC_BASE + 16)) = r1->all;
    *((volatile unsigned long*) (COPROC_BASE + 20)) = r2->all;
    *((volatile unsigned long*) (COPROC_BASE + 28)) = q1->all;
    *((volatile unsigned long*) (COPROC_BASE + 32)) = q2->all;
    *((volatile unsigned long*) (COPROC_BASE + 36)) = q3->all;

    // writes y coordinates, offset of 40

    *((volatile unsigned long*) (COPROC_BASE + 40 + 8)) = s1->all;
    *((volatile unsigned long*) (COPROC_BASE + 40 + 16)) = r1->all;
    *((volatile unsigned long*) (COPROC_BASE + 40 + 20)) = r2->all;
    *((volatile unsigned long*) (COPROC_BASE + 40 + 28)) = q1->all;
    *((volatile unsigned long*) (COPROC_BASE + 40 + 32)) = q2->all;
    *((volatile unsigned long*) (COPROC_BASE + 40 + 36)) = q3->all;
}


