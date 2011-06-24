#include "etrange.h"

inline mfixed fx_mul(mfixed A, mfixed B);
inline mfixed fx_add(mfixed A, mfixed B);

void etrange_initialize(mfixed* s0, mfixed* r0, mfixed* q0, mfixed* p0, mfixed* r2_c1, mfixed* r2_c2, mfixed* q2_c1, mfixed* q2_c2) {
    /*//CONSTANTS
	mfixed tree;
	tree.l = 0;
	tree.h = 3;
	mfixed two;
	two.l = 0;
	two.h = 2;
	mfixed six;
	six.l = 0;
	six.h = 6;

	//INIT P2
	p2->b[0] = fx_mul(tree, p3->a[0]);
	p2->b[1] = fx_mul(two,p3->a[1]);
	p2->b[2] =   p3->a[2];
	p2->b[3] = fx_add(fx_mul(tree,p3->a[0]) , fx_mul(two,p3->a[4]));
	p2->b[4] =(mfixed)   (p3->a[1].all +   p3->a[5].all);
	p2->b[5] =(mfixed)   (p3->a[0].all +   p3->a[4].all +   p3->a[7].all);

	//INIT P1
	p1->c[0] = fx_mul(six,p3->a[0]);
	p1->c[1] = fx_mul(two,p3->a[1]);
	p1->c[2] = fx_add(fx_mul(six,p3->a[0]) , fx_mul(two,p3->a[4]));

	//INIT P0
	p0->d    = fx_mul(six,p3->a[0]);
    */

    s0 = fx_mul(mfixed(2),(a21));
    r0 = fx_mul(mfixed(2),(a12));
    q0 = fx_mul(mfixed(6),(a03));
    
    r2_c1 = fx_add(fx_mul(mfixed(3),a[0]),fx_mul(mfixed(2),a20));
       
    r2_c2 = fx_add(a21,a11);

       
    q2_c1 = fx_add(a12,a11);
      
    q2_c2 = fx_add(fx_mul(mfixed(3),a03),fx_mul(mfixed(2),a02));

}

void etrange_polyinit(mfixed* s1, mfixed* r1, mfixed* r2, mfixed* q1, mfixed* q2, mfixed* q3, mfixed x, mfixed y) {

    x2 = fx_mul(x,x);
    x3 = fx_mul(x2,x);

    y2 = fx_mul(y,y);
    y3 = fx_mul(y2,y);

    /*//COMPUTE XY X² Y² X²Y Y²X X³ Y³
      mfixed x2,y2,xy,x2y,y2x,x3,y3;
      x2  = fx_mul(x,x);
      xy  = fx_mul(y,x);
      y2  = fx_mul(y,y);
      x2y = fx_mul(x2,y);
      y2x = fx_mul(y2,x);
      x3  = fx_mul(x2,x);
      y3  = fx_mul(y2,y);


      //Compute P3(X,Y)
      p30->all=0;
      *p30 = fx_add(*p30,fx_mul(p3->a[0],x3));
      *p30 = fx_add(*p30,fx_mul(p3->a[1],x2y));
      *p30 = fx_add(*p30,fx_mul(p3->a[2],y2x));
      *p30 = fx_add(*p30,fx_mul(p3->a[3],y3));
      *p30 = fx_add(*p30,fx_mul(p3->a[4],x2));
      *p30 = fx_add(*p30,fx_mul(p3->a[5],xy));
      *p30 = fx_add(*p30,fx_mul(p3->a[6],y2));
      *p30 = fx_add(*p30,fx_mul(p3->a[7],x));
      *p30 = fx_add(*p30,fx_mul(p3->a[8],y));
      *p30 = fx_add(*p30,p3->a[9]);
      //Compute P2(X,Y)
      p20->all=0;
      *p20 = fx_add(*p20,fx_mul(p2->b[0],x2));
      *p20 = fx_add(*p20,fx_mul(p2->b[1],xy));
      *p20 = fx_add(*p20,fx_mul(p2->b[2],y2));
      *p20 = fx_add(*p20,fx_mul(p2->b[3],x));
      *p20 = fx_add(*p20,fx_mul(p2->b[4],y));
      *p20 = fx_add(*p20,p2->b[5]);
      //Compute P1(X,Y)
      p10->all=0;
      *p10 = fx_add(*p10,fx_mul(p1->c[0],x));
      *p10 = fx_add(*p10,fx_mul(p1->c[1],y));
      *p10 = fx_add(*p10,p1->c[2]);*/


    s1 = fx_add(fx_mul(mfixed(6),a[0]),fx_add(fx_mul(mfixed(2),a[4]),fx_add(fx_mul(mfixed(6),fx_mul(a[0],x)),fx_mul(mfixed(2),fx_mul(a[1],y)))));

    r1 = fx_add(fx_mul(mfixed(2),fx_mul(a[2],x)),fx_add(fx_mul(mfixed(2),fx_mul(a[2],y)),fx_add(a[2],(fx_add(a[1],(fx_add(a[5],(fx_mul(mfixed(2),fx_mul(a[2],y))))))))));


    r2 = fx_add(fx_mul(x2,fx_mul(mfixed(3),a[0])),fx_add(fx_mul(mfixed(2),fx_mul(a[1],fx_mul(y,x))),fx_add(fx_mul(a[2],y2),fx_add(fx_mul(r2_c1,x),fx_add(fx_mul(r2_c2,y),fx_add(a[0],(fx_add(a[4],(a[7])))))))));

  
    q1 = fx_add(fx_mul(mfixed(2),fx_mul(a[2],x)),fx_add(fx_mul(mfixed(6),fx_mul(a[3],y)),fx_add(fx_mul(mfixed(6),a[3]),fx_add(fx_mul(mfixed(2),a[6]),fx_mul(mfixed(6),fx_mul(a[3],y))))));

    q2 = fx_add(fx_mul(a[1],x2),fx_add(fx_mul(mfixed(2),fx_mul(a[2],fx_mul(x,y))),fx_add(fx_mul(mfixed(3),fx_mul(a[3],y2)),fx_add(fx_mul(q2_c1,x),fx_add(fx_mul(q2_c2,y),fx_add(a[3],(fx_add(a[6],(a[8])))))))));

    q3 = fx_add(fx_mul(a[0],x3),fx_add(fx_mul(a[1],fx_mul(x2,y)),fx_add(fx_mul(a[2],fx_mul(x,y2)),fx_add(fx_mul(a[3],y3),fx_add(fx_mul(a[4],x2),fx_add(fx_mul(a[5],fx_mul(x,y)),fx_add(fx_mul(a[6],y2),fx_add(fx_mul(a[7],x),fx_add(fx_mul(a[8],y),a[9])))))))));
}


