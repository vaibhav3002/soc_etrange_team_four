/************************************************************
 *
 *      File : mfixed.h
 *      Author: A. Schindler
 *      Credits: T. Graba
 *      Telecom ParisTECH
 *
 *      Adapted the mfixed type to C++ to be used with
 *      SystemC
 *
 ************************************************************/

#ifndef __MFIXED2__
#define __MFIXED2__
#include "systemc.h"

class mfixed {
  private:
    unsigned char h;
    short l;
  public:

    // overloaded constructors
    mfixed ();

    mfixed(int, int );
    
    mfixed(int);

    mfixed(uint32_t);

    mfixed(double);

    /* operators */

    bool operator == (const mfixed & rhs) const;

    mfixed& operator = (const mfixed& rhs);

   friend ostream& operator << ( ostream& os,  mfixed const & v );

    friend mfixed operator + (const mfixed&    u, const mfixed&    v);


   friend void sc_trace(sc_trace_file *tf, const mfixed & v,
				const std::string & NAME );

   

    void fx_copy (mfixed A, mfixed B);

    /* converts to float */
    friend float to_float(mfixed A);

// fixed point multiplication
    friend mfixed fx_mul  (mfixed A, mfixed B);

// fixed point addition
    friend mfixed fx_add (mfixed A, mfixed B);

    /* extracts digit and fractional part */
    friend unsigned short getH(mfixed A);
    friend short getL(mfixed A);

};
#endif
