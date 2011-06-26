#ifndef __MFIXED2__
#define __MFIXED2__
#include "systemc.h"

class mfixed {
  private:
    unsigned short h;
    short l;
  public:

    // constructor
    mfixed ();

    mfixed(int, int );
    
    mfixed(int);

    mfixed(uint32_t);

    mfixed(double);


    bool operator == (const mfixed & rhs) const;

    mfixed& operator = (const mfixed& rhs);

    friend void sc_trace(sc_trace_file *tf, const mfixed & v,
				const std::string & NAME );

    friend ostream& operator << ( ostream& os,  mfixed const & v );


    void fx_copy (mfixed A, mfixed B);

    friend float to_float(mfixed A);

// fixed point multiplication
    friend mfixed fx_mul  (mfixed A, mfixed B);

// fixed point addition
    friend mfixed fx_add (mfixed A, mfixed B);

    //friend mfixed operator + (const mfixed& rhs);
    friend mfixed operator + (const mfixed&    u, const mfixed&    v);
    
    friend unsigned short getH(mfixed A);
    friend short getL(mfixed A);

};
#endif
