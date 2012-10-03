#include "gfx_i.h"
#include "GFX\gfx_bkgrndfx.h"

#define MAX_ALPHA	255

//message for FX functions
typedef enum {
	BKFXM_CREATE,
	BKFXM_UPDATE,
	BKFXM_DISPLAY,
	BKFXM_DESTROY
} FXmessage;

//signals for FX update
typedef enum {
	BKFXsignal_animate,
	BKFXsignal_noanimate
} BKFXsignal;

/**********************************************************
***********************************************************

  Data structs for the background FXs

***********************************************************
**********************************************************/

//
// background FX main data struct for each type
//

#define GFXCLRVTX (GFXFVF_XYZRHW | GFXFVF_DIFFUSE) //xyzw and color

typedef struct _gfxClrVtx {
    f32 x, y, z, rhw;
    u32 color;
} gfxClrVtx;

//fade in/out
typedef struct _fadeinout {
	u8			r,g,b;				//the color
	s32			curA;				//current alpha
	u8			bFadeIn;			//are we fading to the color?
		
	GFXVTXBUFF  *vtx;				//four point vtx
} fadeinout;

typedef struct _imgfadeinout {
	hTXT	theImg;		//image to transit to
	s32		curA;		//current alpha
	u8		bFadeIn;	//are we fading to the color?
} imgfadeinout;

//curtain
typedef struct _curtain {
	hTXT curtainImg;		//curtain to fall down
	u32 fadeColor;	//fadeout color after curtain goes down
} curtain;

//fade away
/*
typedef struct _fadeaway_init {
	BYTE r,g,b;
	double delay;
} curtain_init;*/
typedef struct _fadeaway {
	u8 		rStart,gStart,bStart;
	u8 		rEnd,gEnd,bEnd;

	s32		r,g,b;		//current r/g/b

	s32		curA;		//current alpha
} fadeaway;

/**********************************************************
***********************************************************

  The methods for all FX

***********************************************************
**********************************************************/

//fade in/out
s32 bkFadeInOutFunc(hBKFX bkFX, u32 message, LPARAM dumbParam, WPARAM otherParam)
{
	fadeinout *thisData = (fadeinout *)bkFX->Data;

	switch(message)
	{
	case BKFXM_UPDATE:
		{
			f32 t = bkFX->time/bkFX->timeDelay;
			
			if(thisData->bFadeIn)
				thisData->curA = (s32)(t*MAX_ALPHA);
			else
				thisData->curA = (s32)(MAX_ALPHA - (t*MAX_ALPHA));

			if(thisData->curA > MAX_ALPHA) thisData->curA = MAX_ALPHA;
			else if(thisData->curA < 0) thisData->curA = 0;

			gfxClrVtx *vtxData;

			u32 clr = D3DCOLOR_RGBA(thisData->r, thisData->g, thisData->b, thisData->curA);

			if(VtxLock(thisData->vtx, 0, 0,0, (void**)&vtxData, 0))
			{
				vtxData[0].color = clr;
				vtxData[1].color = clr;
				vtxData[2].color = clr;
				vtxData[3].color = clr;

				VtxUnlock(thisData->vtx, 0);
			}

			if(t >= 1)
			{
				if(thisData->curA == MAX_ALPHA) //now fade out
					thisData->bFadeIn = FALSE;
				else if(thisData->curA == 0)	//check to see if we are over
					return GFXRET_BREAK;
			}
		}
		break;

	case BKFXM_DISPLAY:
		if(thisData->bFadeIn)
		{
			TextureBegin();
			GFXScreenDisplay(0,0,0);
			TextureEnd();
		}

		VtxSet(thisData->vtx);
		
		g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

		VtxUnset(thisData->vtx);
		break;

	case BKFXM_CREATE:
		{
			fadeinout_init *initData = (fadeinout_init *)dumbParam;

			if(!initData)
				return GFXRET_FAILURE;

			fadeinout *newData = (fadeinout *)MemAlloc(sizeof(fadeinout));

			if(!newData)
				return GFXRET_FAILURE;

			newData->r = initData->r; 
			newData->g = initData->g;
			newData->b = initData->b;

			//start at alpha = 0 and bFadeIn = true
			newData->curA    = 0;
			newData->bFadeIn = TRUE;

			//create flat plane
			newData->vtx = GFXCreateVtxBuff(sizeof(gfxClrVtx), 4, GFXUSAGE_WRITEONLY, GFXCLRVTX, GFXPOOL_MANAGED);
			if(!newData->vtx)
				return GFXRET_FAILURE;

			gfxClrVtx *vtxData;

			u32 clr = D3DCOLOR_RGBA(newData->r, newData->g, newData->b, newData->curA);

			if(VtxLock(newData->vtx, 0, 0,0, (void**)&vtxData, 0))
			{
				vtxData[0].x = 0; vtxData[0].y = 0; vtxData[0].z = 0; vtxData[0].rhw = 1.0f;
				vtxData[0].color = clr;

				vtxData[1].x = 0; vtxData[1].y = g_mode.height; vtxData[1].z = 0; vtxData[1].rhw = 1.0f;
				vtxData[1].color = clr;

				vtxData[2].x = g_mode.width; vtxData[2].y = g_mode.height; vtxData[2].z = 0; vtxData[2].rhw = 1.0f;
				vtxData[2].color = clr;

				vtxData[3].x = g_mode.width; vtxData[3].y = 0; vtxData[3].z = 0; vtxData[3].rhw = 1.0f;
				vtxData[3].color = clr;

				VtxUnlock(newData->vtx, 0);
			}

			bkFX->Data = newData;

			//get a snap-shot of the screen
			GFXScreenCapture();
		}
		break;

	case BKFXM_DESTROY:
		{
			if(thisData)
				GFXDestroyVtxBuff(thisData->vtx);
		}
		break;
	}

	return GFXRET_SUCCESS;
}

