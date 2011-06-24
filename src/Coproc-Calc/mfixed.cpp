#include "mfixed2.h"

mfixed::mfixed () 
{
    h = 0;
    l = 0;
}

mfixed::mfixed(int a)
{
    l = a;
    h = 0;
}

 mfixed::mfixed (int a, int b) 
{
    l = a;
    h = b;
    }

 mfixed::mfixed (double a) 
{
    l = (int)(a);// && 0xFFFF0000));
    h = (short)((int)(a * (1<<16)));
    //printf ("A   0x%x 0x%x\n", h, l);
}


bool mfixed::operator == (const mfixed & rhs) const {
    return (rhs.h == h && rhs.l == l );
}

mfixed& mfixed::operator = (const mfixed& rhs)  {
    h = rhs.h;
    l = rhs.l;
    return *this;
}

//inline mfixed mfixed::operator + (const mfixed& rhs) {
mfixed operator + (const mfixed&    u, const mfixed&    v)
{
    mfixed temp;
    temp.h = u.h+v.h;
    temp.l = u.l+v.l;
    return (temp);
}

void sc_trace(sc_trace_file *tf, const mfixed & v,
				    const std::string & NAME ) {
    sc_trace(tf,v.h, NAME + ".h");
    sc_trace(tf,v.l, NAME + ".l");
}

// fixed point multiplication
mfixed fx_mul  (mfixed A, mfixed B)
{
    mfixed temp;
    temp.l =  ((A.l*B.l) +
	       (A.l * B.h ) * (1 >> 16)  +
	       (A.h * B.l ) * (1 << 16)  +
	       (A.h*B.h) * (1 << 16));
	

    //temp.l = (A.l * B.h ) >> 16 + (A.h * B.l ) >> 16 + (A.l*B.l);
    temp.h =  (A.l * B.h )    +
	(A.h * B.l )    +
	((A.h*B.h) >> 16);

//5; /*(short)((int)(((( ((A.l*B.l)<< 16) +
    /*(A.l * B.h )    +
            (A.h * B.l )    +
				    ((A.h*B.h) >> 16)))) * (1<<16)));


/*(short)((int)((( ((A.l*B.l) << 16) +
			      (A.l * B.h )    +
			      (A.h * B.l )    +
			      ((A.h*B.h) >> 16)))) * (1<<16));
*/
    //cout << "mul" << temp << endl;

    /*return (mfixed)( ((A.l*B.l) << 16) +
		     (A.l * B.h )    +
		     (A.h * B.l )    +
		     ((A.h*B.h) >> 16)
		     );*/

    return temp;
}

// fixed point addition
mfixed fx_add (mfixed A, mfixed B)
{
    return A+B;
}

float to_float(mfixed A)
{
    float fl;
    fl = (float)A.h / (float)65535;
    fl += A.l;
    return fl;
}

ostream& operator << ( ostream& os,  mfixed const & v ) {
    float fl;
    fl = (float)v.h / (float)65535;
    fl += v.l;
    os << fl;
    return os;
}
