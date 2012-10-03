#include "engine_local.h"

/////////////////////////////////////
// Name:	EngineLoadDisplay
// Purpose:	display an image instantly
//			use this for load screens.
// Output:	image displayed when this is
//			called
// Return:	none
/////////////////////////////////////
void EngineLoadDisplay(hTXT txt, f32 x, f32 y, u8 bClear)
{
	if(!g_GFX)
		return;

	if(bClear)
		g_GFX->GFXClear(GFXCLEAR_TARGET, 0, 0, GFXCOLOR_XRGB(0,0,0), 1.0f, 0);

	if(txt)
	{
		g_GFX->GFXBegin();

		g_GFX->TextureBegin();
		
		txt->Blt(x, y, 0, 0);
		
		g_GFX->TextureEnd();

		g_GFX->GFXEnd();
	}

	g_GFX->GFXPresent(0);
}

/////////////////////////////////////
// Name:	EngineLoadDisplayText
// Purpose:	display text at bottom instantly
//			use this for load screens.
// Output:	text displayed when this is
//			called
// Return:	none
/////////////////////////////////////
void EngineLoadDisplayText(hFNT fnt, const tCHAR *str, ...)
{
	if(!fnt) return;

	tCHAR buff[DMAXCHARBUFF];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, str);
	_vsnwprintf(buff, sizeof(buff), str, valist);
	va_end(valist);

	iPoint fntSize;
	fnt->GetStrSize((tCHAR*)buff, &fntSize);

	gfxMode mode; g_GFX->GFXGetMode(&mode);

	iRect area;
	area.left   = 0;
	area.top    = mode.height-fntSize.y;
	area.right  = mode.width;
	area.bottom = mode.height;

	g_GFX->GFXClear(GFXCLEAR_TARGET, 1, &area, GFXCOLOR_XRGB(0,0,0), 1.0f, 0);

	g_GFX->GFXBegin();

	fnt->TextBox(buff, -1, &area, 0, 0xffffffff);

	g_GFX->GFXEnd();

	g_GFX->GFXPresent(0);  //update frame on display
}