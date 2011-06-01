#include "etrange.h"

inline mfixed fx_mul(mfixed A, mfixed B);
inline mfixed fx_add(mfixed A, mfixed B);

void etrange_initialize(P3* p3, P2* p2, P1* p1, P0* p0) {
	//CONSTANTS
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

}
