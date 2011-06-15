/************************************************************
 *
 * File mfixed.h
 *
 * Simple Fixed point math
 *
 * 16 bis.16bits
 *
 ************************************************************/

// fixed point taype definition
// This works only for big endian
typedef union {
    struct {
        short h;
        unsigned short l;
    } ;
    int all;
}mfixed;

// fixed point multiplication
inline mfixed fx_mul  (mfixed A, mfixed B)
{

    return (mfixed)( ((A.l*B.l)>> 16) +
            (A.l * B.h )    +
            (A.h * B.l )    +
            ((A.h*B.h) << 16)
            );
}

// fixed point addition
inline mfixed fx_add (mfixed A, mfixed B)
{
    return (mfixed)(A.all + B.all);
}

