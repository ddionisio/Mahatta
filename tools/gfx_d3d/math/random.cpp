#include "common.h"
#include "GFX\gfx_math.h"

//return a random double-precision number between 0 and 1
f64 F_API RandDouble()
{
	//make sure all bytes of the int are random
	u32 i = rand() + (rand() << 15); 
	u32 j = rand() + (rand() << 15); 
	f64 k = (f64)i / (f64)(u32)(0x3fffffff) + (f64)j / (f64)(u32)(0x3fffffff) / (f64)(u32)(0x3fffffff);

	//return a number between 0 and 1...
	return k;
}

f32 F_API RandRngFloat(f32 a, f32 b)
{
	s32 r;
    f32	x;
    
	r = rand ();
    x = (f32)(r & 0x7fff) /
		(f32)0x7fff;
    return (x * (b - a) + a);
}

s32 F_API RandRngInt(s32 a1, s32 a2)
{
	s32 tempnum = a2 - a1 + 1;
	s32 tempnum2;

	tempnum2 = a2 - (rand()%tempnum);

	return tempnum2;
}