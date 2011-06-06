/************************************************************
 *
 * File mfixed.h
 *
 * Simple Fixed point math
 *
 * 16 bis.16bits
 *
 ************************************************************/

#ifndef _MFIXED_H
#define _MFIXED_H


// fixed point taype definition
// This works only for big endian
typedef union {
    struct {
        short h;
        unsigned short l;
    } ;
    int all;
}mfixed;


// The following functions are implemented in mfixed.c and their prototypes are given directly in etrange.c (compilation issues arose)
//	// fixed point multiplication
//	inline mfixed fx_mul  (mfixed A, mfixed B);
//	// fixed point addition
//	inline mfixed fx_add (mfixed A, mfixed B);

#endif