//image fade in/out
s32 bkImgFadeInOutFunc(hBKFX bkFX, u32 message, LPARAM dumbParam, WPARAM otherParam)
{
	imgfadeinout *thisData = (imgfadeinout *)bkFX->Data;

	switch(message)
	{
	case BKFXM_UPDATE:
		{
			f32 t = bkFX->time/bkFX->timeDelay;
			
			if(thisData->bFadeIn)
				thisData->curA = (s32)(t*MAX_ALPHA);
			else
				thisData->curA = (s32)(MAX_ALPHA - (t*MAX_ALPHA));

			if(thisData->curA > MAX_ALPHA) thisData->curA = MAX_ALPHA;
			else if(thisData->curA < 0) thisData->curA = 0;

			if(t >= 1)
			{
				if(thisData->curA == MAX_ALPHA) //now fade out
					thisData->bFadeIn = FALSE;
				else if(thisData->curA == 0)	//check to see if we are over
					return GFXRET_BREAK;
			}
		}
		break;

	case BKFXM_DISPLAY:
		{
			TextureBegin();
			
			if(thisData->bFadeIn)
				GFXScreenDisplay(0,0,0);

			gfxBlt bltDat={0};
			bltDat.clr = GFXCOLOR_RGBA(255, 255, 255, thisData->curA);

			thisData->theImg->StretchBlt(0, 0, g_mode.width,g_mode.height,0,&bltDat);

			TextureEnd();
		}
		break;

	case BKFXM_CREATE:
		{
			imgfadeinout_init *initData = (imgfadeinout_init *)dumbParam;

			if(!initData)
				return GFXRET_FAILURE;

			imgfadeinout *newData = (imgfadeinout *)MemAlloc(sizeof(imgfadeinout));

			if(!newData)
				return GFXRET_FAILURE;

			//start at alpha = 0 and bFadeIn = true
			newData->curA    = 0;
			newData->bFadeIn = TRUE;

			//add ref to texture
			newData->theImg = initData->theImg;
			newData->theImg->AddRef();

			bkFX->Data = newData;

			//get a snap-shot of the screen
			GFXScreenCapture();
		}
		break;

	case BKFXM_DESTROY:
		if(thisData)
			SAFE_RELEASE(thisData->theImg);
		break;
	}

	return GFXRET_SUCCESS;
}

