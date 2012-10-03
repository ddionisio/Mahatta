#include "engine_local.h"

/////////////////////////////////////
// Name:	EngineRender
// Purpose:	render everything
// Output:	stuff rendered
// Return:	FALSE if something went
//			wrong
/////////////////////////////////////
s32 EngineRender()
{
	if(!g_GFX || !g_ge)
		return TRUE;

//	g_GFX->GFXClear(GFXCLEAR_ZBUFFER | GFXCLEAR_STENCIL, 0, 0, GFXCOLOR_XRGB(0,0,0), 1.0f, 0);
	g_GFX->GFXClear(GFXCLEAR_ALL, 0, 0, GFXCOLOR_XRGB(0,0,0), 1.0f, 0);

	g_GFX->GFXBegin();

	//game display
	g_ge->RenderFrame();

	//UI display
//	g_uie->RenderFrame();
	
	g_GFX->GFXEnd();

	g_GFX->GFXPresent(0);

	return TRUE;
}