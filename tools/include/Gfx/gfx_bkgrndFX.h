#ifndef _gfx_bkgrndfx_h
#define _gfx_bkgrndfx_h

typedef enum {
	eBKFX_FADEINOUT,	//in  - fade from color to background
						//out - fade from background to color
	eBKFX_IMGFADEINOUT,	//same as above...except transition would be an image
	eBKFX_FADEAWAY,		//fade the old scene away
	eBKFX_MAX
} eBKFXType;

/**********************************************************
***********************************************************

  The init structs you must fill up for FX

***********************************************************
**********************************************************/

//
// background FX init struct for each type
//
typedef struct _fadeinout_init {
	u8 r,g,b;
} fadeinout_init;

typedef struct _imgfadeinout_init {
	hTXT theImg;	//image to transit to
} imgfadeinout_init;

typedef struct _fadeaway_init {
	u8 r,g,b;
} fadeaway_init;

#endif