//fade away
s32 bkFadeAwayFunc(hBKFX bkFX, u32 message, LPARAM dumbParam, WPARAM otherParam)
{
	fadeaway *thisData = (fadeaway *)bkFX->Data;

	switch(message)
	{
	case BKFXM_UPDATE:
		{
			f32 t = bkFX->time/bkFX->timeDelay;

			thisData->curA = (s32)(MAX_ALPHA - (t*MAX_ALPHA));

			if(thisData->curA < 0) thisData->curA = 0;

			thisData->r = thisData->rStart + t*(thisData->rEnd-thisData->rStart);
			thisData->g = thisData->gStart + t*(thisData->gEnd-thisData->gStart);
			thisData->b = thisData->bStart + t*(thisData->bEnd-thisData->bStart);

			if(t >= 1)
			{
				if(thisData->curA == 0)	//check to see if we are over
					return GFXRET_BREAK;
			}
		}
		break;

	case BKFXM_DISPLAY:
		{
			gfxBlt bltDat={0};
			bltDat.clr = GFXCOLOR_RGBA(thisData->r,thisData->g,thisData->b,thisData->curA);

			TextureBegin();
			
			GFXScreenDisplay(0,0,&bltDat);

			TextureEnd();
		}
		break;

	case BKFXM_CREATE:
		{
			fadeaway_init *initData = (fadeaway_init *)dumbParam;

			if(!initData)
				return GFXRET_FAILURE;

			fadeaway *newData = (fadeaway *)MemAlloc(sizeof(fadeaway));

			if(!newData)
				return GFXRET_FAILURE;

			//start at alpha = 255
			newData->curA    = MAX_ALPHA;
			
			newData->rEnd = initData->r;
			newData->gEnd = initData->g;
			newData->bEnd = initData->b;

			newData->rStart = newData->gStart = newData->bStart = MAX_ALPHA;

			bkFX->Data = newData;

			//get a snap-shot of the screen
			GFXScreenCapture();
		}
		break;

	case BKFXM_DESTROY:
		break;
	}

	return GFXRET_SUCCESS;
}

//
// A bunch of FX functions
//
static BKEFFECT BKEffectTable[eBKFX_MAX] = {bkFadeInOutFunc,bkImgFadeInOutFunc,bkFadeAwayFunc};

/////////////////////////////////////
// Name:	BKFXDestroy
// Purpose:	destroys given bkFX
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
void F_API BKFXDestroy(hBKFX bkFX)
{
	if(bkFX)
	{
		if(bkFX->Effect)
			bkFX->Effect(bkFX, BKFXM_DESTROY, 0, 0);

		if(bkFX->Data)
			MemFree(bkFX->Data);

		MemFree(bkFX);
	}

	bkFX=0;
}

/////////////////////////////////////
// Name:	BKFXCreate
// Purpose:	creates a background FX
// Output:	none
// Return:	the new background FX
/////////////////////////////////////
hBKFX F_API BKFXCreate(u32 type, f32 delay, void *initStruct)
{
	hBKFX newBkFX = (hBKFX)MemAlloc(sizeof(gfxBkFX));

	if(!newBkFX) { LogMsg(LOG_FILE, L"BKFXCreate", L"Unable to allocate new background FX"); return 0; }

	newBkFX->Type = type;

	newBkFX->time = 0;
	newBkFX->timeDelay = delay;

	newBkFX->Effect = BKEffectTable[type];

	if(newBkFX->Effect(newBkFX, BKFXM_CREATE, (LPARAM)initStruct, 0) != GFXRET_SUCCESS)
	{ LogMsg(LOG_FILE, L"BKFXCreate", L"Unable to allocate new background FX"); BKFXDestroy(newBkFX); return 0; }

	return newBkFX;
}

/////////////////////////////////////
// Name:	BKFXUpdate
// Purpose:	updates and displays the
//			background FX
// Output:	stuff displayed
// Return:	-1 if duration
//			is over or FX expired
/////////////////////////////////////
s32 F_API BKFXUpdate(hBKFX bkFX)
{
	bkFX->time += g_time;

	s32 ret = bkFX->Effect(bkFX, BKFXM_UPDATE, 0, 0);

	if(bkFX->time >= bkFX->timeDelay)
		bkFX->time = 0;

	return ret;
}

/////////////////////////////////////
// Name:	BKFXDisplay
// Purpose:	display background FX
// Output:	stuff
// Return:	success
/////////////////////////////////////
s32 F_API BKFXDisplay(hBKFX bkFX)
{
	return bkFX->Effect(bkFX, BKFXM_DISPLAY, 0, 0);
}